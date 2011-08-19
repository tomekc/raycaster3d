/*
 *
 *
 */
#include <allegro.h>

class BitFont
{
  char *header;
  char *body;
  int fnt_height;
  int maxwidth;
  int ofstab[256];
  int fgcolor;
  int bgcolor;
  BITMAP *out_bmp;

  void newcolor (const char *);
  void draw_char (BITMAP *bmp, int x, int y, int ch);
public:
  BitFont (void *dat);
  int width (const char *text);
  void print (int x, int y, char *text);
  void printf (int x,int y,char *format,...);
  // funkcje innline
  void output (BITMAP *bm)
  {
    out_bmp = bm;
  }
  void color (int fg, int bg=0)
  {
    fgcolor = fg;
    bgcolor = bg;
  }
};
