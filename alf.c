//80----------------------------------------------------------------------------
// ALphabetical Forth byte code

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef DEBUG
int debug= 0; // DEBUG
#define DEBUG(D) if (debug) do{D;}while(0);
#endif

#include "mem.c"

// Variable Name Bindings on Stack
#include "nantypes.c"

#include "obj.c"

// Parse from P a name
//
// (P is pointer to char* and updated)
//
// Note: this allows any alnum || _
//
// Returns a static string (use fast!)
char* parsename(char** p) { static char s[64], i; i=0; while((isalnum(**p)
  ||**p=='_') && i<sizeof(s)-1) s[i++]=*(*p)++; s[i]= 0; return s;}

// skips a { block } returns after
//   TODO: must skip STRINGs!!! lol
char* skip(char* p){ int n=1;while(n&&*p)if(*p=='?'&&p[1]!='{')p+=2;else n+=(*p=='{')-(*p=='}'),p++;return p;}

void prstack(){P("\t:");for(D* s= K+2; s<=S; s++){dprint(*s);pc(' ');}} // DEBUG

// total tokens processed
long nn=0;

#define Z goto next; case

// run ALF code Program with ARGS
// starting that stack position with
// N items, IFF=1 if running ?{}{}
//
// Returns next position to parse from
//   NULL if done/fail (loop/if)
char* alf(char*p, D* A,int n,int iff) {long x,c;char*e=NULL,*d;if(!p)return 0;
DEBUG(P("\n===ALF >>>%s<<<\n", p))
next: DEBUG(prstack();pc('\n');P("\t  '%c'\n",*p))
x=0;nn++;switch(*p++){case 0:case';':case')':case']':return p;Z' ':Z'\n':Z'\t':Z'\r':
// - stack
Z'd':S[1]=*S;S++;Z'\\':S--;Z'o':S[1]=S[-1];S++;Z's':{D d=*S;*S=S[-1];S[-1]=d;}
// - numbers / functions call/define
Z'A'...'Z': alf(F[p[-1]-'A'],S,0,0);
Z':': e=strchr(p,';'); if(e) F[*p-'A']=strndup(p+1,e-p),p=e+1;
Z'x':{D d=POP;char x[]={d,0};alf(TYP(d)==TSTR?dchars(d):x,A,n,0);}
Z'[': { e=p; while(*p&&*p!=']')p++;U=newstr(e, p++-e);
  {D f=POP; D c=obj(); Obj* o= PTR(TOBJ, c); D* pars=&(o->np[0].name);
    //o->proto=OCLOS;
    // Can store 12 values/frame!
    // TODO: copy from current frame
    // For each frame UP
    // - coppy to new obj()
    // - change frameptr stack values
    // - chain up each frame as next
    // (not using proto)
    // alt: use actual names in closure!
  }
 }
// TODO: error if no method?
Z'(':{x=S-K;S[1]=A-S;S++;D*s=S;p=alf(p,A,n,1);D n=atom(parsename(&p));
  e=dchars(deq(n,nil)?POP:get(s[1],n)); U=x; alf(e,s+1,S-s,0); *s=*S;S=s;}
  //DEBUG(P("\n\tCALL: o="); dprint(s[1]); P(" nom="); dprint(nom); P(" m="); dprint(m); pc('\n'););
// -- numbers / math
Z'0'...'9':{D v=0;p--;while(isdigit(*p))v=v*10+*p++-'0';U=v;}
Z'~': S--;*S=deq(*S,S[1]); Z'%': S--;*S=L*S%L S[1]; Z'z': *S=!*S;Z'n':*S=-L*S;
#define OP(op,e) Z #op[0]: S--;*S=*S op##e S[1];
OP(+,);OP(-,);OP(*,);OP(/,);OP(<,);OP(>,);OP(=,=);OP(|,|);OP(&,&);
// -- memory
Z'h': U=H-M; Z'm':x=*S;*S=H-M;H+=x; Z'a':H+=L POP;
Z'g': case ',': align(); if (p[-1]=='g') goto next; memcpy(H,&POP,SL); H+=SL;
Z'@': *S= *(D*)m(*S,A,n); Z'!': *(D*)m(*S,A,n)= S[-1]; S-=2;
// -- cC - char | string
Z'c': switch(c=*p++){ Z'r':pc('\n'); Z'c':*S=dlen(*S); Z'=':S--;*S=dcmp(*S, S[1]);
  Z'"':case'\'':x=p[-1];e=p;while(*p&&*p!=x)p++;U=newstr(e,p++-e);
  // Z'?':case'1':x=*S;if(isnan(*S)){e=dchars(*S);x=e?*e:0;};if(c=='1'){U=x;break;}
  //*S= isalpha(x)?'a':isdigit(x)?'d':isspace(x)?'s':x=='_'?'_':'o';
}
// -- printing
Z'.': dprint(POP);pc(' '); Z'e':pc(POP); Z'p': dprint(POP);
Z't': P("%*s.",(int)*S,M+L S[-1]);S-=2;
Z'\'': U= *p++; Z'"': while(*p&&*p!='"')pc(*p++); p++;
Z'^': A[1]= *S; S=A+1; return p-1;
// -- hash/atoms/dict
Z'#': switch(*p++){ Z'a'...'z':case'A'...'Z':case'_':p--;U=atom(parsename(&p));
  Z',': S--;set(*S,dlen(*S),S[1]); Z':': S-=2;set(*S,S[1],S[2]);
  Z'@': S--;*S=get(S[1],*S); Z'!': S-=3;set(S[3],S[2],S[1]);
  Z'?': *S=typ(*S); Z'^': U=obj(); goto next; default: goto error; }
// -- loop/if
Z'}': return iff?p:NULL; Z'{': while(!((e=alf(p, A, n, 0)))){}; p= e;
Z'?': if (POP) { switch(*p++){ Z'}': return p; Z']': return 0;
  Z'{': p=alf(p,A,n,1);if(*p=='{')p=skip(p+1); default:p=alf(p,A,n,1);
 }} else { // IF==false
  if (*p=='{') { p=skip(p+1); } if (p) p= alf(p+1, A, n, 1); else return 0; }
// -- bitops
#define LOP(op,e) Z#op[0]: S--; *S=(L S[1]) op##e L *S;
Z'b': switch(*p++){ LOP(&,);LOP(|,);LOP(^,); Z'~': *S= ~L *S; }
// -- ` address of stuff
Z'`': switch(*p++) { Z'#': U=n; Z'0'...'9': U='0'-p[-1]-1; Z'A'...'Z':
 case'a'...'z':case'_':p--;U=atom(parsename(&p)); case' ':case 0:case'\n':
   *S=1+2*(DAT(*S)>>32);break;default:goto error;}
// string/stack/misc functions
Z'$': x=1;switch(c=*p++){ Z'.': prstack(); case'n': pc('\n');
  // TODO: why 3?
  Z'0'...'9': U=A[p[-1]-'0'];
  Z'$': n=POP; A-=n; Z'd': x=S-K; U=x;
  Z'!': A[*p++-'0']=POP; Z's':x=POP;case' ':while(x-->=0)pc(' ');
  Z'"':case'\'':e=H;while(*p&&*p!=c)*H++=*p++;*H++=0;if(*p)p++;U=e-M;U=H-e-1;
  Z'h': P("%lx\n", L POP); Z'q': S=1+K;
  Z'D':{ char* M=(void*)C; // DEBUG TODO: M?
  for(int i=0; i<*S;) { int n= S[-1]; P("\n%04x ", n); // DEBUG
    for(int j=0; j<8; j++) P("%02x%*s", M[n+j], j==3, "");  P("  "); // DEBUG
    for(int j=0; j<8; j++) P("%c", M[n+j]?(M[n+j]<32||M[n+j]>126? '?': M[n+j]):'.'); // DEBUG
    D d= *(D*)(M+n); x=TYP(d); if ((x&0x0ff8)==0x0ff8) { // DEBUG
      if (x>32*1024) x=-(x&7); else x= x&7; } else x=0; // DEBUG
    P(" %2ld:", x);dprint(d);S[-1]+=8;i+=8;}P("\n");prstack();break;} // DEBUG
  Z'K':{ char*e=0; size_t z=0; long n=0, a=0; // DEBUG
    do { int p=0; // DEBUG
      for(int p=0; p<32;) { // DEBUG
	D d= K[a]; // DEBUG
	if (++a >= KSZ) goto next; // DEBUG
	if (d!=0 || K[a]) n=0; else if (n>3) { a++; continue; }// DEBUG
	p++; // DEBUG
	if (a%2==1) printf("\n%5ld : ", a); // DEBUG
	x=TYP(d); if ((x&0x0ff8)==0x0ff8) { // DEBUG
	  if (x>32*1024) x=-(x&7); else x= x&7; } else x=0; // DEBUG
	P(" %2ld:", x); // DEBUG
	printf("%*s", 10-dprint(d), " "); // DEBUG
	if (d==0 && ++n==3) pc('\n'); // DEBUG
      }
      fputc('>', stderr); // DEBUG
    } while(getline(&e,&z,stdin)); } // DEBUG
  goto next; default: p--;} /* err */
default: error: P("\n[%% Undefined op: '%s']\n", p-1);p++;} goto next;
}

// Simple peep-hole optimization
//
// Removes extra spaces-fib 19-27% faster!
// 
// TODO: doesn't rem spc in >c"fooo" 35<
// TODO: `1@ == $1
// TODO: #foo => base 128 number
char* opt(char* p) { char *s= p; while(s&&s[0]&&s[1]&&s[2]){switch(s[0]){
  case'"':while(*s&&*++s!='"'){}break;case'#':case')':s++;parsename(&s);break;
  case'`': break; case'0'...'9': if(isdigit(s[2]))break;
  default: if (!isspace(s[1])) break; memmove(s+1, s+2, strlen(s+2)+1);continue;
  } s++; } DEBUG(P("\n%s\n", p)); return p; }

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
  # - atom/obj (hash/dict) stuff
    #abc - make atom (alnum)

    #? - type? -> i)nt f)float n)nan N)il A)tom S)tring O)bj C)ons U)ndef
         lowercase ( >- 'A' is  "num" )

    #@ - obj: get prop value (N O - V)
    #! - obj: add value,name (V N O -)
    #^ - obj: new (- O)
    (obj 11 22)foo - call obj.foo(11, 22)
      obj becomes $0 $1==1 $2==2 use ^=ret
  ( #.bar - get o.bar property (- x) )
  ( #=bar - set o.bar property (x -) )

    (- builder funcs/reverse order)
    #: - obj: set Value Name (O N V - O)
    #, - append to array (O V - O)
  ( #< - push on array/obj )
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

  $ - rgs/string functions/aux funs
    (- function args and return)
    $$ - set number of args (legacy)
    $# - number of args
    $1 - $9 - get frame arg N (see `1)
    $!1 - $!9 - set frame arg N
    ( see ^ for return )
    
   '$ '- print space
    $s - print N spaces

    $h - print hex
    $. - print stack (.S normally)
    $d - depth of stack

  ( $p - P? )
    $D - dump from address / DEBUG
    $K - dump K mem (stack + globals + obj)
  ( $F - format number? tra forth? )
    $q - quit/reset stacks


    $" - counted mem string
  ( $c - copy )
  ( $m - move )
  ( $\" - ... " parse quoted )
  ( $sC ... C - parse using C as delim )
  ( $( - counted string) )
  ( $[ - another?] )
  ( $l - strlen - see cc ? )

  % - mod
  & - and
  ' - char
  ( - param start for object method call
  )nm - look up method 'nm' and call
  )   - use last valuie as method & call
  * - mul
  + - add
  , - ret aligned here, write word
  - - minus
  . - print data w space (see . e t p q)
  / - div
  0123456789 - number
  : - define
  ; - end define
  < - number (?) less than
  = - number equal (?) see ~ for eq!
  > - number (?) greater than
  ? - if
    ?{ - IF-stmt 1?{then}{else}
    ?] - if true exit {loop} = break
    ?} - if true begin again = cont
    ??? TODO: label/goto/continue/break? )
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
    ` - address of atom global var
    `foo - address of global var 'foo'
    
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
  ( c= - compare strings/all -1 0 +1 )

    ( -- Char type operators - TODO: )
  ( c! - )
  ( c@ - )
  ( c, - )

  ( - code written but 'excluded' - )
  ( cp = get pointer to string )
  ( c# - get nth char? (S N - C) )
  ( c1 - ORD: get first char f str/chr )
  ( c? - _ a=alhpa d=digit o=other s=spc )
         ( CHAR c? 'e' < === alphanum||_ )
  ( cu - toupper char/str )
  ( cl - tolower char/str )
    
    ci - c++
    cd - c--

    c+ - c+=T
    c-
    c*
    c/
    c< - c<<=T -- TODO: c{ update skip
    c> - c>>=T -- TODO: c} update skip

  ???
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
  ~ - identity eq === (bitpattern)
( <del> )
( 128-255 - optimized atomnames/addr )
*/

#ifndef alfNOMAIN

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
    P("1 %s\n", skip("foo}OK"));
    P("2 %s\n", skip("f?]oo}OK"));
    P("3 %s\n", skip("f?}oo}OK"));
    P("4 %s\n", skip("f?{oo}OK"));
    P("5 %s\n", skip("f{o}o}OK"));
    P("6 %s\n", skip("1}{OK}"));
    exit(3);
  }

  // read-eval
  char* ln= NULL; size_t sz= 0;
  while(getline(&ln, &sz, stdin)>=0) {
    alf(opt(ln), S, 0, 0);
    printf(" [%% %ld ops]\n", nn); nn=0;
    if (S<=K) { P("\n%%STACK underflow %ld\n", S-K); } // DEBUG
    if (S>=K+SMAX) { P("\n%%STACK overrun\n"); } // DEBUG
    if (!deq(K[SMAX-1],error)) { P("\n%%STACK corrupted/overrun to cons/var storage\n"); } // DEBUG
  }
}
#endif
