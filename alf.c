// ALphabetical Forth byte code

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define SMAX 1024
double S[SMAX]={0};
int sp= 0;

char* F['Z'-'A'+1]= {0};

// 31 ops:
// dup \=drop s=swap 0=ret ' ' num call
// + - / * %   < > = ! & | <! >!
// ~ n . e(emit)
// : ; ( ) [ ] p=param v=variable
// '=char "=printstring
// x=execute
//
//   Missing:
//     bit& bit| bit==?
//     over pick key
//     math? library use external dlopen?
//
//     quit if type for loop while

/*
  <spc> - delimiter
( ! - store )
  " - print string
  #
  $
  % - mod
  & - and
  ' - char
  ( - param start
  ) - param end
  * - mul
  + - add
( , - write word, here )
  - - invert bits
  . - print number
  / - div
  0123456789 - number
  : - define
  ; - end define
  < - less than
  = - equal
  > - greater than
( ? - if )
( @ - read )
  ABCDEFGHIJKLMNOPQRSTUVWXYZ - sub
  [ - enter param
  \ - drop
  ] - exit param
( ^ - xor )
  _
( ` - address? )
  a
( b - bit )
( b& )
( b| )
( b^ )
  c
  d - drop
  e - emit
  fg
( h - here )
  ijkl
( m - malloc/alloc? here )
  n - negate
( o - over )
  p - parameter N
  qr
  s - swap
( t - type )
  u
  v - variable (set parameter) N
  w
  x - execute (call char f stack)
  y
  z - =0? zero?, invert?
  {
  | - or
  }
  ~ - bit negate
  <del>
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

// skips a { block } returns after
char* skip(char* p) {
  printf("skipping.... >>>%s<<<\n", p);
  if (!p) return NULL;
  int n= 1;
  while(n && *p) {
    n+= (*p=='{') - (*p=='}');
    p++;
  }
  printf("SKIPPED..... >>>%s<<<\n", p);
  return p;
}


char* alf(char* p, int args, int n, int iff) {
  printf("\n===ALF >>>%s<<<\n", p);
  if (!p) return NULL;
  int x; char* e= NULL;
 next:
  printf("\n>>> "); for(int i=0; i<sp; i++) printf("%.20lg ", S[i]); printf("\t  '%c'\n", *p);
  switch(*p++){
  case 0: case ';': case ')': return p;
  case ' ': goto next;
  case 'd': S[sp]= S[sp-1]; sp++; break; case '\\': sp--; goto next;
  case 's': x=S[sp-1]; S[sp-1]= S[sp-2]; S[sp-2]= x; goto next;
  case '0'...'9': S[sp++]= atoi(p-1); while(isdigit(*p))p++; goto next;
  case 'A'...'Z': alf(F[p[-1]-'A'], 0, 0, 0); goto next;
  case 'x': { char x[]={S[--sp],0}; alf(x, 0, 0, 0); goto next; }

#define OP(op,e) case #op[0]: S[sp-2]=S[sp-1] op##e S[sp-2]; sp--; goto next;
OP(+,);OP(-,);OP(*,);OP(/,);OP(<,);OP(>,);OP(=,=);OP(|,|);OP(&,&);
  case '%': S[sp-2]=(long)S[sp-1] % (long)S[sp-2]; sp--; goto next;
  case '~': S[sp-1]= ~(long)S[sp-1]; goto next;
  case 'n': S[sp-1]= -(long)S[sp-1]; goto next;
  case 'z': S[sp-1]= !S[sp-1]; goto next;
    
  // TODO: << >> bit& bit|

  // TODO: STORE case

  case '.': printf("%.20lg ", S[--sp]); goto next;
  case 'e': putchar(S[--sp]); goto next;
  case '\'': S[sp++]= *p++; goto next;
  case '"': while(*p&&*p!='"')putchar(*p++); p++; goto next;

  case ':':{char*e=strchr(p,';');if(e) F[*p-'A']=strndup(p+1,e-p),p=e+1;break;}

  // Function call parameters handling
  //                  v--args
  // (11 22 33) -> .. 11 22 33 args
  // [          -> .. 11 22 33 args 3
  // p1 p3      -> 11 33
  // 99]        -> .. 99 (and exit)
    // TODO: by call alf, ret on ')'
  case '(': { int fp= sp; p= alf(p, args, n, 1); S[sp++]= fp; goto next; }
  case '[': args= S[sp-1]; n= S[sp]= sp-args-1; sp++; goto next;
  case ']': S[args]= S[sp-1]; sp= args+1; return p;

  case 'p': S[sp++]= S[args+*p-'0']; p++; goto next;
  case 'v': S[args+*p-'0']= S[--sp]; p++; goto next;
    
  // control/IF/FOR/WHILE - ? { }
  case '}': return iff?p:NULL;
  case '{': {
    char* r;
    while(!((r=alf(p, args, n, 0))));
    p= r;
    goto next;
  }
  case '?': {
    // maybe if find {} etc just patch
    // with jumps? (128..255)
    if (S[--sp]) { // true
      switch(*p++){
      case '}': return NULL; // again
      case ']': return p; // break
      case '{': p= alf(p, args, n, 1);
	printf("AFTER ALF >>>%s<<<\n", p);
	if (*p=='{') p= skip(p+1);
	goto next; // if
      default: p=alf(p, args, n, 1);
      }
    } else { // false
      if (*p=='{') {
	p= skip(p+1);
	if (!iff) goto next;
	//return p;
	//goto next;
      }
      if (p) p= alf(p+1, args, n, 1);
      else return NULL;
    }
    goto next;
  }

  default: printf("\n[%% Undefined op '%c']\n", p[-1]);
  }
  goto next;
}

int main(int argc, char** argv) {
if(0){
  alf("3d.4d.+. 44444d. 1111111d. + . 3 3=. 4 3=. 1 0|. 7 3|. 1 0&. 1 3&.", 0, 0, 0);
  alf(":A666;333Ad.+.", 0, 0, 0);
  alf(":C[p0];:B[.p0.p1.p2.p3.(999 222)C];(777 888)666(11 22 33)(44 55 66 77)B.", 0, 0, 0);
  alf("666 'A . 'a. ' .", 0, 0, 0);
  alf("\"Foo=\" 42. '\"e", 0, 0, 0);
  alf(":P...; 11 22 33 'P x", 0, 0, 0);
}

  //alf("1.{2.}3.", 0, 0, 0);
//alf("1d.?{2d.}{3d.}.4.", 0, 0, 0);
//  alf("0d.?{2d.}{3d.}.4.", 0, 0, 0);

  alf("1.{2.1 3<?}3.", 0, 0, 0);
  return 0;
}
