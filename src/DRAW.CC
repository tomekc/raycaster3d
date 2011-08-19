/*-----------------96-09-30 15:50-------------------
  Draw - rendering widoku
--------------------------------------------------*/
#include "game.h"
#include "defs.h"
#include "fixed.h"

//#define DEBUG

#ifdef DEBUG
extern FILE *debugfp;
#endif

static int ray,ray_angle;
static int cast_x,cast_y;      // boolean casting flags
static int next_grid_x,next_grid_y;  // przy szukaniu kolejnych przeciec
static int x,y;                // actual position of ray
static int xcross_x,xcross_y;  // X ray hit point
static int ycross_x,ycross_y;  // Y ray hit point
static int grid_x,grid_y;      // gridlines
static dword dist_x, dist_y;
static int step_x,step_y;      // przyrost przy szukaniu przeciëç
static int maze_x,maze_y;        // pozycja w labiryncie <0..63>
static int hit_x, hit_y;       // znacznik trafienia
static int wall_x, wall_y;     // wartoûç trafionego klocka
static int next_w_x, next_w_y;
static int column,scale;

static int view_width;

static int t_tan;               // tymczasowa zmienna z tangensem
static int t_ctg;

static dword z_buffer[640];     // odlegíoûci dla wszystkich pask¢w ekranu

void Game::draw (PlayerObj *player)
{
  view_width = player->get_w ();
//***( Cast ray thru every screen column )**********************************
  for ( ray=0; ray<view_width; ray++ )
  {
    ray_angle = (pixel_delta[ray]+player->geta() )&(ANGLES-1);
    t_tan = tangent[ray_angle];
    if ( t_tan ) t_ctg = odwrot(t_tan);
    x = player->getx();
    y = player->gety();
    xcross_x = x;    //set up incase any of these values are uninitialised
    xcross_y = y;    //due to vertical and horizontal rays
    ycross_x = x;
    ycross_y = y;
    step_x = step_y = 0;
//***( Work out gridlines )*************************************************
    if (ray_angle == ANG90)
    {
      cast_x = FALSE;
      cast_y = TRUE;
      next_grid_y = (1 * 65536);
      t_ctg = 0;  // by krok X byí zerowy
    }
    //vertical line - downward
    else if (ray_angle == ANG270)
    {
      cast_x = FALSE;
      cast_y = TRUE;
      next_grid_y = (-1 * 65536);
      t_ctg = 0;  // by krok X byí zerowy
    }
    //horz line - right
    else if (ray_angle == 0 )
    {
      cast_x=TRUE;
      cast_y=FALSE;
      next_grid_x = (1 * 65536);
      t_tan = 0;  // by krok Y byí zerowy
    }
    //horz line - left
    else if (ray_angle == ANG180)
    {
      cast_x=TRUE;
      cast_y=FALSE;
      next_grid_x = (-1 * 65536);
      t_tan = 0;  // by krok Y byí zerowy
    }
    // positive x, positive y
    else if ( ray_angle<ANG90 && ray_angle>0 )
    {
      cast_x=TRUE;
      cast_y=TRUE;
      next_grid_x = (1 * 65536);
      next_grid_y = (1 * 65536);
      step_x = t_ctg;
      step_y = t_tan;
   }
    // positive x, negative y
    else if ( ray_angle<ANG360 && ray_angle>ANG270 )
    {
      cast_x=TRUE;
      cast_y=TRUE;
      next_grid_x = ( 1 * 65536);
      next_grid_y = (-1 * 65536);
      step_x = -t_ctg;
      step_y = t_tan;
    }
    // negative x, positive y
    else if ( ray_angle < ANG180 && ray_angle > ANG90 )
    {
      cast_x=TRUE;
      cast_y=TRUE;
      next_grid_x = (-1 * 65536);
      next_grid_y = ( 1 * 65536);
      step_x = t_ctg;
      step_y = -t_tan;
    }
    // negative x, negative y
    else // if (ray_angle > ANG180 && ray_angle < ANG270 )
    {
      cast_x=TRUE;
      cast_y=TRUE;
      next_grid_x = (-1 * 65536);
      next_grid_y = (-1 * 65536);
      step_x = -t_ctg;
      step_y = -t_tan;
    }
//***( Find first intersection )********************************************
    /*
     ponißsze zmienne bëdÜ chyba ußyte tylko raz, po to, by ustaliç
     najblißszÜ nastëpnÜ (lub poprzedniÜ - zaleßy od kierunku promienia)
     linië przeciëcia. Uwaga - ußyíem odwrotnej nomenklatury niß w "gurus"
     - promie§ X oznacza, ße szukamy przeciëcia z prostÜ _pionowÜ_.
    */
    int next_b_x, next_b_y;
    next_b_x = (next_grid_x>0)?FIXED1:0;
    next_b_y = (next_grid_y>0)?FIXED1:0;
    next_w_x = (next_grid_x<0)?-FIXED1:0;
    next_w_y = (next_grid_y<0)?-FIXED1:0;
    /* Szukamy pierwszego przeciëcia. Zmienne [xy]_cross bedÜ zawieraíy
       wsp¢írzëdne pierwszego przeciëcia.     */
    if ( cast_x )
    {
      grid_x = ( x & 0xFFFF0000 ) + next_b_x;
      xcross_x = grid_x;
      xcross_y = y + mnoz( grid_x-x, t_tan );
    }
    if ( cast_y )
    {
      grid_y = ( y & 0xFFFF0000 ) + next_b_y;
      ycross_y = grid_y;
      ycross_x = x + mnoz( grid_y-y, t_ctg );
    }
//***( Continue raycastnig )************************************************
 /* Gí¢wna pëtla ray-castera bëdzie lecieç, dopuki wszystkie rzucane
    promienie w coû walnÜ. Trzeba uwaßaç, bo przy odrobinie szczëûcia pëtla
    moße sië okazaç niesko§czona....
 */
    hit_x = hit_y = FALSE;
    dist_x = dist_y = 0xFFFFFFFF;
    while ( cast_x || cast_y )
    {
 //*** promie§ X ******************************************
      if ( !hit_x && cast_x )   //*** Czy nie trafionio promieniem X ?
      {
        maze_x = ( xcross_x + next_w_x )>>GRIDSHIFT;
        maze_y = xcross_y >> GRIDSHIFT;
        wall_x = tileat (maze_x,maze_y);
        if ( wall_x > 0 )   // jest klocek
        {
          hit_x = TRUE;
          cast_x = FALSE;
          if ( sinus[ray_angle]==0 )
            dist_x = abs(xcross_x - x);
          else
            dist_x = dziel( xcross_y-y, sinus[ray_angle] );
        }
        else  // Jeszcze ni ma
        {
          xcross_x += next_grid_x;
          xcross_y += step_y;
        }
      }

 //*** promie§ Y ******************************************
      if ( !hit_y && cast_y )
      {
        maze_x = ycross_x >> GRIDSHIFT;
        maze_y = ( ycross_y + next_w_y )>>GRIDSHIFT;
        wall_y = tileat(maze_x, maze_y);
        if ( wall_y > 0 )
        {
          hit_y = TRUE;
          cast_y = FALSE;
          if ( cosinus[ray_angle]==0 )
            dist_y = abs(ycross_y - y);
          else
            dist_y = dziel( ycross_x-x, cosinus[ray_angle] );
        }
        else
        {
          ycross_x += step_x;
          ycross_y += next_grid_y;
        }
      }
    } // END WHILE casting
//***( Rendering )**********************************************************
#ifdef DEBUG
    fprintf (debugfp,"r=%d a=%d {%d} ",ray,ray_angle,pixel_delta[ray]);
#endif
    if ( dist_x < dist_y )
    {
#ifdef DEBUG
       fprintf (debugfp,"[X] ");
#endif
      column = (xcross_y >> 10) & 63;
      if ( next_grid_x > 0 ) column = 63 - column;
      scale = mnoz( dist_x>>10, fisheye_table[ray] );
      z_buffer[ray] = dist_x;
      DC_Sliver ( ray, column, scale, wall_tex[wall_x-1] );
    }
    else
    {
#ifdef DEBUG
      fprintf (debugfp,"[Y] ");
#endif
      column = (ycross_x >> 10) & 63;
      if ( next_grid_y < 0 ) column = 63 - column;
      scale = mnoz( dist_y>>10, fisheye_table[ray] );
      z_buffer[ray] = dist_y;
      DC_Sliver ( ray, column, scale, wall_tex[wall_y-1] );
    }
#ifdef DEBUG
      fprintf (debugfp,"crossx %x %x distx %x | crossy %x %x disty %x\n",xcross_x,xcross_y,dist_x,ycross_x,ycross_y,dist_y);
#endif



  } // NEXT ray
}
