//80----------------------------------------------------------------------------
// NAN-boxing and unboxing of data
//
// We're using the NAN values to encode
// data, type info + offsets into storage.
//
// Data values knows their types:
// - douible
// - TATM - atoms (symbols/globals)
// - TSTR - managed immutable strings
// - TOBJ - JS prototypical objects
// - TFUN ? - TODO: closuresfunctions

// === ATOMS:
// Atoms have constant time:
// - printing name
// - getting global var offset
// 
// An atom encodes [TATM, n, hp-offset]
//                  12b  20b   32b
//
// An atom: [TATM, typ, n, hp-offset]
//           12b   10b 16b   26b
//            (- 64 12 10 16)
//            (** 2 26) = 64 MB
//
// - typ number
//   2^9=512 types, 1 bit==ptr
// - n is the linear number of atom
//   (nil=0, undef-1 ...)
//     16b = 64K max
// - the name is stored in hp heap
//     26b = 64 MB addressing

// === STRINGS
// (see str.c)

// === OBJECTS:
// (see obj.c)

// === FUNCTIONS: (clsoures)
// (see ...)

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

// alias to clarify extended meaning
typedef double D;
unsigned long d2u(D d) { return *(unsigned long*)(&d); } D u2d(long u) { return *(D*)(&u); }

// 1 sign, 7 types, 48(47?) bits of data
// Use Typ:2B = sIII IIII IIII Ittt
//#define MTYP 0x000070000000000L
//#define MNAN 0x7fff80000000000L
//#define MDAT 0x0000fffffffffffL
//#define MNEG 0x800000000000000L
//const uint64_t MNAN=0x7ff8L<<48,MDAT=(1L<<48)-1L; //,MNEG=1L<<63;
// TODO: MDAT should be 1L<<49?

#define BOX(t,dat) ((((unsigned long)(dat))&((1LU<<48)-1)) | (((unsigned long)t)<<48))
#define DAT(x) (d2u(x)&(((1LU<<48))-1))
#define TYP(x) ((unsigned int)(d2u(x)>>48))

// Typ:s constants (neg if need to be GC:ed)
//  0 reserved 0xfff8 - plain nan (can be used for more...)
//  1 inline6  0x7ff9 - inline 6c x 8=48 chars, 8c x 6=A-Za-z0-9_\0
//  2 = TATM   0x7ffa - atom
// -3 = TSTR   0xfffb - managed GC strings
//(-4 = TOBJ   9xfffc - TODO: JS-style objects)
//(-5 = TENV   0xfffd - TODO: same same?      )
//(-6 = TCONS  0xfffe - TODO:                 ) 
//(-7 = TCLOS  0xffff - TODO:                 }
const long TNAN=0x7ff8,TATM=0x7ffa,TSTR=0xfffb,TOBJ=0xfffc,TCONS=0xfffe,TENV=0xfffd;
//,TCONS=0xfffe,TCLOS=0xffff;

// TODO: isnan isn't working, and atom > number!!!
int ISNAN(double d) {unsigned long x=d2u(d); return (TYP(d)&TNAN)==TNAN; }

// TODO? unify with svar + K + M ???

#define HPSIZE 16*1024
char hp[HPSIZE]= {0}; int nilo=0; D nil, undef, error, proto;

// TODO: global storage idea
// - treat:SMAX..VMAX

// Add a String to HP limited by SIZE
//
// Searches String in HeaP of SIZE
// (size==-1 doesn't check overflow)
// Adding string if needed.
//
// Returns offset to LEN of string
// followed by a cstring at offset 1.
// (-offset returned if created)
//
// Extra + (n<<32) atom number
//
//   HP=[len=3, "foo", 8b data]...
//   len=0 means end of list
//   note: assumes zeroed HeaP
long nameadd(char* s) {char*p=hp;int l=strlen(s),n=0;while(*p){if(!strcmp(s,p+1))
  return p-hp+((L n)<<32); p+=1+strlen(p+1)+1+sizeof(D);n++;}
  assert(p+l+2-hp < HPSIZE);
  // create new (neg)
  *p=l; return -(strcpy(p+1,s)-hp-1+((L n)<<32));
}

void prnames() { char* p= hp; printf("\n"); while(*p) { D* d= (D*)(p+1+strlen(p+1)+1); printf("%5ld: %d v=> %10.7g %5ld\t%s\n", p-hp, *p, *d, d2u(*d), p+1); p+=strlen(p+1)+1+1+sizeof(D); } } // DEBUG

// Return a data atom from String
// empty string returns nil
D atom(char* s){if(!s||!*s)return nil;
  long n=nameadd(s); D a=u2d(BOX(TATM,n<0?-n:n));
  if (n<=0) { n=0-n; *C++= a; *C++= (n>>32<3)?a:undef; } return a; }

long atomaddr(D a) { return 1+2*(DAT(a)>>32); }
// return 1+2*(d2u(a)>32)&0x1ffff; }

// TODO: "ERROR"
void inittypes() {nil=atom("nil");undef=atom("undef");K[SMAX-1]=*S=*K=error
  =atom("*ERROR*");proto=atom("__proto__");
  assert(DAT(nil)==nilo); }

int deq(D a, D b) { return d2u(a)==d2u(b); }

char typ(D d) { int t= TYP(d); return !isnan(d)?(d==L d?'i':'f'):deq(t,nil)?
  'N':deq(t,undef)?'U':deq(d,error)?'E':t==TATM?'A':t==TSTR?'S':t==TOBJ?'O':
  t==TCONS?'C':t==TNAN?'n':t==TENV?'E':0; }


// TODO: it's not aligned
// AtomValPTR is used as address to global var storage in M
// TODO: remove: use svar.c and hibit encoded offsets!
#define AVPTR(d) (TYP(d)!=2?NULL:(D*)&hp[DAT(d)+hp[DAT(d)]+2])

// Get M pointer from offset TYP
// TODO: use m() ???
void* PTR(int t, D o) { return TYP(o)==t?M+DAT(o):0;}

// how many strings can we handle?
// TODO: make dynamic?
#define ZZ 1024


typedef char* dstr; dstr ss[ZZ]={0}; char sr[ZZ]={0}; int sn=1;

// Dynamic String dstr append/cat string X N chars
//
// dstr: String pointer is resized in chunks
// of 1024 bytes. Free as normal
//
// S= NULL, or malloced destination
// X= NULL, or string to append from
// N= chars to copy, or -1=all
//
// Returns S or a new pointer.
//  
// TODO: not safe if x not ZeroTerminated...
dstr sncat(dstr s, char* x, int n) {int i=s?strlen(s):0,l=(x||n<0)?strlen(x):0;
  if (n<0 || n>l) n= l; s= realloc(s, 1024*((i+n+1024)/1024)); s[i+n]= 0;
  return strncpy(s+i, x?x:"", n), s;
}

// Return a new str from char* S take N chars.
D newstr(char* s,int n){ ss[sn]=sncat(0,s?s:"",n); return u2d(BOX(TSTR,sn++)); }

char* dchars(D d) { int t=TYP(d), x=DAT(d);
  return t==TATM? (x&0xffffffff)+hp+1: t==TSTR? (char*)ss[x]: 0;}

// TODO: length of $" char* ?
int dlen(D f) { char* r= dchars(f); return r?strlen(r):TYP(f)==TOBJ?((D*)PTR(TOBJ,f))[3]:0;}

// compare anything
// - if both numbers        => <=>
// - if both strings/atoms  => strcmp
// - num,nan < obj          => -1 (+1) 
// - nan and number
int dcmp(D a, D b) { unsigned long c,d,e=1; switch (!!isnan(a)*10+!!isnan(b)) {
 case 00:return(a>b)-(a<b);case 01:e=-e;case 10:return DAT(e>0?a:b)?+256:-256;}
  c=d2u(a),d=d2u(b);if (c==d) return 0;  char *g=dchars(a), *h=dchars(b);
  if (g&&h) return strcmp(g, h); return (d<c)-(c<d); }

int pobj(D); // FORWARD

int dprint(D f){char*s=dchars(f);int l=pobj(f);return l?l:s?printf("%s",s):printf("%.10g",f);}

// Concatenate D + S as new str
// from Index in S take N chars.
// TODO: optimize?
D strnconcat(D d, D s, int i, int n) { char* x= dchars(s);
  ss[sn]= sncat(sncat(0,dchars(d),-1), x?x+i:0, n); return BOX(TSTR,sn++); }

// GC for managed strings
// TODO: TCONS
void gc() { memset(sr, 0, sizeof(sr));
  // --- MARK: sr[x]++ for each ref
  // stack
  // TODO: what if stack was upper part of heap and grew down?
  for(int i=0; i<SMAX; i++) { D d= K[i]; if (TYP(d)==TSTR) sr[DAT(d)]++; }
  // memory (and globals)
  char* p= M; while(p<H) { D d= *(D*)p; if (TYP(d)==TSTR) sr[DAT(d)]++; }

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

  data n= NAN;
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
