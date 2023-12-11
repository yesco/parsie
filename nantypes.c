// NAN boxing and unboxing of data
#include <stdio.h>
#include <math.h>
#include <assert.h>

// only for *this* endian...
typedef union { uint64_t u; double d; } data;

// 1 sign, 7 types, 48 bits of data
//#define MTYP 0x0007000000000000L
//#define MNAN 0x7ff8000000000000L
//#define MDAT 0x0000ffffffffffffL
//#define MNEG 0x8000000000000000L
const uint64_t MTYP=0x0007L<<48,MNAN=0x7ff8L<<48,MDAT=(1L<<49)-1,MNEG=1L<<63;

#define TYP(x) ((int)((x).u>>48) & 7)
#define NEG(x) (x.u & MNEG)
#define BOX(n,t,dat) ((data){.u=MNAN|((n)?MNEG:0)|(((t)&7L)<<48)|(dat)&MDAT})
#define DAT(x) (x.u & MDAT)

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

//long* APTR(data d, char* hp) {
//  int ix= DAT(d), l= hp[ix];
//  return (long*)&hp[ix+l+1];
//}

// TODO: it's not aligned
// AtomValPTR can be used as global
#define AVPTR(d) (TYP(d)!=2?NULL:(data*)&hp[DAT(d)+hp[DAT(d)]+2])

//data* atomval(data d, char* hp) { if (TYP(d)!=2) return NULL;
//  int i=DAT(d),l=hp[i]; return (data*)&hp[i+l+1];}

// ENDWCOUNT

// TODO: extras to save space
// but it's atoms, so no need...

// TYPES:
//   0: NaN         (could be used)
//   1: long atom   offset atom heap
//   2: short atom  char[6]
// not ipmlemented (yet):
//   3: short str   char[6] (==1)
//   4: long str    offset str heap
//   5: void*       offset data heap
//   6: closure     ix closure array
//   7: function    ix func array
//
// Negative: mean need to be GC:ed

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