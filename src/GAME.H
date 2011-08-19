#include <stdio.h>
#include <allegro.h>
#include "pakfile.h"
#include "defs.h"


extern "C" void DC_LightSetup (void *ptable, int lightOn);
extern "C" void DC_ScreenSetup (int wt, int ht, void *screen);
extern "C" void DC_Sliver (int col, int slice, int dist, byte *wall);

class Game;

/*---------------------------------------------------------------------
  TABLES.CC
---------------------------------------------------------------------*/
void generateTrigTables ();
void generateViewTables (int w, int *,int *);
extern int *fisheye_table;     // do kompensacji efektu "rybiego oka"
extern int *pixel_delta;       // r¢ßnica do kÜta promienia biegnÜcego wprost
extern int tangent[ANGLES];    // nachylenie prostej dla kaßdego kÜta
extern int sinus[ANGLES];
extern int cosinus[ANGLES];

/*---------------------------------------------------------------------
  TEXTURES.CC
---------------------------------------------------------------------*/
class Textures
{
  byte *mapptr;
  int nummaps;
public:
  Textures ();
  void load (PAKfile *res, char *name);
  ~Textures ();
  byte* operator[] (int n);
};

/*---------------------------------------------------------------------
  OBJECTS.CC
---------------------------------------------------------------------*/
enum objtype_t { o_actor, o_player, o_static, o_door };

class Object
{
protected:
  int gridx,gridy;          // pozycja w siatce
  Game *parent;
  objtype_t objtype;
};

class ActorObj : public Object
{
  int xpos, ypos;                 // dokíadna pozycja
  int angle;                      // kÜt
public:
  ActorObj (Game *);
  void position (int x, int y, int a);
  void rotate (int a);           // obr¢ç
  void move (int n);
  void strafe (int n);           // p¢jd¶ bokiem
  int checkMove (int x,int y);
  int getx() { return xpos; }
  int gety() { return ypos; }
  int geta() { return angle; }
  int getgridx() { return gridx; }
  int getgridy() { return gridy; }
};
//---------------------------------------------------------------------------
class PlayerObj : public ActorObj
{
  int view_x, view_y, view_w, view_h;   // wspolrzedne obrazu
  BITMAP *double_buffer;
  int *fisheye, *pixdelta;
public:
  PlayerObj (Game *);
  ~PlayerObj ();
  void destroy_view ();
  void init_view (int x,int y,int w,int h);
  void render ();
  void toscreen ();
  int get_w ()
  {
    return view_w;
  }
};
//---------------------------------------------------------------------------
enum doorstate_t { d_open, d_closed, d_locked, d_opening, d_closing };
enum doortype_t { d_horiz, d_vert };

class DoorObj : public Object
{
  doorstate_t state;
  doortype_t type;
  int pos;            // stan: 0 = otwarte, 63 = zamkniete
public:
  DoorObj (Game*, int,int,doortype_t );
  void open ();
  void close ();
  void act ();        // powoduje robienie czegoû z drzwiami
  int getpos ()
  {
    return pos;
  }

};

/*------------------------------------------------------------------------
  GAME.CC
--------------------------------------------------------------------------*/
void Error (char *str);

class Game {
  PAKfile *res;
  PlayerObj *player1, *player2;
  byte tilemap[64][64];
  byte objectmap[64][64];
  // view properties
  //int view_maxw, view_maxh;
  //int view_w, view_h, view_x, view_y;
  void *light_table;
  int light_flag;
  int done_flag;
  //BITMAP *viewbuf;
  // texture maps
  Textures wall_tex;
  Textures object_tex;
  // inne
  friend void poll_keyboard(...);
  void logo ();
public:
  Game ();
	~Game ();
  void startup ();              // perform startup operations
  void draw (PlayerObj *);                 // draw player's view
  void run ();                  // main gameloop !!!
  void renderer_init ();        // init view
  int loadmap (char* name);     // load and interpret a map
  byte tileat (int x,int y)
  {
    return tilemap[63-(y&63)][x&63];
  }
  byte objectat (int x, int y)
  {
    return objectmap[63-(y&63)][x&63];
  }
};



