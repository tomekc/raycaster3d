/***************************************************************************
	GAME.CC

	Uk’ad krwionožny ca’ej maszyny 3D (sercem jest plik CASTER.CC). Tu jest
	funkcja main(), tu si‘ ’aduje z dysku textury.

****************************************************************************/
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <dpmi.h>
#include "game.h"
#include <allegro.h>

#include "bitfont.h"

//#define DEBUG
#define KEYTIMER
//#define KEYSTD

#define LOS_VERSION "1.21"

FILE *debugfp;
static int frames,fps;

BitFont *fon;

//---------------------------------------------------------------------------
// bardzo wazny statyczny obiekt klasy Game!
Game gra;
const char main_resource_file[] = "sryrdata.pak";
//---------------------------------------------------------------------------
// odczyt klawiszy i reakcja na
void poll_keyboard(...)
{
  int spd = WALK_SPEED;
  if ( key[KEY_LSHIFT] || key[KEY_RSHIFT] ) spd = RUN_SPEED;
  if ( key[KEY_UP] ) gra.player1->move (spd);
  if ( key[KEY_DOWN] ) gra.player1->move (-spd);
  if ( key[KEY_LEFT] ) gra.player1->rotate (12);
  if ( key[KEY_RIGHT] ) gra.player1->rotate (-12);
  if ( key[KEY_ESC] ) gra.done_flag = 1;
}
END_OF_FUNCTION(poll_keyboard);
//---------------------------------------------------------------------------
void fps_counter (...)
{
  fps = frames;
  frames =  0;
}
END_OF_FUNCTION (fps_counter);
//---------------------------------------------------------------------------
void LOS_Banner ()
{
  puts ("-=[ Lucyper(tm) Multimedia Operating System "LOS_VERSION" ]=-");
}
//---------------------------------------------------------------------------
void Error (char *str)
{
  allegro_exit ();
  LOS_Banner ();
  printf ("Aborting due to fatal error.\n%s\n", str);
  exit (1);
}
//---------------------------------------------------------------------------
Game::Game ()
{
  light_flag = 1;
  done_flag = 0;
}
//---------------------------------------------------------------------------
void Game::run ()
{
  startup ();
  fon = new BitFont ( res->get("bmini.spf") );
  fon->output (screen);
  clear (screen);

  if ( loadmap ("mapa00")==FAILED )
  {
    puts ("loadmap failed");
    exit(0);
  }
  logo ();
  clear_to_color (screen,119);
  hline (screen, 9, 9, 311, 115 );
  hline (screen, 9, 140, 311, 115 );
  vline (screen, 8, 9, 140, 123 );
  vline (screen, 311, 9, 140, 123 );


  frames = fps = 0;
//***( b†d¦ got¢w... )*******************************************************

#ifdef KEYSTD
  int k;
  do
  {
    k = getch () & 0xff;

    switch ( k )
    {
      case 'w': player1->move (4096); break;
      case 'q': player1->rotate (12); break;
      case 'e': player1->rotate (-12); break;
    }

    player1->render ();
    player1->toscreen ();
    fon->printf (2,2,"~246POZYCJA %d %d KT %d", player1->getx(),player1->gety(),player1->geta() );

  } while ( k!=27 );
#endif
#ifdef KEYTIMER
  do
  {
//***( narysowanie widokow playrow )*****************************************
    player1->render ();
    vsync ();
    player1->toscreen ();
//***( inne glupoty na ekranie )*********************************************
    rectfill (screen,0,176,319,199,119);
    fon->printf (2,176,"~251POZYCJA %d %d KT %d", player1->getx(),player1->gety(),player1->geta() );
    fon->printf (2,188,"~250SZYBKO˜•: %d KLATEK/S", fps );
    frames++;
//***( sprawdzanie klawiszy funkcyjnych )************************************
    // F1: swiat’o
    if ( key[KEY_F1] )
    {
      while ( key[KEY_F1] );
      light_flag ^= 1;
      DC_LightSetup (light_table, light_flag);
    }
    if ( key[KEY_F10] )
    {
      res->get("kuku_tralalala");  // wywolaj blad
      Error ("Nie powinienes naciskac F10, skoro nie wiesz do czego sluzy.");
    }
    if ( key[KEY_F11] )
    {
      Error ("Do not press F11 key again!");
    }
    // F12: zrzutka ekranu do Pcx'a
    if ( key[KEY_F12] )
    {
      while ( key[KEY_F12] );
      save_pcx ("obraz.pcx", screen, (RGB*)res->get("palette") );
    }
  } while ( !done_flag );
#endif
  delete fon;

}
//---------------------------------------------------------------------------
void Game::renderer_init ()
{
  puts ("renderer_init: setting up lightsourcing");
  // setup lighsourcing
  light_table = res->get ("colormap");
  DC_LightSetup (light_table, light_flag);
}
//---------------------------------------------------------------------------
void Game::startup ()
{
  __dpmi_free_mem_info minfo;
	textattr(7);
	clrscr();
	textattr (0x4F);
	clreol();
  printf (" Tajemnica sryriusza 1.5á startup \n");
	textattr (7);
  LOS_Banner ();
	__dpmi_get_free_memory_information (&minfo);
	printf ("Available DPMI memory: %d kB\n",
						minfo.total_number_of_physical_pages*4);
  res = new PAKfile (main_resource_file, Error);
  if (res->status == PAK_ERROR)
  {
//    Error ("Cannot open resource file");
  }
  puts ("Init raycasting engine: keyboard, timer");
  generateTrigTables ();
  allegro_init ();
#ifdef KEYTIMER
  install_keyboard ();
  install_timer ();
  LOCK_FUNCTION (poll_keyboard);
  install_int_ex ( poll_keyboard, BPS_TO_TIMER(50) );   // 50 Hz
  LOCK_FUNCTION (fps_counter);
  LOCK_VARIABLE (frames);
  LOCK_VARIABLE (fps);
  install_int_ex ( fps_counter, BPS_TO_TIMER(1) );   // 1 Hz
#endif
  //****( obiekty typu player )*********************************************
  puts ("Init players");
  player1 = new PlayerObj (this);
  //...
  player1->init_view (10,10,300,130);
  //...
  //****( tekstury, lighting etc )******************************************
  renderer_init ();
  printf ("Loading textures\n");
  wall_tex.load (res,"walls.art");
  //****( tryb graficzny )**************************************************
  puts ("Switching to graphics\n");
  set_gfx_mode (GFX_VGA, 320, 200, 0, 0 );
  set_palette ( (RGB*)res->get("palette") );
#ifdef DEBUG
  debugfp = fopen ("debug.-","wt");
#endif
}
//-------------------------------------------------------------------------
// funkcje stosowane tylko raz lub rzadko...
void Game::logo ()
{
  set_palette ( black_pallete );
  draw_rle_sprite (screen, (RLE_SPRITE*)res->get("tslogo.rle"), 0, 0);
  fade_in ( (RGB*)res->get("palette"), 1 );
  fon->printf (16,16,"~014HERETYK PRZEDSTAWIA SW£J PROJEKCIK");
  fon->printf (16,50,"~010KLAWISZE DOSTPNE W GRZE:");
  fon->printf (40, 80,"~011STRZAœKI ~015RUCH GRACZEM");
  fon->printf (40, 90,"~011      F1 ~015PRZEœCZNIK ˜WIATœA");
  fon->printf (40,100,"~011     F12 ~015ZRZUTKA EKRANU DO PLIKU PCX");
  fon->printf (20,160,"~015TERAZ MO¡ESZ NACISN• DOWOLNY KLAWISZ");

  readkey ();
  res->kill ("tslogo.rle");
}
//-------------------------------------------------------------------------
int Game::loadmap (char* mapname)
{
  byte *pbMapTemp = (byte*)res->get(mapname);
  memcpy (tilemap,pbMapTemp,4096);
  memcpy (objectmap,pbMapTemp+4096,4096);
  //*** obejdz wszystkie komorki
  player1->position (0,0,0);
  for ( int x=0; x<64; x++ )
    for ( int y=0; y<64; y++ )
    {
      //*** Sprawd¦ czy nie jest to punkt startowy ***
      switch ( (int)objectmap[63-y][x] )
      {
        case OB_STARTN: player1->position ( (x*TILESIZE)+TILECENTER, (y*TILESIZE)+TILECENTER, NORTH );
                        break;
        case OB_STARTE: player1->position ( (x*TILESIZE)+TILECENTER, (y*TILESIZE)+TILECENTER, EAST );
                        break;
        case OB_STARTS: player1->position ( (x*TILESIZE)+TILECENTER, (y*TILESIZE)+TILECENTER, SOUTH );
                        break;
        case OB_STARTW: player1->position ( (x*TILESIZE)+TILECENTER, (y*TILESIZE)+TILECENTER, WEST );
                        break;
      }
      //*** Tu mo§na poszuka jakichž drzwi ***
    }
  if ( player1->getx()==0 || player1->gety()==0 )
  {
    return FAILED;
  }
  return OK;
}
//---------------------------------------------------------------------------
Game::~Game ()
{
  delete res;
  delete player1;
#ifdef DEBUG
  fclose (debugfp);
#endif
}
//---------------------------------------------------------------------------

void main ()
{
  LOCK_VARIABLE(gra);
  gra.run ();
}
