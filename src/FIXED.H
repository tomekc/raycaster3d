/*********************************************************
 Wplatane (inline) funkcje staíoprzecinkowe 16.16:
 - mnoßenie
 - dzielenie
 - odwrotnoûç
 Copyright (C) Tomasz J. Cejner 1996
**********************************************************/

inline int mnoz(int x,int y)
{
  int result;

  __asm__ (
    " imull %2 ;"
// Usuniëcie tych komentarzy wíÜcza zaokrÜglanie
//    " addl $0x8000,%%edx ;"
//    " adcl $0,%%edx ;"
    " shrdl $16, %2, %1 ;"

    : "=a" (result)

    : "a" (x),
      "d" (y)
  );

  return result;
}

inline int dziel(int y,int x)
{
  int result;
  // Dzielenie EDX/EAX
  __asm__ (
    " movl %1,%%ecx ;"
    " xorl %%eax,%%eax ;"
    " shrdl $16, %%edx, %%eax ;"
    " sarl $16, %%edx ;"
    " idivl %%ecx ;"

    : "=a" (result)

    : "a" (x),
      "d" (y)
  );

  return result;
}


inline int odwrot(int x)
{
    int result;
  __asm__ (
    "movl $1,%%edx ;"
    "xorl %%eax,%%eax ;"
    "idivl %1 ;"

    : "=a" (result)

    : "b" (x)
  );
  return result;
}

