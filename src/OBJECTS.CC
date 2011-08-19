#include "game.h"
#include "defs.h"
#include "fixed.h"

//===========================================================================
//
//    ACTOR OBJECT
//
//===========================================================================

ActorObj::ActorObj (Game *gam)
{
  parent = gam;
  objtype = o_actor;
}

void ActorObj::position (int x, int y, int a)
{
  xpos = x;
  ypos = y;
  angle = a & (ANGLES-1);
  gridx = x >> GRIDSHIFT;
  gridy = y >> GRIDSHIFT;
}

void ActorObj::rotate (int a)
{
  angle += a;
  angle &= (ANGLES-1);
}

// funkcja check Move zwraca TRUE, jesli ruch jest poprawny
int ActorObj::checkMove (int x,int y)
{
  int xl,xh,yl,yh;
  int cx,cy;

  xl = (x - PLAYERSIZE)>> GRIDSHIFT;
  yl = (y - PLAYERSIZE)>> GRIDSHIFT;
  xh = (x + PLAYERSIZE)>> GRIDSHIFT;
  yh = (y + PLAYERSIZE)>> GRIDSHIFT;
  // sprawdz czy wlazi na sciane
  for ( cx=xl; cx<=xh; cx++ )
  {
    for ( cy=yl; cy<=yh; cy++ )
    {
      if ( parent->tileat(cx,cy) ) return FALSE;
    }
  }
  return TRUE;
}

void ActorObj::move (int speed)
{
  int dx,dy;

  dx = mnoz( speed, cosinus[angle] );
  dy = mnoz( speed, sinus[angle] );

  if ( checkMove(xpos+dx, ypos+dy) )
  {
    xpos += dx;
    ypos += dy;
    return;
  }
  // sprawdzenie, czy jest mozliwy ruch po chociaz jednej osi

  if ( checkMove(xpos+dx, ypos) )
  {
    xpos += dx;
    return;
  }

  if ( checkMove(xpos, ypos+dy) )
  {
    ypos += dy;
    return;
  }


}

void ActorObj::strafe (int speed)
{
}
//===========================================================================
//
//    PLAYER OBJECT
//
//===========================================================================

PlayerObj::PlayerObj (Game *g) : ActorObj(g)
{
  objtype = o_player;
  fisheye = pixdelta = NULL;
  double_buffer = NULL;
}

PlayerObj::~PlayerObj ()
{
}

void PlayerObj::destroy_view ()
{
  delete [] fisheye;
  delete [] pixdelta;
  destroy_bitmap (double_buffer);
}

void PlayerObj::init_view (int x,int y,int w,int h)
{
  view_x = x;
  view_y = y;
  view_w = w;
  view_h = h;
  destroy_view ();        // zniszcz stary wju
  double_buffer = create_bitmap (w,h);
  if ( double_buffer == NULL )
  {
    Error ("PlayerObj: not enough memory for double buffer.");
  }
  fisheye = new int[w];
  pixdelta = new int[w];
  generateViewTables (w, fisheye, pixdelta);
}

void PlayerObj::render ()
{
  clear (double_buffer);
  fisheye_table = fisheye;
  pixel_delta = pixdelta;
  DC_ScreenSetup (view_w,view_h/2,double_buffer->dat);
  parent->draw (this);
}

void PlayerObj::toscreen ()
{
  blit (double_buffer, screen, 0,0, view_x, view_y, view_w, view_h );
}





//===========================================================================
//
//    DOOR OBJECT
//
//===========================================================================
DoorObj::DoorObj (Game *g, int x, int y, doortype_t typ)
{
  parent = g;
  objtype = o_door;
  state = d_closed;
  gridx = x;
  gridy = y;
  type = typ;
  pos = 63;
}

inline void DoorObj::open ()
{
  if ( d_open || d_locked ) return;
  state = d_opening;
}

inline void DoorObj::close ()
{
  if ( d_closed || d_locked ) return;
  state = d_closing;
}

void DoorObj::act ()
{
  switch ( state )
  {
    case d_opening: if (pos>0) pos--;
                  else state = d_open;
    case d_closing: if (pos<63) pos++;
                  else state = d_closed;
  }
}


