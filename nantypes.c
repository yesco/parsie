// NAN boxing and unboxing of data
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

// only for *this* endian...
typedef union { uint64_t u; double d; } data;

// 1 sign, 7 types, 48(47?) bits of data
//#define MTYP 0x0007000000000000L
//#define MNAN 0x7ff8000000000000L
//#define MDAT 0x0000ffffffffffffL
//#define MNEG 0x8000000000000000L
const uint64_t MTYP=0x0007L<<48,MNAN=0x7ff8L<<48,MDAT=(1L<<48)-1L,MNEG=1L<<63;
// TODO: MDAT should be 1L<<49?

#define TYP(x) ((int)((x).u>>48) & 7)
#define NEG(x) (x.u & MNEG)
#define BOX(n,t,dat) ((data){.u=MNAN|((n)?MNEG:0)|(((t)&7L)<<48)|((long)(dat))&MDAT})
#define DAT(x) ((x).u & MDAT)

#define HPSIZE 16*1024
char hp[HPSIZE]= {0}; int nilo; data nil, undef;

// Add a String to HP limited by SIZE
//
// Searches String in HeaP of SIZE
// (size==-1 doesn't check overflow)
// Adding string if needed.
//
// Returns offset to LEN of string
// followed by a cstring at offset 1.
//
//   HP=[len=3, "foo", 8b data]...
//   len=0 means end of list
//   note: assumes zeroed HeaP
int nameadd(char* s) { char* p= hp; int l= strlen(s);
  while(*p){if(!strcmp(s,p+1))return p-hp;p+=1+strlen(p+1)+1+sizeof(data);}
  assert(p+l+2-hp < HPSIZE);
  return *p=l, strcpy(p+1, s)-hp-1;
}

void prnames() { char* p= hp; printf("\n"); while(*p) { data* d= (data*)(p+1+strlen(p+1)+1); printf("%5ld: %d v=> %10.7g %5ld\t%s\n", p-hp, *p, d->d, d->u, p+1); p+=strlen(p+1)+1+1+sizeof(data); } } // DEBUG

// Return a data atom
data atom(char* s){return BOX(0,2,nameadd(s));}

void inittypes() {nil=atom("nil");assert(DAT(nil)==nilo);undef=atom("undef");}

char* nanstr(data d) { return TYP(d)==2? DAT(d)+hp+1 : 0; }

// Prints a nanData valuie using HeaP
//
// Returns 0-n chars printed
//   -1: EOF on error
//   -2..-9 if not handled: =TYP(d)-2
int nanprint(data d) {int t=TYP(d);
  return isnan(d.d)?(t==2?printf("%s",nanstr(d)):-t-2):printf("%.7g ",d.d);}

// TODO: it's not aligned
// AtomValPTR can be used as global
#define AVPTR(d) (TYP(d)!=2?NULL:(data*)&hp[DAT(d)+hp[DAT(d)]+2])

// how many strings can we handle?
// TODO: make dynamic?
#define ZZ 1024

// pos ss[0] is free list
typedef char* dstr; dstr ss[ZZ]={0}; char sr[ZZ]={0}; int sn=1;

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
char* sncat(dstr s, char* x, int n) { int i= s?strlen(s):0, l= x?strlen(x):0;
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

void gc() { memset(sr, 0, sizeof(sr));
  // --- MARK: sr[x]++ for each ref
  // stack
  // TODO: what if stack was upper part of heap and grew down?
  for(int i=0; i<SMAX; i++) { data d= {.d=S[i]}; if (!isnan(d.d)||!NEG(d)) continue;
    if (TYP(d)==3) sr[DAT(d)]++; }
  // memory (and globals)
  char* p= M; while(p<H) { data d= *(data*)p; if (!isnan(d.d)||!NEG(d)) continue;
    if (TYP(d)==3) sr[DAT(d)]++; }

  // --- SWEEP - prefer lower first
  for(int i=sn;i;i--)if(!sr[i]){free(ss[i]);ss[i]=ss[0];ss[0]=(char*)(long)i;}
  DEBUG({long f= 0; printf("Free: "); do printf("%lu=>", f); while((f=(long)ss[f]));printf("\n");});
  
}

// ENDWCOUNT

// NEG:  used to indicate need GC
//
// TYPES:
//   0: NaN         (could be used)
//   1: -
//   2: atom        offset atom heap
//   3: dstr        managed strings
//
// Possible candidates:
//   - short str   char[6] (==1)
//   - void*       offset data heap
//   6: closure     ix closure array
//   7: function    ix func array
//

/*
typedef union data {
  uint64_t u;
  double d;
  struct str { char ch[6]; char t: 3; } str;
} data;

// Encodes max 6 chars as a nan data
// More than 6 chars are truncated
data char6nan(char* s) { data r= BOX(0, 1, 0);
  strncpy((char*)&r.str.ch, s, sizeof(r.str.ch));
  return r;
}

// Return a static string copy of
// string encoded as NaN.
//   Returns NULL if not string (T!=0)
char* nanchar6(data d) { static char s[7]= {0}; if (TYP(d)!=1) return NULL;
  return memcpy((char*)s, d.str.ch, sizeof(d.str.ch));
}

*/

#ifdef nantypesTEST
void P(char* desc, data d) {
  //printf("TYPE=%d\n", d.str.t);
  data *v= AVPTR(d);
  if (v) v->d++;
  //printf("DATA=%s\n", nanstr(v, hp));
  //memcpy(&l, aptr(d, hp), sizeof(l));
  printf("T=%d %5f 0x%lx '%s' %s data=%f\n", TYP(d), d.d, d.u, nanstr(d), desc, v?v->d:0); //*aptr(d, hp));
}

int main() {
  inittypes();

  //assert(mtyp==MTYP);assert(mnan==MNAN);assert(mdat==MDAT);assert(mneg==MNEG);

  data n= {.u=MNAN};
  data f= BOX(1, 7, MDAT);
  //data c= char6nan("gurkafoobar");
  data s= BOX(1, 7, 4711);

  P("nil", nil);
  P("nil", nil);
  P("undef", undef);
  P("n  ", n);
  P("f  ", f);
  //P("c  ", c, NULL);
  P("s  ", s);
  data shortatom= atom("foobar");
  P("shortatom", shortatom);
  data longatom= atom("foobarfiefum");
  P("longatom", longatom);
}
#endif
