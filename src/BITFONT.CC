/*
 *  BITFON - element systemu graficznego Heretix
 *
 *
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include "bitfont.h"

//==========================================================================
BitFont::BitFont (void *dat)
{
  int j,k;
  header = (char*)dat;
  body = &header[256];
  fnt_height = (int)header[0];
  maxwidth = 0;
  // odczytaj szerokosci znakow
  k = 0;
  for ( j=1; j<256; j++ )
  {
    ofstab[j] = k * fnt_height;
    k += (int)header[j];
    if ( (int)header[j]>maxwidth ) maxwidth=(int)header[j];
  }
  header[32] = header[65];    // szerokosc spacji
  fgcolor = 1;
  bgcolor = 0;
  out_bmp = screen;
}
//==========================================================================
// zwraca szerokosc lancucha (wliczajac odstepy)
int BitFont::width (const char *text)
{
  int l=0,ic;
  const unsigned char *ch;
  ch = (unsigned char*)text;
  do {
    ic = (unsigned int)*ch;
    if ( ic=='~' )
    {
      ch+=3;
      continue;
    }
    l += (int)header[ic];
    l += 1;
  } while ( *ch++ );
  return l;
}
//==========================================================================
// zmiana koloru wymuszona komenda wewnatrz lancucha
void BitFont::newcolor (const char *str)
{
  char atemp[4] = {0,0,0,0};
  atemp[0] = str[0];
  atemp[1] = str[1];
  atemp[2] = str[2];
  color (atoi(atemp));
}
//==========================================================================
// rysuje znak, zapisany kolumnami
void BitFont::draw_char (BITMAP *bmp, int x, int y, int ch)
{
  int w,ofs,row,col;
  char *glyph,pix;
  w = (int)header[ch];
  if ( w==0 ) return;
  glyph = (body + ofstab[ch]);
  for ( col=0; col<w; col++ )
  {
    for ( row=0; row<fnt_height; row++ )
    {
      if ( *glyph ) putpixel(bmp, x+col, y+row, fgcolor);
      glyph++;
    }
  }
}
//==========================================================================
void BitFont::print (int x, int y, char *text)
{
  BITMAP *bmp;
  int wide;
  int ic,sx,sy;
  int chw;
  unsigned char *ch;
  wide = width(text);
  bmp = create_bitmap (wide,fnt_height);
  clear_to_color (bmp, bgcolor);
  // stworzenie glifu
  sx = 0;
  ch = (unsigned char *)text;
  do
  {
    ic = (unsigned int)*ch;
    chw = (int)header[ic] + 1;
    if ( ic=='~' )
    {
      ch++;
      newcolor ((char*)ch);
      ch+=3;
      continue;
    }
    if ( ic!=32 ) draw_char (bmp,sx,0,ic);
    sx += chw;
    ch++;
  } while ( *ch );
  // narysuj gotowy napis
  draw_sprite (out_bmp, bmp, x, y );
  // zwolnij zasoby
  destroy_bitmap (bmp);
}
//==========================================================================
//  Ciekawsze funkcje wypisuj†ce teksty.
//
void BitFont::printf (int x,int y,char *format,...)
{
  char str[128];
  va_list arg;

  va_start (arg,format);
  vsprintf (str,format,arg);
  va_end (arg);

  print (x,y,str);
}


