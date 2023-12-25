//80----------------------------------------------------------------------------
// ALphabetical Forth byte code

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int debug= 0; // DEBUG
#define DEBUG(D) if (debug) do{D;}while(0);

#define SMAX 1024
double S[SMAX]={0}; int sp=0,memsize=0; char *M=0, *H=0, *F['Z'-'A'+1]={0};

#define T S[sp-1]
#define POP S[--sp]
#define U S[sp++]

#define L (long)
#define SL (sizeof L)
#define P printf
#define align() while((H-M)%SL&!(H-M))H++

void initalf(size_t sz) { H=M=calloc(memsize= sz, 1); } // TODO: ?zero S F

// Variable Name Bindings on Stack
#include "nantypes.c"
#include "varz.c"

// Parse from P a name
// (P is pointer to char* and updated)
//
// Returns a static string (use fast!)
char* parsename(char** p) { static char s[64], i; i=0; while((isalnum(**p)
   ||**p=='_') && i<sizeof(s)-1) s[i++]=*(*p)++; s[i]= 0; return s;}

// skips a { block } returns after
//   TODO: must skip STRINGs!!! lol
char* skip(char* p){ int n=1;while(n&&*p)if(*p=='?'&&p[1]!='{')p+=2;else n+=(*p=='{')-(*p=='}'),p++;return p;}

void prstack(){P("\t:");for(int i=0;i<sp;i++){dprint(S[i]);putchar(' ');}} // DEBUG

// run ALF code Program with ARGS
// starting that stack position with
// N items, IFF=1 if running ?{}{}
//
// Returns next position to parse from
//   NULL if done/fail (loop/if)
char* alf(char*p,int args,int n,int iff) {long x;char*e=NULL,*d;if(!p)return 0;
DEBUG(P("\n===ALF >>>%s<<<\n", p))
#define Z goto next; case
next: DEBUG(prstack();putchar('\n');P("\t  '%c'\n",*p))
x=0;switch(*p++){ case 0:case';':case')':return p; Z' ':Z'\n':Z'\t':Z'\r':
// -- stack stuff
Z'd': S[sp]= T; sp++; Z'\\': sp--; // TODO: more?
Z'o': S[sp]= S[sp-2]; sp++; Z's': x=T; T= S[sp-2]; S[sp-2]= x;

Z'0'...'9':{D v=0;p--;while(isdigit(*p))v=v*10+*p++-'0';U=v;}
Z'A'...'Z': alf(F[p[-1]-'A'],sp,0,0); Z'x':{char x[]={POP,0};alf(x,0,0,0); }
    //Z'A'...'Z':alf(F[p[-1]-'A'],args,n,0); Z'x':{char x[]={POP,0};alf(x,0,0,0); }

// -- math stuff
#define OP(op,e) Z #op[0]: S[sp-2]=S[sp-2] op##e T; sp--;
OP(+,);OP(-,);OP(*,);OP(/,);OP(<,);OP(>,);OP(=,=);OP(|,|);OP(&,&);
Z'%': S[sp-2]=L T % L S[sp-2]; sp--; Z'z': T= !T; Z'n': T= -L T;

// -- memory stuff (, aligns)
// TODO: malloc/free not use arena
Z'h': U=H-M; Z'm':x=T;T=H-M;H+=x; Z'a':H+=L POP;
Z'g': case ',': align(); if (p[-1]=='g') goto next; memcpy(H,&POP,SL); H+=SL;

Z'@': T=T<0?S[args+n+L T]:*(long*)(M+8*L T);
Z'!': x=POP; if(x<0) S[args+x]=S[sp-1]; else *(long*)(M+8*L x)= S[sp-1]; POP;
  
// TODO: not good/aligned?
//Z'l':case'!':case'@':x+=4;case'w':x+=3;case'c':x++;d=(char*)&T;e=T<0?(char*)S+L-T-1:M+8*L T;
// LOL: some "overlap"
Z'c':
  switch(*p++){ Z'!': sp--; Z'@': memcpy(d, e, x); Z'r':putchar('\n');
  Z'"': e=p; while(*p&&*p!='"')p++; U=newstr(e, p++-e); Z'c':T=dlen(T);
}
 // -- printers (see also $...)
Z'.': dprint(POP);Z'e':putchar(POP);Z't': P("%*s.",(int)T,M+L S[sp-2]);sp-=2;

Z'\'': U= *p++; Z'"': while(*p&&*p!='"')putchar(*p++); p++;

// -- define function ;
Z':': e=strchr(p,';'); if(e) F[*p-'A']=strndup(p+1,e-p),p=e+1;

// Exit (Function) removes args
Z'^': S[args]= T; sp=args+1; return p;

// TODO: only lisp need atoms? globals?
//Z'#': U= atom(parsename(&p));

// control/IF/FOR/WHILE - ? { }
Z'}': return iff?p:NULL; Z'{': while(!((e=alf(p, args, n, 0)))){}; p= e;
// -- control flow
// ?] = break
// ?} = again
// ?{ = if{then}{else}
// probgably not correctif inside IFF???? how to propagate several layers up? special return code?
Z'?': if (POP) { switch(*p++){ Z'}': return p; Z']': return 0;
  Z'{': p=alf(p,args,n,1);if(*p=='{')p=skip(p+1); default:p=alf(p,args,n,1);
 }} else { // IF==false
  if (*p=='{') { p=skip(p+1); } if (p) p= alf(p+1, args, n, 1); else return 0; }

// -- bit ops
Z'b': switch(*p++){
  #define LOP(op,e) Z#op[0]: S[sp-2]=(L T) op##e L S[sp-2]; sp--;
  LOP(&,);LOP(|,);LOP(^,); Z'~': T= ~L T;
}

Z'`': switch(*p++) {
  Z'0'...'9': U=p[-1]-'0'-n-1;
}
  
// -- string ops
Z'$': x=1;switch(*p++){ Z'.': prstack(); case'n': putchar('\n');
  Z'0'...'9':U=S[args+n+p[-1]-'0'-n-1]; Z'd': S[sp]=sp;sp++;
  Z'$': n=POP;args-=n;
  Z'!': S[args+*p++-'0'-1]=POP; Z's':x=POP;case' ':while(x-->=0)putchar(' ');
    // TODO: quotes?
  Z'"': e=H;while(*p&&*p!='"')*H++=*p++; *H++=0;if(*p)p++; U=e-M; U=H-e-1;
  Z'h': P("%lx\n", L POP);goto next; default: p--; // err
}

default: P("\n[%% Undefined op: '%s']\n", p-1);p++;exit(3);} goto next;
}

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

// -- 94 ops:
// stack:	dup \=drop s=swap o=over
// delim:       ' '
// numbers
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

// Stack frame param access:
// 	p0-p9=param  v0-v9=set_var
// 
// b& b| b^ b~
// c@ c! ci cd c+ c- c* c/ c< c> c& c| c^
// w@ w! wi wd w+ w- w* w/ w< w> w& w| w^
//
// FREE:    f ijkl pqr uv y ~
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
( # - formatting? or printf? )
  $ - string functions
    $d - depth (of stack)
    $1 - $9 - get frame parameter N
    $!1 - $!9 - set frame parameter N
   '$ '- print blank
    $s - print N blanks
    $h - print hex
    $. - print stack (.S normally)
  ( $# - format? )

    $" - counted string
  ( $\ - interpret \n etc - counted? )
  ( $( - counted string) )
  ( $[ - another?] )
    $l - strlen
  ( $c - copy )
  ( $m - move )
  % - mod
  & - and
  ' - char
  ( - param start
  ) - param end
  * - mul
  + - add
  , - ret aligned here, write word
  . - print data (D/atom)
  / - div
  0123456789 - number
  : - define
  ; - end define
  < - less than
  = - equal
  > - greater than
  ? - if
    ?] - if true exit {loop} = break
    ?} - if true begin again = cont
    ?{ - IF-stmt 1?{then}{else}
  @ - read (& prefix by c/w/l)

  ABCDEFGHIJKLMNOPQRSTUVWXYZ - UDF
  [ - enter parameter frame
  \ - drop
  ] - exit param frame
( ^ - break  b~ for xor )
  _ - stack name define
  ` - address of stack name
  a - allot (inc here, from arena)
  b - bit ops:
    b& - bit & (64 bit)
    b| - bit | (64 bit)
    b^ - bit ^ (64 bit)
    b~ - bit ~ (64 bit)
  c - char ops prefix !@
    c" - managed null-terminated string
    cc - count=length of mngd/"ptr"/#atom
    cr - carriage return $n already
  ( ca - append 2 mngd str->mngd )
  ( cm - cmove ? TODO: )
    c! - 
    c@
    c, - TODO
    ci - c++
    cd - c--
    c+ - c+=T 
    c-
    c*
    c/
    c< - c<<=T -- TODO: c{ update skip
    c> - c>>=T -- TODO: c} update skip
  d - dup
  e - emit
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
  pqr
  s - swap
  t - type counted string from M
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
  ~
( <del> )
( 128-255 - longname slots? )
  
*/

int main(int argc, char** argv) {
  assert(sizeof(long)==8);
  assert(sizeof(void*)==sizeof(D));
  assert(sizeof(void*)==sizeof(long));
  assert(sizeof(void*)==sizeof(long));

  // parse arguments
  do{
    if (0==strcmp("-d", argv++[0])) debug++;
  } while(--argc);

  inittypes(); initalf(16*1024);

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
  while(getline(&ln, &sz, stdin)>=0)
    alf(ln, 0, 0, 0);
  return 0;
}
