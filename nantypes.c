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
UL d2u(D d) { RET *(UL*)(&d); } D u2d(UL u) { RET *(D*)(&u); }

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

// Typ:s constants (neg if need to be GC:ed
const UL TNAN=0x7ff8,TATM=0x7ffa,TSTR=0xfffb,TOBJ=0xfffc,TCNS=0xfffe,TFUN=0xfffd;


// TODO? unify with svar + K + M ???

#define HPSIZE 16*1024
char hp[HPSIZE]= {0}; int nilo=0; D nil, undef, error, empty, proto, __;

// TODO: global storage idea
// - treat:SMAX..VMAX

// Add a String to HP limited by SIZE
//
// Searches String in HeaP of SIZE
// (size==-1 doesn't check overflow)
// Adding string if needed.
//
// RETs offset to LEN of string
// followed by a cstring at offset 1.
// (-offset-1 RETed if created)
//
// Extra + (n<<32) atom number
//
//   HP=[len=3, "foo", 8b data]...
//   len=0 means end of list
//   note: assumes zeroed HeaP
long nameadd(char* s) {
char*p=hp;int l=strlen(s),n=0;while(*p){if(!strcmp(s,p+1))
 RET p-hp+((L n)<<22); p+=1+strlen(p+1)+1+sizeof(D);n++;}
  assert(p+l+2-hp < HPSIZE);
  // create new (neg)
  *p=l; RET -(strcpy(p+1,s)-hp-1+((L n)<<22))-1;
}

#define GCn(c,n) kr[c-K]=n
#define GCA(c) GCn(c,2)

void csetmark(D,D,D); // FORWARD

// RET a data atom from String
// empty string RETs nil
D ofsatom(long ofs, char* s){if(empty&&(!s||!*s))RET empty; ofs+=512; assert(ofs>=0 && ofs<0x3ff);
  long x=nameadd(s); D a=u2d(BOX(TATM,(x<0?-x-1:x)|(ofs<<(22+16))));
  if (x<0) { x=-x-1; GCn(G,2);*G++= a; long n= (x>>22)&0xffff; GCn(G,2);*G++= n<3?a:undef; }
  if (ofs<512) { *--Y= undef; *--Y= a; } RET a; }

D atom(char* s) { RET ofsatom(0, s); }

char* dchars(double); // FORWARD

// TODO: set/get??? (JS does...)
D atomize(D d) { RET TYP(d)==TSTR?atom(dchars(d)):d; }

// Search scoped 8atom:
//   1:st in local scope stack
//   2:nd in global vars storage
// (since they are adj just one linear!) 
D varatom(char* s) { D a= atom(s); char* c= dchars(a);
  for(D* x= Y; Y<G; x+=2) if (dchars(*x)==c) RET *x;  RET a; }

// global var storage location
long atomaddr(D a) { RET 1+2*((DAT(a)>>22)&0xffff); }
long atomofs(D a) { RET ((DAT(a)>>(22+16))&0x3ff)-512; }
// RET 1+2*(d2u(a)>32)&0x1ffff; }

int deq(D a, D b) { RET d2u(a)==d2u(b); }

// TODO: isnan isn't working, and atom > number!!!
int ISNAN(double d) { RET (TYP(d)&TNAN)==TNAN; }

int istyped(D v) { int t= TYP(v); RET t!=TNAN && (t&TNAN)==TNAN; }

#define TT(nm, t) int nm(D d) { RET TYP(d)==t; }
TT(isatom,TATM);TT(isobj,TOBJ);TT(isstr,TSTR);TT(iscons,TCNS);TT(isfun,TFUN);

// lowercase is numeric! codes returned:
// i/nt f/loat n/an o/infinite N/il U/ndef E/rror S/ym $/tring O/bj C/ons F/un
char typ(D d) { int t= TYP(d); RET !isnan(d)?(d==L d?'i':isinf(d)?'o':'f')
  :deq(t,nil)?'N':deq(t,undef)?'U':deq(d,error)?'E':t==TATM?'S':t==TSTR?'$'
  :t==TOBJ?'O':t==TCNS?'C':t==TNAN?'n':t==TFUN?'F':0; }

// TODO: remove?
// TODO: it's not aligned
// AtomValPTR is used as address to global var storage in M
// TODO: remove: use svar.c and hibit encoded offsets!
#define AVPTR(d) (TYP(d)!=2?NULL:(D*)&hp[DAT(d)+hp[DAT(d)]+2])

// Get M pointer from offset TYP
// TODO: use m() ???
void* PTR(int t, D o) { RET TYP(o)==t?M+DAT(o):0;}

void inittypes() {
  // Order matters: first 3 == self

  // ---WARNING: Do NOT change order
  //nil=ofsatom(-512,"nil");undef=atom("undef");
  nil=atom("nil");undef=atom("undef");
  //assert(DAT(nil)==nilo);
  K[SMAX-2]=K[SMAX-1]=*S=*K=error =atom("*ERROR*");
  // ---WARNING: Do NOT add above!

  // Add after here...
  // false=atom("false");

  // emtpy becomes same as next?
  // TODO: BUG: fix
  empty=atom("_");

  // true=atom("true");
  // is == atomno >= true
  
  proto=atom("__proto__");

P("%lx\n%lx\n", *(long*)&empty, *(long*)&proto);
  __=atom("__");

  // TODO:
  //   - move obj alloc from H to K
  //   - make a special this ptr at "0"
  //   - make obj.set/.get work w D
  //   - (and iterators/printer)?
  //     (any one knows NPN)
  //this=atom("__this__");
}

#include "common.c"

// how many strings can we handle?
// TODO: make dynamic?
#define ZZ 1024

dstr ss[ZZ]={0}; char sr[ZZ]={0}; int sn=1;

// RET a new str from char* S take N chars.
// These strings are trimmed.
//
// TODO: GC and freelist... LOL
D newstr(char* s,int n){int i=0;if(s&&*s)ss[i=sn++]=strndup(s,n);RET u2d(BOX(TSTR,i));}

char* dchars(D d) { int t=TYP(d), x=DAT(d);
  RET t==TATM? (x&0x3fffff)+hp+1: t==TSTR? (char*)ss[x]: 0;}

// TODO: length of $" char* ?
int dlen(D f) { char* r= dchars(f); RET r?strlen(r):TYP(f)==TOBJ?K[DAT(f)+3]:0;}

int pobj(D); // FORWARD
int cprint(D); // FORWARD

int dprint(D f){char*s=dchars(f);int l=pobj(f);RET l?l:s?P("%s",s):
  iscons(f)?cprint(f):P("%.10g",f);}

// Concatenate D + S as new str
// from Index in S take N chars.
// TODO: optimize?
D strnconcat(D d, D s, int i, int n) { char* x= dchars(s);
  ss[sn]= sncat(sncat(0,dchars(d),-1), x?x+i:0, n); RET BOX(TSTR,sn++); }

////////////////////////////////////////
// TCNS
//
//      TCNS............
//      TCNSoooooooooooo
//
//    o= 48 bits offset into K

D cons(D a, D d) {UL o=C-K;GCA(C);*C++=a;GCA(C);*C++=d;RET u2d(BOX(TCNS, o));}
D car(D c) { RET iscons(c)?K[DAT(c)+0]:nil; }
D cdr(D c) { RET iscons(c)?K[DAT(c)+1]:nil; }
D setcar(D c, D a) { if(iscons(c))csetmark(c,car(c),a); RET iscons(c)?K[DAT(c)+0]=a:error; }
D setcdr(D c, D d) { if(iscons(c))csetmark(c,cdr(c),d); RET iscons(c)?K[DAT(c)+1]=d:error; }
//D setcdr(D c, D d) { RET iscons(c)?K[setmark(DAT(c)+1,d)]=d:error; }

int cprint(D c){ int n= 0; while(iscons(c)){ n+= pc(n?' ':'(')+dprint(car(c));
    c= cdr(c); }  if (!deq(c,nil)) n+=P(" . ")+dprint(c); RET n+=pc(')'); }

// TODO: nested [] ?
D readstr(char** p, char q) {
  char* e=*p; while(**p&&**p!=q)(*p)++; RET newstr(e,(*p)++-e);
}

D append(D a, D b) {if(!iscons(a))RET b;D r=nil,c=r;do{D n=cons(car(a),b);
  if(deq(r,nil))r=c=n;else c=setcdr(c,n);}while(iscons((a=cdr(a))));RET r;}

////////////////////////////////////////
/// Readers & Writers
//

D reader(char** p,int bq); // FORWARD
D obj(); // FORWARD
D set(D,D,D); // FORWARD


// end of line matches any end char...
D readlist(char** p, int bq) {spc(p);if(!**p||(**p==')'&&(*p)++))RET nil;
  if(**p=='@'){assert(bq||!"@ !bq-mode");(*p)++;RET append(POP,readlist(p,bq));}
  D d,a=(**p=='@')?(*p)++,append(POP,readlist(p,bq)):reader(p,bq);spc(p);
  if(**p=='.'){(*p)++;d=reader(p,bq);spc(p);(*p)++;}
    else d=readlist(p,bq); RET cons(a,d); }

// NOTE: commas are optional!
//   [11 22 33] {a:1 b:2} ok!
// NOTE: on errors *p is SET to 0
//   this to not continue execute
//   after error parsing code
D reader(char** p, int bq){ spc(p); switch(**p){ case 0: RET nil;
  case'\'': (*p)++;RET cons(atom("quote"), reader(p,bq));
  case';': if(!bq)assert(!" ; =not bq mode"); (*p)++;RET POP;
  // TODO: for obj... copy all
  //  case'@': {assert(bq||!"@ !bq-mode"); (*p)++;RET append(POP,reader(p,bq));
  case'"': (*p)++; RET readstr(p, '"'); case'(': (*p)++; RET readlist(p,bq);
  case'{':case'[':{char e=**p=='{'?'}':']';D o=obj();(*p)++;while(**p&&**p!=e){
    spc(p);D a=(**p==',')?undef:reader(p,bq);spc(p);if(**p==':'){(*p)++;
    set(o,atomize(a),reader(p,bq));}else set(o,dlen(o),a);if(**p==',')(*p)++;}
  if(*p)(*p)++; RET o; }
  // TODO: other 0x 07 0b1010?
  case'0'...'9': { D d=0;while(isdigit(**p))d=d*10+*(*p)++-'0'; RET d; }
  default:RET atom(parseatomstr(p));
  }
}


////////////////////////////////////////
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

D fun(D f, D* A) { UL fd= DAT(f), s= fd&MF; long z= S-A; long a= A-K;
  assert(TYP(f)==TSTR); assert(fd<MF); assert(A>=K);
  RET u2d(BOX(TFUN,z<<(22+18)|a<<18|s)); }

// when called assumes and args frame on stack, what is this frame then, it's outer function
D funf(D f){ RET TYP(f)==TFUN?u2d(BOX(TSTR,DAT(f)&MF)):0; }
D* fune(D f){ UL u= (DAT(f)>>18)&MA; RET u?K+u:0; }

// TODO: remove? see Z'(' for calls...

void funcall(D c) { // DDEBUG
  // TOOD: use str?
  long u= DAT(c);  // DEBUG
  D f= u2d(BOX(TSTR,u&MF)); // DEBUG
  D* A= K+((u>>18)&MA); // DEBUG
  int z= u>>(22+18); // incl locals

  char* e= dchars(f); // DEBUG
  // TODO: need to add a prev frame ptr?
  // when called previous frame (A) should be on the stack as beginning of the frame used in this call...
  // But it points to the outer frame...
  // That means we patch it?
  // most function calls would have 0?
  // as end of chain...
  // TODO: n?
  //  extern char* alf(char*,D*,int,D*,int); // FORWARD

  assert(!"TODO");
  //alf(e,A,z,0,0); // DEBUG
  // No cleanup!
} // DEBUG

#include "obj.c"

// compare anything
// - if both numbers        => <=>
// - if both strings/atoms  => strcmp
// - num,nan < obj          => -1 (+1) 
// - nan and number
//
// TODO: atom not equal string?
int dcmp(D a, D b) { UL c,d,e=1; switch (!!isnan(a)*10+!!isnan(b)) {
 case 00:RET(a>b)-(a<b);case 01:e=-e;case 10:RET DAT(e>0?a:b)?+256:-256;}
  c=d2u(a),d=d2u(b);if (c==d) RET 0; char *g=dchars(a), *h=dchars(b);
  if (g&&h) RET strcmp(g, h); if (g||h) RET !!h-!!g;  int i=TYP(a),j=TYP(b);
  if (i!=j) RET j-i; if (i==TCNS) {int l=dcmp(car(a),car(b));if(l)RET l;
    RET dcmp(cdr(a),cdr(b));} RET (d<c)-(c<d); }

// GC for managed strings,cons,obj

// reference bits
// 0   == WHITE (free(list))
// 1   == allocated
// 2   == set aside (by set)
// 3   == used in set
// 7...== used/marked several times
// 254 == GRAY (ToScan)
// 255 == BLACK

// 0000 0000 = WHITE (free)
// 0000 0010 = allocated
// 0xxx xx11 = set
// 1111 1110 = unset (GRAY))
// 1111 1111 = BLACK

void bits(unsigned char c) {  for(int i=7; i>=0; i--) pc(!!((c&(1<<i)))+'0'); }

// currently we use eager marking
// 

// <<1 | 1 :  0=>1 1=>11 10=>101 11=>111
// not ++ as it can overflow
#define MRK(c,m) c=(m==-1)?((c)&254):(((c)<<1)|m)

int marks=0, nfree=0, nused=0; // DEBUG

void mark(D *v, int n, int m) { if(!v)return;  UL d= DAT(*v);
  marks++;
  DEBUG(if (debug>2) P("mark.1:\n"));
  DEBUG(if (debug>2) P("mark.2:\n"));
  if (v<K || v>=K+KSZ){DEBUG(if (debug>3)P("%%GC:Pointer out of bounds (C-stack?) i=%ld p=%p, m=%d\n",v-K,v,m));} else {
    // K-address (not C-stack)
    DEBUG(if (debug>2) P("mark.3:\n"));

    int k=kr[v-K];
    int kn=kr[v-K]= m==-1? 254: m;

    //TDOO: loop?
    //if (k==kn) RET; // avoid loop

    DEBUG(if (debug>2) P("mark.4:\n"));

    //if (kr[v-K]<m) GCn(v,m);
    DEBUG(P("%.*sMARK[%ld]#",n,"",v-K);bits(kr[v-K]);P(" ");dprint(*v);P("\n"););
    //if (kr[v-K]>=m){ 
    if (kr[v-K]>=m && m<=255 && m>=0){ // !FORCE
      //if (debug>2) P("..mark..m=%d  ", m);dprint(*v);P("\n");
      DEBUG(if (debug>2) P("mark.5:\n"));

      DEBUG(if(debug>2)P("%.*s- already marked\n",n,""));

// TODO: BUG erh, we just marked it so...
      //RET;
    }
  }

  DEBUG(if (debug>2) P("mark.6:\n"));

  // Not an istyped (i.e. number)
  if (!isnan(*v)) return;

  switch(TYP(*v)){
  case TSTR: MRK(sr[d],m); break;
  case TCNS:
    // CORRECT
    //printf("---TCNS m=%d\n", m);
    mark(K+d+0,n+1,m); return mark(K+d+1,n+1,m);
    while(iscons(*v)) {
      mark(K+d+0,n+1,m); GCn(v,m);
      v=K+d+1; d=DAT(*v);

      DEBUG(P("%.*sMARK[%ld]#",n,"",v-K);bits(kr[v-K]);P(" ");dprint(*v);P("\n"););
      if (kr[v-K]>=m){DEBUG(if(debug>2)P("%.*s- already marked\n",n,""));RET;}
    } mark(v,n+1,m); break;

  case TOBJ:

DEBUG(P("..mark..--m=%d OBJECT: ", m);dprint(*v);P("\n"););

{ D* o=K+d;
  DEBUG(P("OBJ %ld: ",d);dprint(*o);P("\n"););
      // TODO: kr might not be right...
      for(int i=0; i<sizeof(Obj)/SL; i++){
MRK(kr[d+i],m);
mark(o+i,n+1,m);
DEBUG(P("%.*sKR[%d]#",n,"",i);bits(kr[d+i]);P(" ");dprint(o[i]);P("\n"););
}
break; }
  // TODO: case TFUN: sr[DAT(
  }
}

// inherit of cell set in
void csetmark(D c, D o, D n) { UL d=DAT(c);
  return;
  int k= kr[d];
  //debug=1;
  DEBUG(P("\n[csetmark=%d]\n", k));
  mark(&o,2,-1);
  DEBUG(P("\n  [csetmark:NEW:]\n"));
  mark(&n,2,k);
  DEBUG(P("\n[/csetmark=%d]\n", k));
  //debug=0;
}

D* setmark(D* p, D* n) {
  return p;
  // TODO: OOB? neg?
  int k= kr[p-K];
  DEBUG(P("\n[setmark=%d @%ld]\n", k, p-K));
  mark(p,2,-1);
//  P("VALUE:"); dprint(*n); P("\n");

  DEBUG(P("\n  [setmark:NEW:]\n"));
// FORCE: todo, this may loop?
//k=255+256;
///k=255;

 DEBUG(dprint(*n);P("\n"););

// only works with this, still wrong...
k=255+256;

  mark(n,2,k);
  DEBUG(P("\n[/setmark]\n"));
  return p;
}

void xcsetmark(D c, D o, D n) { UL d=DAT(c);
  mark(&o,2,kr[d]<<1);
  mark(&n,2,kr[d]);
}


void sweep() {
  // --- SWEEP - prefer lower first
  DEBUG(P("\n---GC.Free\n"));

  // --- managed strings
  for(int i=sn-1;i>0;i--)if(!sr[i]){P("TSTR[%d]:\"%s\"\n", i, ss[i]);}

  // --- cells
  D FREE=atom("*FREE*");
  char* k=kr+GMAX;
  for(D* c=K+GMAX; c<C; c++,k++)
    if(!*k){
      nfree++;
      //if (!deq(*c,FREE)) {
      DEBUG(P("FREE[%ld]: ", c-K); dprint(*c);P("\n"););
      *c=FREE;
      //}
    }
    else {
      nused++;
      DEBUG(P("used[%ld]#", c-K);bits(*k);P(": ");dprint(*c);P("\n"););
    }

  // --- put in free list
  //for(int i=sn;i;i--)if(!sr[i]){free(ss[i]);ss[i]=ss[0];ss[0]=(char*)(long)i;}

  // -- print free list
  if (0) {
    DEBUG({long f= 0; P("Free: "); do P("%lu=>", f); while((f=(long)ss[f]));P("\n");});
  }
}

int dogc= 0; // FORWARD

void gc(D* start, D* end) { int omarks=marks,ofree=nfree,oused=nused;
  D t= stime(); marks=nfree=nused=0;
  memset(sr, 0, sizeof(sr));
  assert(kr);
  char* cend= kr+(C-K);
  // incremental? TODO: it's incorrect...
  //for(char* k=kr+GMAX; k<cend; k++) if (!(*k&1)) *k=0; // *k=*k&~(2+4);

  for(char* k=kr+GMAX; k<cend; k++) *k=0;

  // --- MARK: sr[x]++ for each ref
  if (start && end) {
    DEBUG(P("\n---GC.CTACK\n\n"));
    for(D* s=start; s>=end; s--) {
      if (!isnan(*s)) continue;
      DEBUG(P("CTACK[%4ld]: ", start-s);dprint(*s);P("\n"););
      mark(s,2,4);
    }
  }

  DEBUG(P("\n---GC.STACK\n\n"));
  for(D* s=K;      s<=S; s++) {
    kr[s-K]=0;
    mark(s,0,8);
  }

  DEBUG(P("\n---GC.Globals\n"));
  for(D* g=K+SMAX; g<=G; g++) {
    kr[g-K]=0;
    mark(g,0,255);
  }

if (0) {
  DEBUG(P("\n--GC.all marked already\n"));
  for(D* c=K+GMAX; c<=C; c++)
    if (kr[c-K]==255) {
      kr[c-K]=0;
      mark(c,2,255);
    }
}

  sweep();
  if (dogc) {
    P("\n[GC: %7.6fs free:%+d used:%+d", stime()-t, nfree-ofree, nused-oused);
    if (debug) 
      P("(marks=%d nfree=%d nused=%d)", marks, nfree, nused);
    P("]\n");
  }
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
  RET r;
}

// RET a static string copy of
// string encoded as NaN.
//   RETs NULL if not string (T!=0)
char* nanchar6(data d) { static char s[7]= {0}; if (TYP(d)!=1) RET NULL;
  RET memcpy((char*)s, d.str.ch, sizeof(d.str.ch));
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
