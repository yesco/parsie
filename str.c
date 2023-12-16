// str are managed strings

#include <stdio.h>
#include <stdlib.h>

#ifdef strTEST
#include "nantypes.c"
#define DEBUG(a) do a while(0);
#endif

#define dstr char*

// how many strings can we handle?
// TODO: make dynamic?
#define ZZ 1024
dstr ss[ZZ]={0}; char sr[ZZ]={0}; int sn=1; // pos=9 == free list

// Dynamic String append/cat string X N chars
//
// String pointer is resized in chunks
// of 1024 bytes.
//
// S= NULL, or malloced destination
// X= NULL, or string to append from
// N= chars to copy, or -1=all
//
// Returns S or a new pointer.
//  
dstr sncat(dstr s, char* x, int n) { int i= s?strlen(s):0, l= x?strlen(x):0;
  if (n<0 || n>l) n= l; s= realloc(s, 1024*((i+n+1024)/1024)); s[i+n]= 0;
  return strncpy(s+i, x?x:"", n), s;
}

// Return a new str from char* S take N chars.
double newstr(char* s,int n){ss[sn]=sncat(0,s?s:"",n);return BOX(0,3,sn++).d;}

dstr str(data d) { return TYP(d)==3?ss[DAT(d)]:0; }

char* dchars(double f) { data d= {.d=f}; char* e;
  if (!isnan(f)) return M+(long)f;
  e= nanstr(d);
  return e?e:str(d);
}

int dlen(double f) { char* r= dchars(f); return r?strlen(r):0; }

int dprint(double f) {
  // TODO: nan?
  if (!isnan(f)) return printf("%.7g ",f);
  return printf("%s", dchars(f));
}

//printf("[%.8g T=%d D=%lu]", d.d, TYP(d), DAT(d));
	     
// Concatenate D + S as new str
// from Index in S take N chars.
// TODO: optimize?
data strnconcat(data d, data s, int i, int n) { char* x= str(s);
  ss[sn]= sncat(sncat(0,str(d),-1), x?x+i:0, n); return BOX(1,3,sn++); }

void strgc() { memset(sr, 0, sizeof(sr));
  // simulated, lol
  assert(!"TODO: list variables\n");

  #ifdef strTEST // DEBUG
  sr[0]= 1; sr[1]= 1; sr[2]= 0; sr[3]= 0; sr[4]= 1; // DEBUG
  #endif // DEBUG

  // prefer lower first
  for(int i=sn;i;i--)if(!sr[i]){free(ss[i]);ss[i]=ss[0];ss[0]=(char*)(long)i;}
  DEBUG({long f= 0; printf("Free: "); do printf("%lu=>", f); while((f=(long)ss[f]));printf("\n");});
  
}

// ENDWCOUNT

#ifdef strTEST
int main() {
  printf("str\n");
  data a= newstr("foo", -1);
  // %lu??? DAT() returns is wrong...
  printf("%.8g T=%d D=%lu a='%s'\n", a.d, TYP(a), DAT(a), str(a));

  data c= strnconcat(newstr("foo", -1), newstr("bar", -1), 0, -1);
  // %lu??? DAT() returns is wrong...
  printf("%.8g T=%d D=%lu a='%s'\n", c.d, TYP(c), DAT(c), str(c));
  //strgc();
  printf("newstr %s\n", str(newstr("foobarf", 6)));
}
#endif

