#include <math.h>
#include "types.h"
#include "defs.h"

//const float PI = 3.141592654;

int *fisheye_table;     // do kompensacji efektu "rybiego oka"
int *pixel_delta;       // r¢§nica do k†ta promienia biegn†cego wprost
int tangent[ANGLES];    // nachylenie prostej dla ka§dego k†ta
int sinus[ANGLES];
int cosinus[ANGLES];

inline int rad2deg (float teta)
{
  return (int)( teta*ANG180/PI );
}

/*--------------------------------------------------------------------------
 Stworzenie tablic trygonometrycznych
--------------------------------------------------------------------------*/
void generateTrigTables ()
{
  int c,ref;
  float temp,theta;

  for ( theta=0,ref=0; ref<ANGLES; theta+=((2*PI)/ANGLES),ref++ )
  {
    if ( ref==512 || ref==1536 )
    {
      tangent[ref] = 0;
    }
    else
    {
      temp = tan(theta);
      tangent[ref] = (int)(temp * FIXED1);
    }
    temp = sin(theta);
    sinus[ref] = (int)(temp * FIXED1);
    temp = cos(theta);
    cosinus[ref] = (int)(temp * FIXED1);
  }
}
//--------------------------------------------------------------------------
// generowanie tablic zaleznych od wymiarow ekranu i/lub widoku
//--------------------------------------------------------------------------
void generateViewTables (int wide, int *fish, int *pixdelta)
{
  int c,ref;
  int halfwide = wide/2;
  float temp,theta;

  for (ref=0; ref<wide; ref++ )
  {
    c = ref-halfwide;
    temp = -atan( (float)c/ViewerDist );
    pixdelta[ref] = rad2deg(temp);
    fish[ref] = (int)(cos( atan( (float)c/ViewerDist ) ) * FIXED1);
  }
}






