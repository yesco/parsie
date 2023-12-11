// ALphabetical Forth byte code

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int debug= 0; // DEBUG
#define DEBUG(D) if (debug) do{D;}while(0);

#define SMAX 1024
double S[SMAX]={0};
int sp= 0, memsize= 0;
char *M= NULL, *H= NULL;
#define T S[sp-1]
#define POP S[--sp]
#define U S[sp++]

char* F['Z'-'A'+1]= {0};

void initalf(size_t sz) { H=M=calloc(memsize= sz, 1); } // TODO: ?zero S F

// Variable Name Bindings on Stack
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

void prstack() { printf("\t>>> ");for(int i=0; i<sp; i++){
  nanprint((data){.d=S[i]});putchar(' ');}}

// run ALF code Program with ARGS
// starting that stack position with
// N items, IFF=1 if running ?{}{}
//
// Returns next position to parse from
//   NULL if done/fail (loop/if)
char*alf(char*p,int args,int n,int iff){long x;char*e=NULL;if(!p)return NULL;
  DEBUG(printf("\n===ALF >>>%s<<<\n", p))

#define NXT goto next;
next: DEBUG(prstack();putchar('\n');printf("\t  '%c'\n",*p))
  switch(*p++){
  case 0:case';':case')':return p; case' ':case'\n':case'\t':case'\r':NXT;
  // -- stack stuff
  case'd': S[sp]= T; sp++;NXT case'\\': sp--;NXT;
  case'o': S[sp]= S[sp-2]; sp++;NXT case 's': x=T; T= S[sp-2]; S[sp-2]= x;NXT

  case'0'...'9': U= atoi(p-1); while(isdigit(*p))p++;NXT
  case'A'...'Z': alf(F[p[-1]-'A'], 0, 0, 0);NXT
  case'x': { char x[]={POP,0}; alf(x, 0, 0, 0);NXT }

  // -- math stuff
#define L (long)
#define SL (sizeof(long))
#define OP(op,e) case #op[0]: S[sp-2]=S[sp-2] op##e T; sp--;NXT
OP(+,);OP(-,);OP(*,);OP(/,);OP(<,);OP(>,);OP(=,=);OP(|,|);OP(&,&);
  case'%': S[sp-2]=L T % L S[sp-2]; sp--;NXT
  case'z': T= !T;NXT case 'n': T= -L T;NXT
  
  // -- memory stuff (, aligns)
  // TODO: malloc/free not use arena
  case'h':U=H-M;NXT case'm':x=T;T=H-M;H+=x;NXT case'a':H+=L POP;NXT
  case',': H=(char*)((((L H)+SL-1)/SL)*SL);memcpy(H,&POP,SL);H+=SL;NXT
  // TODO: use raw offset, alignment error?
  case'@': if (T<0) T=S[(L-T)-1];
    else T=*(double*)&M[8*L T]; NXT
  case'!': if (T<0) S[L-T-1]=S[sp-2]; else *(double*)&M[8*L T]= S[sp-2]; sp-=2;NXT;

  // -- printers (see also $...)
  case'.': nanprint((data){.d=POP});NXT case 'e': putchar(POP);NXT;
  case't': printf("%.*s",(int)T,M+L S[sp-2]);sp-=2;NXT; // counted
  case'\'': U= *p++;NXT case'"':while(*p&&*p!='"')putchar(*p++);p++;NXT;

  // -- define function ;
  case ':':{char*e=strchr(p,';');if(e) F[*p-'A']=strndup(p+1,e-p),p=e+1;break;}

  // Function call parameters handling
  //                  v--args
  // (11 22 33) -> .. 11 22 33 args
  // [          -> .. 11 22 33 args 3
  // p1 p3      -> 11 33
  // 99]        -> .. 99 (and exit)
  case'(': { int fp= sp; p= alf(p, args, n, 1); U= fp;NXT }
  case'[': args= T; n= S[sp]= sp-args-1; sp++; bindenter(args);NXT
  case'^': assert(!"implement");NXT; // TODO: "exit" / break?
  case']': bindexit(); S[args]= T; sp= args+1; NXT; // TODO: return p;???
  case'_': bindadd(parsename(&p));NXT
  case'`': U= -bindfind(parsename(&p))-1;NXT
  case'#': U= atom(parsename(&p)).d;NXT

  // stack value access
  // TODO: _ and ~ and give actual address???
    //  case'p': U= S[args+*p-'0']; p++;NXT
    //case'v': S[args+*p-'0']= POP; p++;NXT
    
  // hacky name variables
  // these should be "scoped" on [ ]
  // What if all variables belonged to a stack frame (no dynamic?)
  // this implies, static, no closure???
  // new functions during parsing
  //  $:NAME define/allocate NAME
  //  $[ enter
  //  $] exit
  // STORE: name= (frameNr,offset,bytes)
  //   frmaenum=0 global?
  //   framenum=1 current
  //           =2 one up
  //   offset=bytes from startOfFrm
  // ALF:
  //  `0#8L8 frame 0 offset 8 size 8
			  
  // control/IF/FOR/WHILE - ? { }
  case'}': return iff?p:NULL;
  case'{': { char* r; while(!((r=alf(p, args, n, 0)))){}; p= r; NXT }
  // -- control flow
  // ?] = break
  // ?} = again
  // ?{ = if{then}{else}
  case'?': if (POP) { switch(*p++){
    case'}': return p; case']': return NULL;
    case'{': p= alf(p, args, n, 1);
      //printf("AFTER THEN: '%s'\n", p);
      if (*p=='{') p=skip(p+1);//NXT
      //printf("AFTER THEN SKIP: '%s'\n", p);
      NXT
    default: p=alf(p, args, n, 1);NXT //??
    }
 } else { // IF==false
  if (*p=='{') { p=skip(p+1); } // if (!iff) NXT }
  //printf("ELSE: '%s'\n", p);
  if (p) {
    p= alf(p+1, args, n, 1);
    //printf("AFTER ELSE: '%s'\n", p);
  }
    else return NULL; } NXT;

  // -- char ops
  case'c': switch(*p++){
    case'@': T=M[L T];NXT;case'!': M[L T]= S[sp-2]; sp-=2;NXT;
    case'i': M[L T]++;sp--;NXT case'd': M[L T]--;sp--;NXT;
    #define SM(a,op) case a: x=T; M[L T] op x; sp-=2;NXT
    SM('+',+=);SM('-',-=);SM('*',*=);SM('/',/=);SM('<',<<=);SM('>',>>=);
    SM('&',&=);SM('|',|=);SM('^',^=);
    // TODO: c,
  }

  // -- word ops
  case'w': switch(*p++){
    case'@': T=*(long*)&M[8*L T];NXT case '!': *(long*)&M[8*L T]= S[sp-2]; sp-=2;NXT
    // TODO: w,
    case'i': (*(long*)&M[8*L T])++;sp--;NXT case'd': (*(long*)&M[8*L T])--;sp--;NXT;
#define SW(a,op) case a: x=S[sp-2]; (*(long*)&M[8*L T]) op x; sp-=2;NXT
SW('+',+=);SW('-',-=);SW('*',*=);SW('/',/=);SW('<',<<=);SW('>',>>=);
  }
  // -- bit ops
  case'b': switch(*p++){
#define LOP(op,e) case#op[0]: S[sp-2]=(L T) op##e L S[sp-2]; sp--;NXT
LOP(&,);LOP(|,);LOP(^,);case'~': T= ~L T;NXT
  }

  // -- string ops
  case'$': x=1;switch(*p++) {
    // TODO: address and not value?
    case'0'...'9': U= S[args+p[-1]-'0'-1];NXT // parameter access
  //T=S[args+p[-1]-'0']; sp++;NXT;
    case's': x=POP; case' ': while(x-->=0)putchar(' ');NXT
    case'n': printf("\n");NXT case'.': printf("%lx\n", L POP);NXT
    // TODO: backslash quotiong \n: $\\ .
    case'"': e=H;while(*p&&*p!='"')*H++=*p++;*H++=0;if(*p)p++;U=e-M;U=H-e-1;NXT
    // TODO: "malloc" allocated string?
    case'#': prstack();putchar('\n');NXT
    default: p--; // error fallthrough
  }
  default: printf("\n[%% Undefined op: '%s']\n", p-1); p++; exit(3); printf("FOO\n");
  }
 NXT
}

// ENDWCOUNT

// ALFabetical (F)orth!
// ====================
// as VM for 'parsie"

// DOUBLE stack!
// Ops default on doubles (64bit).
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

// -- 67 ops:
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
// FREE: _ $ ` g ijkl p qr uv y ~
//       p u v
//       b... c... w... ?...
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
  ! - store
  " - print string
( # - formatting? or printf? )
( $ - reserved for parser vars ) ???
    $0-$9 - get frame parameter N
   '$ '- print blank
    $s - print N blanks
    $. - print hex ($h instead? see $#)
    $n - print newline
    $# - print stack ($. instead?)
  ( $t - print string )
  ( $" - quoted string" - counted? )
  ( $\ - interpret \n etc - counted? )
  ( $( - counted string) )
  ( $[ = another?] )
  % - mod
  & - and
  ' - char
  ( - param start
  ) - param end
  * - mul
  + - add
  , - ret aligned here, write word
  . - print number
  / - div
  0123456789 - number
  : - define
  ; - end define
  < - less than
  = - equal
  > - greater than
  ? - if
  ?] - if true exit {loop}
  ?} - if true begin again
  ?{ - 1?{then}{else}
  @ - read
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
(   c" - c-string? )
(   cr - carriage return $n already )
    c!
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
  d - drop
  e - emit
( f - float (i.e. double ops))
  TODO TODO TODO TODO TODO TODO
  TODO TODO TODO TODO....
    fi - f++
    fd - f--

    f+ - f+=T
    f-
    f*
    f/
    f< - f<<=T
    f> - f>>=T
( g - free? lol )
  h - here (offset into M)
  ij
( k - key key?)
  l
  m - here swap malloc (TODO:use heap)
  n - negate value
  o - over
( p - parameter N ??? )
  qr
  s - swap
  t - type counted string from M
  u
( v - variable (set parameter) N ??? )
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
  ~ - invert (64 bits)
( <del> )
( 128-255 - longname slots? )
  
*/

/*
  = if(C) T [else E]
  C ?{T}{E}

  = for(int i=0; i<27; i++) B
            0`i!
  {        `i@27< !?]
            B
	    `i@1+`i!
            }

  I
  {         C ! ?]
            B
	    S
            }


  = while(C) B
  {         C ! ?]
            B
	    }

  = do B while(C);
  {         B
	    C ?}
 */


int main(int argc, char** argv) {
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
