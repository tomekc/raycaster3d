#include <stdio.h>

#ifndef PAKFILE_H
#define PAKFILE_H

typedef unsigned int dword;
typedef unsigned char byte;

#define PACK 0x4B434150
#define PAK_OK    0
#define PAK_ERROR -1

struct PAKheader {
        dword id;
        dword dir_offset;
        dword dir_size;
};

struct PAKentry {
        char name[52];
        dword packlength;
        dword offset;
        dword length;
};


class PAKfile
{
  FILE *fp;
  PAKentry *catalog;
  PAKheader hdr;
  int numfiles;
  void **ptrlist;					// listwa wska¦nik¢w
  void (*errorfunc)(char*);
public:
  int status;
  PAKfile (const char *filename, void (*ef)(char*) );
  ~PAKfile ();
  int find(const char *filename);
  int findptr(void*);
  int seekto(const char *filename);
  void load (const char *filename, void *where);
  int length(const char *filename);
  void* get(const char *lump);
  FILE* getfp(const char *lump);
  void* operator() (const char *lump);
  void kill (const char *lump);
  void kill (void *);
  void error (char *str);
};

#endif

