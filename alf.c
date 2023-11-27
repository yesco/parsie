// ALphabetical Forth byte code

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int debug= 1; // DEBUG
#define DEBUG(D) if (debug) do{D;}while(0);

#define SMAX 1024
double S[SMAX]={0};
int sp= 0, memsize= 0;
char *M= NULL, *H= NULL;
#define T S[sp-1]

char* F['Z'-'A'+1]= {0};

void initalf(size_t sz) { H=M=calloc(memsize= sz, 1); } // TODO: ?zero S F

// skips a { block } returns after
char* skip(char* p){ int n=1;while(n&&*p)n+=(*p=='{')-(*p=='}'),p++;return p;}

char*alf(char*p,int args,int n,int iff){long x;char*e=NULL;if(!p)return NULL;
  DEBUG(printf("\n===ALF >>>%s<<<\n", p))

#define NXT goto next;
 next: DEBUG(printf("\t>>> "); for(int i=0; i<sp; i++) printf("%.20lg ", S[i]); printf("\t  '%c'\n", *p))
  switch(*p++){
  case 0:case';':case')':return p;case' ':case'\n':case'\t':case'\r':NXT
  // -- stack stuff
  case 'd': S[sp]= T; sp++;NXT case '\\': sp--;NXT
  case 'o': S[sp]= S[sp-2]; sp++;NXT case 's': x=T; T= S[sp-2]; S[sp-2]= x;NXT

  case '0'...'9': S[sp++]= atoi(p-1); while(isdigit(*p))p++;NXT
  case 'A'...'Z': alf(F[p[-1]-'A'], 0, 0, 0);NXT
  case 'x': { char x[]={S[--sp],0}; alf(x, 0, 0, 0);NXT }

  // -- math stuff
#define L (long)
#define OP(op,e) case #op[0]: S[sp-2]=T op##e S[sp-2]; sp--;NXT
OP(+,);OP(-,);OP(*,);OP(/,);OP(<,);OP(>,);OP(=,=);OP(|,|);OP(&,&);
  case '%': S[sp-2]=L T % L S[sp-2]; sp--;NXT
  case 'z': T= !T;NXT case '~': T= ~L T;NXT case 'n': T= -L T;NXT
  
  // -- memory stuff
  case 'h': S[sp++]= H-M;NXT
  case 'm': x= T; T= H-M;H+=x;NXT
  case 'a': H+=L S[--sp];NXT

  case '@': T=M[8*L T];NXT case '!': M[8*L T]= S[sp-2]; sp-=2;NXT
  // TODO: << >> bit& bit|

  // TODO: STORE case

  case '.': printf("%.20lg ", S[--sp]);NXT
  case 'e': putchar(S[--sp]);NXT
  case '\'': S[sp++]= *p++;NXT
  case '"': while(*p&&*p!='"')putchar(*p++);p++;NXT

  case ':':{char*e=strchr(p,';');if(e) F[*p-'A']=strndup(p+1,e-p),p=e+1;break;}

  // Function call parameters handling
  //                  v--args
  // (11 22 33) -> .. 11 22 33 args
  // [          -> .. 11 22 33 args 3
  // p1 p3      -> 11 33
  // 99]        -> .. 99 (and exit)
    // TODO: by call alf, ret on ')'
  case '(': { int fp= sp; p= alf(p, args, n, 1); S[sp++]= fp;NXT }
  case '[': args= T; n= S[sp]= sp-args-1; sp++;NXT
  case ']': S[args]= T; sp= args+1; return p;

  case 'p': S[sp++]= S[args+*p-'0']; p++;NXT
  case 'v': S[args+*p-'0']= S[--sp]; p++;NXT
    
  // control/IF/FOR/WHILE - ? { }
  case '}': return iff?p:NULL;
  case '{': { char* r; while(!((r=alf(p, args, n, 0)))){}; p= r; NXT }
  // -- control flow
  // ?] = break
  // ?} = again
  // ?{ = if{then}{else}
  case '?': if (S[--sp]) { switch(*p++){
    case '}': return NULL; case ']': return p;
    case '{': p= alf(p, args, n, 1);if (*p=='{') p=skip(p+1);NXT
    default: p=alf(p, args, n, 1);
    }
 } else { // false
    if (*p=='{') { p=skip(p+1); if (!iff) NXT }
    if (p) p= alf(p+1, args, n, 1); else return NULL; }

  default: printf("\n[%% Undefined op '%c']\n", p[-1]);
  }
 NXT
}

// ENDWCOUNT

// -- 41 ops:
// dup \=drop s=swap o=over
// 0=ret ' ' num Call_udf
// + - / * %   < > = & |
// ~ n
// : ; (param) [frame]
// p0-p9=param  v0-v9=set_var
// '=char e=emit "=prstr .=printnum
// x=execute
// ?]=break ?}=continue ?{=if-then-else
// !=store @=read h=here a=allot m="malloc"
//
//
// FREE: , _ ` bc fg ijkl qr u w y
//             128-255
//
//   Missing:
//     bit& bit| bit==?
//     char!@
//     pick key
//     math? lib: use ext w dlopen
//
//     quit type

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
  #
( $ - reserved for parser vars )
  % - mod
  & - and
  ' - char
  ( - param start
  ) - param end
  * - mul
  + - add
( , - write word, here )
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
( ^ - xor )
( _ - name define? )
( ` - address of name? )
  a - allot (inc here)
( b - bit )
( b& )
( b| )
( b^ )
( c - char ops prefix !@ )
  d - drop
  e - emit
  fg
  h - here 
  ij
( k - key key?)
  l
  m - here swap malloc
  n - negate
  o - over
  p - parameter N
  qr
  s - swap
( t - type )
  u
  v - variable (set parameter) N
( w - *8== word+align)?
  x - execute (call char f stack)
  y
  z - =0? zero?, invert?
  { - loop begin
  | - or
  } - loop end
  ~ - bit invert
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
  initalf(16*1024);

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

  // read-eval
  char* ln= NULL; size_t sz= 0;
  while(getline(&ln, &sz, stdin)>=0)
    alf(ln, 0, 0, 0);
  return 0;
}
