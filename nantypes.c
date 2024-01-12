//80----------------------------------------------------------------------------
// NAN-boxing and unboxing of Data
//
// We're using the NAN values to encode
// data, type info + offsets into
// storage.
//
// We call the datatype D here.
// It's just a double, with extra info.
//
// 
// Normal numbers in double:
//  - 0    = encoded as 0x0
//  + 0    = also possible, haha
//  - int  = 54 bit int
//  - num  = floating point double
//  - TNAN = NaN (can be neg)
//  - INF  = Infinity (can be neg)
//
// Data values knows their types:
//  - TATM = atoms (symbols/globals)
//  - TSTR = managed immutable strings
//  - TOBJ = JS prototypical objects
//  - TARR = array & slices?
//  - TUTS = ? TODO: Unix TS
//  - TFUN = ? TODO: closuresfunctions


// TECHNICAL INFORAMTION
// =====================
// - https://en.m.wikipedia.org/wiki/Double-precision_floating-point_format
//
// Doubles are encoded as:
//
//         2 bytes         6 bytes
//   /-------bits------\   /--hex--\
//   smmm mmmm mmmm 1xxx,   XXXXXX
//   s:1    m11     x:3 + X:6*8=48
//   s: sign bit 1=neg
//   m: mantissa 11 bits
//   x: 
// NAN numbers are encoded:
//
//         2 bytes          6 bytes
//   /-------bits------\    /^^^^\
//   s111 1111 1111 1000,   000000
//   s:1            q:1
//
//   s111 1111 1111 0000,   000001 NaN
//   s111 1111 1111 1000,   000001 qNaN
//   s111 1111 1111 1111,   FFFFFF NAN
//
// We're stuffing data in qNaNs:
//
//   s111 1111 1111 1ttt,   DDDDDD
//
//   0x7ff8 = is the highest bits
//   0x7ff9 = is t=1
//   0x7ffa = is t=2
//   0x8ff9 = is t= -1 (?)
//   0x8ffa = is t= -2 (?)
//   0x7ff8000000000000
//     s << 63, sign bit
//        t << 48, (3bits t= 0..7)
//         dddddddddddd == 48 bits
//
// We're using:
//   s= sign bit to indicate to GC
//   t= 0..7 indicating type
//   d= 48 bits data, for offset
//
//

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

// === ATOMS:
// Atoms have special functions:
// - printing name (constant time)
// - getting global var offset (constant time)
// - use to calculate scopes during oompile
//
// 
// BOX DDDDDDDDDDDDDDDD
//     TATM............
//         oo/nnn/hhhhh
//     ..16|10|.16|.22.
//
//     TATM  = 16 bits nan-type
//     oo/   = 10 bits ofs         1K
//     /nnn/ = 16 bits atom seq n 64K
//     hhhhh = 22 bits hp offset  4MB
//     
// - OFS number [-511..512]
//   at first imagine as a type info
//   now used as -frame*100 - ln
//   local scope var "address"
//
// - n is the linear number of atom
//   (nil=0, undef-1 ...)
//     16b = 64K max
//
// - the name is stored in hp heap
//     22b = 4 MB addressing

// === STRINGS
//
// Most scripting languages, as well
// as PASCAL has managed strings. They
// cannot be modified, but still have
// rich libraries of concat/slicing.
//
// These are intially just boxed
// offsets to an array of char*
//
// The strings stored are "dstr" which
// dyanmically resize if neeed.
// They grow or shrink (realloc:ed)
// in chunks. So even appending char by
// char isn't a too bad operation.

// JS str - https://iliazeus.github.io/articles/js-string-optimizations-en/
// ropes - https://en.m.wikipedia.org/wiki/Rope_(data_structure)
// Worth to note is slicing, and concat

// TODO: slicing strings?
// BOX DDDDDDDDDDDDDDDD
//     TSTR............
//         iiiinnnnSSSS
//                 64K strings
//         0000ppppSSSS = pascal 64KB
//         000000000000 = empty
//         00000000SSSS = C str (unl)

// === OBJECTS:
// (see obj.c)

// === TODO: FUNCTIONS: (clsoures)
// (see ...)

// === TODO: TARR - resizeable arrays?
//     TARR............
//         iiiinnnnAAAA
//                 64K arrs

// == TODO: T datetime?
//
// BOX DDDDDDDDDDDDDDDD
//     TUTSAxxxUUUUUUUU = unixTS 32b
//          xxx=1024*16 store ms 10b
//               ms  2b=*y 4yyy ...
//     TISOyyyymmddhhmm = DateTime
//     TUTC............
//         D=y14m4d5 = 23b
//         T=h5m6s6  = 17b 40
//           m10   > 8 - no 'ms'


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

// alias to clarify extended meaning
typedef double D;
UL d2u(D d) { return *(UL*)(&d); } D u2d(UL u) { return *(D*)(&u); }

// 1 sign, 7 types, 48(47?) bits of data
// Use Typ:2B = sIII IIII IIII Ittt
//#define MTYP 0x000070000000000L
//#define MNAN 0x7fff80000000000L
//#define MDAT 0x0000fffffffffffL
//#define MNEG 0x800000000000000L
//const uint64_t MNAN=0x7ff8L<<48,MDAT=(1L<<48)-1L; //,MNEG=1L<<63;
// TODO: MDAT should be 1L<<49?

#define BOX(t,dat) ((((UL)(dat))&((1LU<<48)-1)) | (((UL)t)<<48))
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
const long TNAN=0x7ff8,TATM=0x7ffa,TSTR=0xfffb,TOBJ=0xfffc,TCONS=0xfffe,TFUN=0xfffd;
//,TCONS=0xfffe,TCLOS=0xffff;

// TODO: isnan isn't working, and atom > number!!!
int ISNAN(double d) { return (TYP(d)&TNAN)==TNAN; }

// TODO? unify with svar + K + M ???

#define HPSIZE 16*1024
char hp[HPSIZE]= {0}; int nilo=0; D nil, undef, error, proto, __;

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
 return p-hp+((L n)<<22); p+=1+strlen(p+1)+1+sizeof(D);n++;}
  assert(p+l+2-hp < HPSIZE);
  // create new (neg)
  *p=l; return -(strcpy(p+1,s)-hp-1+((L n)<<22));
}

void prnames() { char* p= hp; printf("\n"); while(*p) { D* d= (D*)(p+1+strlen(p+1)+1); printf("%5ld: %d v=> %10.7g %5ld\t%s\n", p-hp, *p, *d, d2u(*d), p+1); p+=strlen(p+1)+1+1+sizeof(D); } } // DEBUG

// Return a data atom from String
// empty string returns nil
D ofsatom(long ofs, char* s){if(!s||!*s)return nil; ofs+=512; assert(ofs>=0 && ofs<0x3ff);
  long x=nameadd(s); D a=u2d(BOX(TATM,(x<0?-x:x)|(ofs<<(22+16))));
  if (x<=0) { x=-x; *C++= a; long n= (x>>22)&0xffff; *C++= n<3?a:undef; }
  if (ofs<512) { *--Y= undef; *--Y= a; } return a; }

D atom(char* s) { return ofsatom(0, s); }

// Search scoped 8atom:
//   1:st in local scope stack
//   2:nd in global vars storage
// (since they are adj just one linear!) 
D varatom(char* s) { D a= atom(s); char* c= dchars(a);
  for(D* x= Y; Y<C; x+=2) if (dchars(*x)==c) return *x;  return a; }

// global var storage location
long atomaddr(D a) { return 1+2*((DAT(a)>>22)&0xffff); }
long atomofs(D a) { return ((DAT(a)>>(22+16))&0x3ff)-512; }
// return 1+2*(d2u(a)>32)&0x1ffff; }

int deq(D a, D b) { return d2u(a)==d2u(b); }

#define TT(nm, t) int nm(D d) { return TYP(d)==t; }
TT(isatom,TATM);TT(isobj,TOBJ);TT(isstr,TSTR);TT(iscons,TCONS);TT(isfun,TFUN);

// lowercase is numeric
char typ(D d) { int t= TYP(d); return !isnan(d)?(d==L d?'i':'f'):deq(t,nil)?
  'N':deq(t,undef)?'U':deq(d,error)?'E':t==TATM?'A':t==TSTR?'S':t==TOBJ?'O':
  t==TCONS?'C':t==TNAN?'n':t==TFUN?'F':0; }

// TODO: remove?
// TODO: it's not aligned
// AtomValPTR is used as address to global var storage in M
// TODO: remove: use svar.c and hibit encoded offsets!
#define AVPTR(d) (TYP(d)!=2?NULL:(D*)&hp[DAT(d)+hp[DAT(d)]+2])

// Get M pointer from offset TYP
// TODO: use m() ???
void* PTR(int t, D o) { return TYP(o)==t?M+DAT(o):0;}

void inittypes() {
  // Order matters: first 3 == self
  // WARNING: Do NOT change order
  nil=ofsatom(-512,"nil");undef=atom("undef");
  assert(DAT(nil)==nilo);
  K[SMAX-2]=K[SMAX-1]=*S=*K=error =atom("*ERROR*");
  // WARNING: Do NOT add above!

  proto=atom("__proto__");
  __=atom("__");

  // TODO:
  //   - move obj alloc from H to K
  //   - make a special this ptr at "0"
  //   - make obj.set/.get work w D
  //   - (and iterators/printer)?
  //     (any one knows NPN)
  //this=atom("__this__");
}

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
// Note: N string must be zero-terminated
// the length will be read from it.
//
//
// Returns S or a new pointer.
//  
// TODO: not safe if x not ZeroTerminated...
dstr sncat(dstr s, char* x, int n) {int i=s?strlen(s):0,l=(x||n<0)?strlen(x):n;
  if (n<0 || n>l) n= l; s= realloc(s, 1024*((i+n+1024)/1024)); s[i+n]= 0;
  return strncpy(s+i, x?x:"", n), s;
}

// --- printers
// s=sdprinc(s,,D)   - stringify
// s=sdprinq(s,D)    - "foo" if str #atm
// s=sdprintf(s,f,D) - printf(f,D)

////////////////////////////////////////////////////////////////////////////////  
// only %lf %g %s makes sense...
dstr sdprintf(dstr s, char* f, D d) {char*x=dchars(d);
  if(x) {int n= snprintf(0,0,f,x); char q[n+1];
    snprintf(q,sizeof(q),f,x);
    s=sncat(s,q,-1);
    return s;
  }
  int n= snprintf(0,0,f?f:"%.8g",d); char q[n+1];
  snprintf(q,sizeof(q),f?f:"%.8g",d); return sncat(s,q,-1); }

dstr sdprinc(dstr s, D d) {char*x=dchars(d);
  return x?sncat(s,x,-1):sdprintf(s,0,d); }

// quoted any " in string w \"
dstr _sdprinq(dstr s, D d) { dstr v= sdprinc(0,d),p=v;
  while(p&&*p){if(*p=='"')s=sncat(s,"\\",1);s=sncat(s,p++,1);}free(v);return s;}

// TODO: #atom
dstr sdprinq(dstr s, D d) {if (isatom(d)) s= sncat(s,"#",-1);char* p= dchars(d);
  if (isstr(d)) s= sncat(s,"c\"",-1); if (!p) s= sdprinc(s,d); else
    while(*p){ if(*p=='"')s=sncat(s,"\\",1); s=sncat(s,p++,1); }
  if (isstr(d)) s= sncat(s,"\"",-1); return  s; }

// Return a new str from char* S take N chars.
// These strings are trimmed.
//
// TODO: GC and freelist... LOL
D newstr(char* s,int n){int i=0;if(s&&*s)ss[i=sn++]=strndup(s,n);return u2d(BOX(TSTR,i));}

char* dchars(D d) { int t=TYP(d), x=DAT(d);
  return t==TATM? (x&0x3fffff)+hp+1: t==TSTR? (char*)ss[x]: 0;}

// TODO: length of $" char* ?
int dlen(D f) { char* r= dchars(f); return r?strlen(r):TYP(f)==TOBJ?((D*)PTR(TOBJ,f))[3]:0;}

// compare anything
// - if both numbers        => <=>
// - if both strings/atoms  => strcmp
// - num,nan < obj          => -1 (+1) 
// - nan and number
int dcmp(D a, D b) { UL c,d,e=1; switch (!!isnan(a)*10+!!isnan(b)) {
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

// TFUN: 
//
//      TFUN............
//      TFUNzzaaaaa/ffff
//
//    z=2*4= 8 bits =  256 items
//    a=   =22 bits =   4M cells lol
//    f=   =18 bits = 256K strings

// only works w lexical closure
// TODO: "copy" frame
//   if copied closure have no N
//   how to GC it correctly?
//   () store an N last...LOL
//   N doesn't include local added vars
const UL MF=(1UL<<18)-1, MA=(1UL<<22)-1;

D fun(D f, D* A) {
  UL fd= DAT(f), s= fd&MF;
  assert(TYP(f)==TSTR);
  assert(fd<MF);
  assert(A>=K);
  // TODO: size?
  long z= S-A;
  long a= A-K;
  return u2d(BOX(TFUN,z<<(22+18)|a<<18|s));
}

char* alf(char*,D*,int,D*,int); // FORWARD

// when called assumes and args frame on stack, what is this frame then, it's outer function
void funcall(D c) {
  // TOOD: use str?
  long u= DAT(c);
  D f= u2d(BOX(TSTR,u&MF));
  D* A= K+((u>>18)&MA);
  int z= u>>(22+18); // incl locals

  char* e= dchars(f);
  // TODO: need to add a prev frame ptr?
  // when called previous frame (A) should be on the stack as beginning of the frame used in this call...
  // But it points to the outer frame...
  // That means we patch it?
  // most function calls would have 0?
  // as end of chain...
  // TODO: n?
  alf(e,A,z,0,0);
  // No cleanup!
}

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

not currently working

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
  data f= u2d(BOX(1, 7, MDAT));
  //data c= char6nan("gurkafoobar");
  data s= u2d(BOX(1, 7, 4711));

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
