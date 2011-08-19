#include <string.h>
#include "defs.h"
#include "game.h"
#include "pakfile.h"

/*---------------------------------------------------------------------------
 Klasa: textures

 klasa s’u§y do przechowania tekstur (tego si‘ mo§na domyžle).
 Przeci†§ony operator [] zwraca wska¦nik na wybran† textur‘!

---------------------------------------------------------------------------*/

Textures::Textures ()
{
  nummaps = 0;
}

void Textures::load (PAKfile* res, char* name)
{
  int size;
  size = res->length (name);
  if ( size<0 ) return;
  nummaps = size/4096;
  mapptr = (byte*)res->get(name);							// zaladuj...
}

Textures::~Textures ()
{
	// zabijanie textur zostawmy CacheManagerowi
}

byte* Textures::operator[] (int n)
{
  if ( n>nummaps ) n=0;
  return mapptr + (TEXMAPSIZE * n);
}


