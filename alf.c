//80----------------------------------------------------------------------------
// ALphabetical Forth byte code

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int debug= 0; // DEBUG
#define DEBUG(D) if (debug) do{D;}while(0);

#include "mem.c"

// Variable Name Bindings on Stack
#include "nantypes.c"

#include "obj.c"

// Interpreation of memory ref
//inline void* m(double d, int args, int n) {
void* m(double d, int args, int n) { if (isnan(d)) { long x= DAT(d);
    DEBUG(printf(" TYP=%x\n", TYP(d)));
    switch(TYP(d)){ case TATM: case TSTR: return dchars(d);
    case TOBJ: case TCONS: case TENV: case TNAN: default: return 0; //TCLOS:
    }}
  long x=L d;return x<0?K+args+n:(x>MLIM?M+x-MLIM:x<VMAX?(void*)(K+SMAX+x):0);
}

// Parse from P a name
// (P is pointer to char* and updated)
//
// Returns a static string (use fast!)
char* parsename(char** p) { static char s[64], i; i=0; while((isalnum(**p)
   ||**p=='_') && i<sizeof(s)-1) s[i++]=*(*p)++; s[i]= 0; return s;}

// skips a { block } returns after
//   TODO: must skip STRINGs!!! lol
char* skip(char* p){ int n=1;while(n&&*p)if(*p=='?'&&p[1]!='{')p+=2;else n+=(*p=='{')-(*p=='}'),p++;return p;}

void prstack(){P("\t:");for(D* s= K+1; s<=S; s++){dprint(*s);pc(' ');}} // DEBUG

#define Z goto next; case

// run ALF code Program with ARGS
// starting that stack position with
// N items, IFF=1 if running ?{}{}
//
// Returns next position to parse from
//   NULL if done/fail (loop/if)
char* alf(char*p,int args,int n,int iff) {long x;char*e=NULL,*d;if(!p)return 0;
DEBUG(P("\n===ALF >>>%s<<<\n", p))
next: DEBUG(prstack();pc('\n');P("\t  '%c'\n",*p))
x=0;switch(*p++){ case 0:case';':case')':return p; Z' ':Z'\n':Z'\t':Z'\r':
Z'd': S[1]= *S; S++; Z'\\': S--; // TODO: more?
Z'o': S[1]= S[-1]; S++; Z's': {D d=*S; *S= S[-1]; S[-1]= d;}
Z'0'...'9':{D v=0;p--;while(isdigit(*p))v=v*10+*p++-'0';U=v;}
Z'A'...'Z': alf(F[p[-1]-'A'],S-K,0,0); Z'x':{char x[]={POP,0};alf(x,0,0,0); }
#define OP(op,e) Z #op[0]: S--;*S=*S op##e S[1];
OP(+,);OP(-,);OP(*,);OP(/,);OP(<,);OP(>,);OP(=,=);OP(|,|);OP(&,&);
Z'%': S--; *S=L *S % L S[1]; Z'z': *S= !*S; Z'n': *S= -L *S;
Z'h': U=H-M; Z'm':x=*S;*S=H-M;H+=x; Z'a':H+=L POP;
Z'g': case ',': align(); if (p[-1]=='g') goto next; memcpy(H,&POP,SL); H+=SL;
Z'@': *S= *(D*)m(*S,args,n); Z'!': *(D*)m(*S,args,n)= S[-1]; S-=2;
Z'c': switch(*p++){ Z'!': S--; Z'@': memcpy(d, e, x); Z'r':pc('\n');
  Z'"': e=p; while(*p&&*p!='"')p++; U=newstr(e, p++-e); Z'c': *S=dlen(*S); }
Z'.': dprint(POP);pc(' '); Z'e':pc(POP); Z'p': dprint(POP);
Z't': P("%*s.",(int)*S,M+L S[-1]);S-=2;
  Z'\'': U= *p++; Z'"': while(*p&&*p!='"')pc(*p++); p++;
Z':': e=strchr(p,';'); if(e) F[*p-'A']=strndup(p+1,e-p),p=e+1;
Z'^': K[++args]= *S; S=K+args; return p-1;
Z'#': switch(*p++){ Z'a'...'z':case'A'...'Z':case'_':p--;U=atom(parsename(&p));
  Z'<': S--;set(*S,dlen(*S),S[1]); Z':': S-=2;set(*S,S[1],S[2]);
  Z'@': S--;*S=get(S[1],*S); Z'!': S-=3;set(S[3],S[2],S[1]);
  Z'?': *S=typ(*S); Z'^': U=obj(); goto next; default: goto error;
}
Z'}': return iff?p:NULL; Z'{': while(!((e=alf(p, args, n, 0)))){}; p= e;
Z'?': if (POP) { switch(*p++){ Z'}': return p; Z']': return 0;
  Z'{': p=alf(p,args,n,1);if(*p=='{')p=skip(p+1); default:p=alf(p,args,n,1);
 }} else { // IF==false
  if (*p=='{') { p=skip(p+1); } if (p) p= alf(p+1, args, n, 1); else return 0; }
#define LOP(op,e) Z#op[0]: S--; *S=(L S[1]) op##e L *S;
Z'b': switch(*p++){ LOP(&,);LOP(|,);LOP(^,); Z'~': *S= ~L *S; }
Z'`': switch(*p++) { Z'#': U=n; Z'0'...'9': U=p[-1]-'0'-n-1;}
Z'$': x=1;switch(*p++){ Z'.': prstack(); case'n': pc('\n');
  Z'0'...'9':S++;*S=K[args+p[-1]-'0']; Z'$':n=POP;args-=n; Z'd': x=S-K; U=x; 
  Z'!': K[args+*p++-'0'-1]=POP; Z's':x=POP;case' ':while(x-->=0)pc(' ');
  Z'"': e=H;while(*p&&*p!='"')*H++=*p++; *H++=0;if(*p)p++; U=e-M; U=H-e-1;
  Z'h': P("%lx\n", L POP);goto next; default: p--; // err
  Z'D': for(int i=0; i<*S;) { int n= S[-1]; printf("\n%04x ", n); // DEBUG
    for(int j=0; j<8; j++) printf("%02x%*s", M[n+j], j==3, "");  printf("  "); // DEBUG
    for(int j=0; j<8; j++) printf("%c", M[n+j]?(M[n+j]<32||M[n+j]>126? '?': M[n+j]):'.'); // DEBUG
    D d= *(D*)(M+n); x=TYP(d); if ((x&0x0ff8)==0x0ff8) { // DEBUG
      if (x>32*1024) x=-(x&7); else x= x&7; } else x=0; // DEBUG
    printf(" %2ld:", x);dprint(d);S[-1]+=8;i+=8;}}printf("\n");prstack();break; // DEBUG
  default: error: P("\n[%% Undefined op: '%s']\n", p-1);p++;} goto next;
}

// fib 19-27% faster!
char* opt(char* p) { char *s= p; while(s&&s[0]&&s[1]&&s[2]){switch(s[0]){
  case'"': while(*s && *++s!='"'){}break;  case '#': s++; parsename(&s);break;
  case'`': break; /* TODO */ case'0'...'9': if(isdigit(s[2]))break;
  default: if (!isspace(s[1])) break; memmove(s+1, s+2, strlen(s+2)+1);continue;
  } s++; } DEBUG(printf("\n%s\n", p)); return p; }

// ENDWCOUNT

// ALFabetical (F)orth!
// ====================
// as VM for 'parsie"

// D stack!
// Ops default on Ds (64bit).
// Cast to long for bit ops.
// int32 is safe and correct!
// memory is addressed using offset
//   (might use NAN-type)
// 
// TODO: strings (using pointers?)
//   but want them tagged as strings
//
// StackFrames are maintained using
//   ( ... ) mark stack parameters
//   [ ... ] enter stack frame
//   This enables varargs etc.

// -- 94 ops: (TODO: update)
// stack:	dup \=drop s=swap o=over
// delim:       ' '
// numbers      0-9... integers
// math:	+ - / * % n=negate
// logical:	< > = & |
// read/write:	!=store @=read
// memory:	a=allot h=here m="malloc" (no free!)
// user funcs:	A-Z
// define:	: X ... ;
// printing     . 'c e "..."==print
// x=execute
// conditional:	?{=if-then-else
// loops:	{ ...?]=break ?}=cont }

// b& b| b^ b~
// c@ c! ci cd c+ c- c* c/ c< c> c& c| c^
// w@ w! wi wd w+ w- w* w/ w< w> w& w| w^
//
// FREE:    () [] _ f ijkl qr uv y ~
// PREFIX:  $ ? b c w ? c
//             128-255
//    planned: f...
//
//   Missing:
//     pick key key?
//     math? lib: use ext w dlopen
//
//     exit break quit type

/*
  == ctrl-a -- ctrl-z reserve for edit?
  0 - end of c-string
  1 ctrl-a start of heading
  2 ctrl-b start of text
  3 ctrl-c end of text
  4 ctrl-d end of transmission
  5 ctrl-e enquire
  6 ctrl-f acknowledge
( 7 ctrl-g - abort/alarm/interrupt? )
( 8 ctrl-h - rubout )
  9 ctrl-i - tab/indent
 10 j lf - down
 11 k vt - up
 12 l ff - new/clear page
 13 m cr - beginning line
(14 n shift out )
(15 o shift in )
 16 p data link escape
 17 r device control one (XON)
 18 s device ccontrol two
 19 t device control three (XOFF)
 20 u device control four
 21 v negative acknowledge
 22 w sync idle
 23 x end of transmission block
 24 y cancel
 25 z end of medium
 26   substitue
 27 [ escpae
 28   file separator
 29   group separator
 30   record separator
 31   unit separator
  <spc> - delimiter
  ! - store (& prefix by c/w/l)
  " - print string
  # - atom/hash stuff
    #abc - make atom (alnum)

    #? - type? -> i)nt f)float n)nan N)il A)tom S)tring O)bj C)ons U)ndef
         lowercase ( >- 'A' is  "num" )

    #@ - obj: get prop value (N O - V)
    #! - obj: add value,name (V N O -)
    #^ - obj: new (- O)
    (-these work on reverse order args)
    #: - obj: set Value Name (O N V - O)
    #< - push on array/obj (O V - O)
  ( #, - alias? )
  ( #> - pop )
  ( #{ - unshift )
  ( #} - shift )
    cc - returns "len" of obj
  ( ## - use instead? )
  
  ( #" - make atom fr string? )
  ( ## - hash a value? lol )
  ( #$ - ? )
    #%
    #&
  ( #' - str/char? name? )
    #( - special formatting lang?
    #)
  ( #* - ptr? )
  ( #+ - insert? )
  ( #, - add N values / array? )
  ( #- - remove? )
  ( #. - print N values? )
  ( #/ - truncate )

    #:
    #;
    #=
    #[
    #]
  ( #\ - drop N items )
  ( #^ - exit N '}' ? )

  $ - string functions
    $1 - $9 - get frame parameter N
    $!1 - $!9 - set frame parameter N
   '$ '- print space
    $s - print N spaces
    $h - print hex
    $. - print stack (.S normally)
    $d - depth of stack
  ( $p - printf? )
    $D - dump from address / DEBUG
  ( $# - format number? tra forth? )

    $" - counted mem string
  ( $\ - interpret \n etc - counted? )
  ( $( - counted string) )
  ( $[ - another?] )
  ( $l - strlen - see cc ? )
  ( $c - copy )
  ( $m - move )
  % - mod
  & - and
  ' - char
  ( 
  ) 
  * - mul
  + - add
  , - ret aligned here, write word
  - - minus
  . - print data w space (see . e t p q)
  / - div
  0123456789 - number
  : - define
  ; - end define
  < - less than
  = - equal
  > - greater than
  ? - if
    ?{ - IF-stmt 1?{then}{else}
    ?] - if true exit {loop} = break
    ?} - if true begin again = cont
  @ - read (& prefix by c/w/l)

  ABCDEFGHIJKLMNOPQRSTUVWXYZ - UDF
  [
  \ - drop
  ]
  ^ - exit/return (TODO: ??? break)
  _
  ` - prefix: addresses?
    `1-`9 - address of N;th parameter
    `@    - number of args
    ( $1-$9 - see this )
  a - allot (inc here, from arena)
  b - bit ops:
    b& - bit & (64 bit)
    b| - bit | (64 bit)
    b^ - bit ^ (64 bit)
    b~ - bit ~ (64 bit)
  c - char ops prefix !@
    c" - managed null-terminated string
    cc - count=length of mngd/"ptr"/#atom
         TODO: name co?
    cr - carriage return
  ( ca - append 2 mngd str->mngd )
  ( cm - cmove ? TODO: )
    c! - 
    c@ -
    c, - TODO
    ci - c++
    cd - c--
    c+ - c+=T 
    c-
    c*
    c/
    c< - c<<=T -- TODO: c{ update skip
    c> - c>>=T -- TODO: c} update skip

  cons - co cc pc p^
  car  - ca c0 pa p0
  cdr  - cd c1 pd p1

  d - dup
  e - emit (see . e t p q )
( f - free for malloced ptr??? )
    fi - f++
    fd - f--

  ( can put on $ ? )
    f+ - f+=T
    f-
    f*
    f/
    f< - f<<=T
    f> - f>>=T
  g - aliGn (auto align for , ?)
  h - here (offset into M)
  ij
( k - key key?)
( l - long )
  m - here swap malloc (TODO:use heap)
  n - negate value
  o - over
  p - dprint w/o space (see . e t p q )
  q - quote print (print so can read)
  r
  s - swap
  t - type counted string from M (see .etpq)
  uv
  w - word/long ops prefix !@
    w! - long!
    w@ - long@
    w, - TODO
    wi - w++
    wd - w--

    w+ - w+=T
    w-
    w*
    w/
    w< - w<<=T -- TODO: w{ update skip
    w> - w>>=T -- TODO: w} update skip
  x - execute (call char f stack)
  y
  z - !T or =0? zero?
  { - loop begin
  | - or
  } - loop end
( ~ - ??? label/goto/continue/break? )
( <del> )
( 128-255 - optimized atomnames/addr )
*/

int main(int argc, char** argv) {
  assert(sizeof(long)==8);
  assert(sizeof(void*)==sizeof(D));
  assert(sizeof(void*)==sizeof(long));
  assert(sizeof(void*)==sizeof(long));

  // parse arguments:
  // -d - increase debug level with 1 !
  //   TODO: -q = exit on error
  //   TODO: -s = stack size
  //   TODO: -v = variable size
  //   TODO: -h = heap size
  //   TODO: -m = mem size
  do{
    if (0==strcmp("-d", argv++[0])) debug++;
  } while(--argc);

  initmem(16*1024); inittypes();

if(0){
  alf("3d.4d.+. 44444d. 1111111d. + . 3 3=. 4 3=. 1 0|. 7 3|. 1 0&. 1 3&.", 0, 0, 0);
  alf(":A666;333Ad.+.", 0, 0, 0);
  alf(":C[p0];:B[.p0.p1.p2.p3.(999 222)C];(777 888)666(11 22 33)(44 55 66 77)B.", 0, 0, 0);
  alf("666 'A . 'a. ' .", 0, 0, 0);
  alf("\"Foo=\" 42. '\"e", 0, 0, 0);
  alf(":P...; 11 22 33 'P x", 0, 0, 0);
  alf("1.{2.1 3<?}3.", 0, 0, 0);
}

  //alf("1.{2.}3.", 0, 0, 0);
//alf("1d.?{2d.}{3d.}.4.", 0, 0, 0);
//  alf("0d.?{2d.}{3d.}.4.", 0, 0, 0);

  if (0) {
    printf("1 %s\n", skip("foo}OK"));
    printf("2 %s\n", skip("f?]oo}OK"));
    printf("3 %s\n", skip("f?}oo}OK"));
    printf("4 %s\n", skip("f?{oo}OK"));
    printf("5 %s\n", skip("f{o}o}OK"));
    printf("6 %s\n", skip("1}{OK}"));
    exit(3);
  }

  // read-eval
  char* ln= NULL; size_t sz= 0;
  while(getline(&ln, &sz, stdin)>=0) {
    alf(opt(ln), 0, 0, 0);
    if (S<=K) { printf("\n%%STACK underflow %ld\n", S-K); } // DEBUG
    if (S>=K+SMAX) { printf("\n%%STACK overrun\n"); } // DEBUG
    if (!deq(K[SMAX],error)) { printf("\n%%STACK corrupted/overrun to cons/var storage\n"); } // DEBUG
  }
}
