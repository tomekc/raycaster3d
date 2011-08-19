/*--------------------------------------------------------------------------

  DEFS.H

 definicje sta’ych u§ytecznych w graficznym engine

--------------------------------------------------------------------------*/

#ifndef DEFS_H
#define DEFS_H

typedef unsigned int dword;
typedef unsigned short word;
typedef unsigned char byte;
typedef int boolean;

// k†ty

#define ANGLES 2048L
#define ANGLES 2048L

#define ANG0    0
#define ANG45   (ANGLES/8)
#define ANG90   (ANGLES/4)
#define ANG180  (ANGLES/2)
#define ANG270  (ANGLES*3/4)
#define ANG360  ANGLES

#define VIEWER_DISTANCE   277
#define FIXSHIFT          16
#define FIXED1            0x10000
#define GRIDSIZE          0x10000
#define GRIDSHIFT         16

#define TEXMAPSIZE 4096

#define TILESIZE					0x10000
#define TILECENTER				(0x10000/2)

#define PLAYERSIZE        0x5800

#define WALK_SPEED        6096L
#define RUN_SPEED         WALK_SPEED*2

#define MAXXRES           640

#define NORTH ANG90
#define EAST  ANG0
#define SOUTH ANG270
#define WEST  ANG180

enum { FAILED= -1, OK };
enum { OB_STARTN=200, OB_STARTE, OB_STARTS, OB_STARTW,
       OB2_STARTN, OB2_STARTE, OB2_STARTS, OB2_STARTW    };

const int ViewerDist=VIEWER_DISTANCE;

#endif
