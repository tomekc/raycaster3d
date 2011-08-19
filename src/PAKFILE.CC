/*===========================================================================
	PAKfile

	Klasa síußy do íatwego dostëpu do danych zapakowanych do jedenego pliku PAK
	programem PAKER. Gdy chcemy zaíadowaç jakieû dane, moßemy zaßÜdaç wska¶nika
	(void*) lub (byte*), a w razie potrzeby obiekt zostanie zaíadowany - nie
	musimy tworzyç wíasnego wska¶nika ani martwiç sië o zwolnienie pamiëci.
	Gdy obiekt jest niepotrzebny, moßemy go wyrzuciç z pamiëci rëcznie!

  31.10.1996 - moßna zniszczyç lumpa znajÜc wska¶nik

===========================================================================*/
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <lzo1x.h>
#include "pakfile.h"

PAKfile::PAKfile (const char *name=NULL, void (*errfn)(char*)=NULL )
{
  if ( name==NULL ) return;
  errorfunc = errfn;
  if ( lzo_init() != LZO_E_OK )
  {
    error ("ResManager: error during decompressor initialization.");
    return;
  }
  fp = fopen(name,"rb");
  if ( fp==NULL )
  {
    error ("ResManager: could not open PAK file.");
    return;
  }
  if ( fread (&hdr, sizeof(hdr), 1, fp) == -1 )
  {
    error ("ResManager: file read error.");
    return;
  }
  if ( hdr.id != PACK )
  {
    error ("ResManager: broken PAK file.");
    return;
  }
  numfiles = hdr.dir_size/sizeof(PAKentry);
  catalog = new PAKentry[ numfiles ];
  fseek (fp, hdr.dir_offset ,SEEK_SET);
  if ( fread (catalog, sizeof(PAKentry), numfiles, fp) == -1 )
  {
    error ("ResManager: file read error.");
    return;
  }
  //*** zr¢b listë wska¶nik¢w i ustaw wszystkie na NULL
  ptrlist = new void*[numfiles];
  if ( ptrlist==NULL )
  {
    error ("ResManager: not enough memory.");
    return;
  }
  for (int c=0; c<numfiles; c++)
    ptrlist[c] = NULL;
  //*** jak to kwaûniewski raz powiedziaí: gut gemacht
  status = PAK_OK;
}

PAKfile::~PAKfile ()
{
  int c;
  fclose (fp);
  for ( c=0; c<numfiles; c++)				// pozwalniaj lumpy
    if (ptrlist[c]) free (ptrlist[c]);
  delete [] catalog;								// wywal katalog
  delete [] ptrlist;								// wywal listwë wska¶nik¢w
}

int PAKfile::find (const char *file)
{
  int c;
  for ( c=0; c<numfiles; c++ )
  {
    if ( stricmp(file,catalog[c].name) == 0 )
    {
      return c;
    }
  }
  error ("ResManager: invalid lumpname [find].");
  return -1;
}
//---------------------------------------------------------------------------
// odnalezienie lumpa na podstawie wska¶nika jest szybsze i wygodniejsze
// niß korzystanie z nazwy
int PAKfile::findptr (void *ptr)
{
  int c;
  for ( c=0; c<numfiles; c++ )
  {
    if ( ptrlist[c] == ptr )
    {
      return c;
    }
  }
  error ("ResManager: invalid lumpname [findptr].");
  return -1;
}

int PAKfile::seekto (const char *file)
{
  int n = find(file);
  fseek (fp, catalog[n].offset, SEEK_SET);
  return n;
}

void PAKfile::load (const char *lump, void *ptr)
{
  int nr = seekto(lump);
  unsigned int out_len;
  int packlen = catalog[nr].packlength;
  int rawlen = catalog[nr].length;
  if ( packlen == 0 )
  {
    status = fread (ptr, rawlen, 1, fp);
    if ( status < 1 ) error ("ResManager: read error [load].");
  }
  else  // w takim razie potrzebna bëdzie dekompresja
  {
    // zabierz na chwilkë trochë pamiëci
    void *pTemp = malloc (packlen);
    if ( pTemp == NULL )
    {
      error ("ResManager: not enough memory [load_packed].");
    }
    status = fread (pTemp, packlen, 1, fp);
    if ( status < 1 ) error ("ResManager: read error [load_packed].");
    // dekompresuj
    status = lzo1x_decompress ((lzo_byte*)pTemp, (lzo_uint)packlen, (lzo_byte*)ptr, &out_len, NULL);
    if ( status != LZO_E_OK )
    {
      char acTemp[80];
      sprintf (acTemp, "ResManager: decompression failed (%d).", status );
      error (acTemp);
    }
  }
}

int PAKfile::length (const char *file)
{
  int n;
  n = find(file);
  return catalog[n].length;
}
//===========================================================================
/*
**  zwraca wska¶nik na obiekt o podanej nazwie, kt¢ry jest íadowany z dysku
**  w razie nieobecnnoûci w pamiëci
*/
void* PAKfile::get (const char *lumpname)
{
  int size,nr;
  void *ptr;
  // we¶ pamiëç jeûli jest plik
  size = length(lumpname);
  if (size<0) return NULL;
  nr = find(lumpname);
  if ( nr<0 )   // nie ma takiego numeru
  {
    return NULL;
  }
  // jeûli juß zaíadowany to wracaj
  if (ptrlist[nr] != NULL) return ptrlist[nr];
  // a gdy nie, to we¶ pamiëç...
  ptr = malloc (size);
  if (ptr == NULL)
  {
    error ("ResManager: not enough memory for lump");
    return NULL;
  }
  // jest pamiëç - íadujemy!
  load (lumpname, ptr);
  ptrlist[nr] = ptr;
  return ptr;
}

FILE* PAKfile::getfp (const char *lumpname)
{
  if ( seekto (lumpname)<0) return NULL;
  return fp;
}

inline void* PAKfile::operator() (const char *lumpname)
{
  return get(lumpname);
}
//---------------------------------------------------------------------------
// niepotrzebny obiekt moßna wyrzuciç oszczëdzajÜc pamiëç

void PAKfile::kill (const char *lump)
{
	int nr = find(lump);
	if (nr>=0) delete ptrlist[nr];
}

void PAKfile::kill (void *ptr)
{
  int nr = findptr(ptr);
	if (nr>=0) delete ptrlist[nr];
}

void PAKfile::error (char *str)
{
  status = PAK_ERROR;
  if ( errorfunc )
  {
    errorfunc(str);
  }
}

