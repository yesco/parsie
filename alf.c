//80----------------------------------------------------------------------------
// ALphabetical Forth byte code

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

// enable to trace memory allocations
//#include "malloc-trace.c"

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

////////////////////////////////////////////////////////////////////////////////
//D lxfind2(D a){D*s=S,f=atom("__"),n=0;char*x=dchars(a); while(--s>K+2) if(isatom(
//  *s)&&(n+=deq(*s,f)?100-L n%100:1)&&dchars(*s)==x)return-n;return atomaddr(a);}

D lxfind4(D q){ // DEBUG
  D*s=S,a=atomaddr(q),n=0,l=0; // DEBUG
  char*x=dchars(q); // DEBUG
  while(--s>K+2 && a>=0) if (isatom(*s)) { // DEBUG
      if (deq(*s,__)){ if (l) a=-n-l;else n+=100;} // DEBUG
      if (l || dchars(*s)==x) l++; // DEBUG
    } // DEBUG
  return a; // DEBUG
} // DEBUG

// Find lexical stack frame reference
// -100 per new stack frame
// -3 variable 3 steps above __
// see alf-lxfind.tst
D lxfind(D q){D*s=S,a=atomaddr(q),n=0,l=0;char*x=dchars(q);while(--s>K+2&&a>=0)
 !isatom(*s)?0:deq(*s,__)?l?a=-n-l:(n+=100):l||dchars(*s)==x?l++:0;return a;}

#define Z goto next; case

// run ALF code Program with ARGS
// starting that stack position with
// N items, IFF=1 if running ?{}{}
//
// Returns next position to parse from
//   NULL if done/fail (loop/if)
char* alf(char*p,D*A,int n,int iff){long x;char*e=0,*s,c;D d;if(!p)return 0;
DEBUG(P("\n===ALF >>>%s<<<\n", p))
next: DEBUG(prstack();P("\t>%.10s ...\n",p));
x=0;nn++;switch(*p++){case 0:case';':case')':case']':return p;Z' ':Z'\n':Z'\t':Z'\r':
// - stack
Z'd':S[1]=*S;S++;Z'\\':S--;Z'o':S[1]=S[-1];S++;Z's':{D d=*S;*S=S[-1];S[-1]=d;}
// - numbers / functions call/define
Z'A'...'Z': alf(F[p[-1]-'A'],0,0,0); Z'^': A[1]= *S; S=A+1; return p-1;
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
// -- cX - char | string
Z'c': s=0;switch(c=*p++){ Z'r':pc('\n'); Z'c':*S=dlen(*S); Z'=':S--;*S=dcmp(*S, S[1]);
  Z'"':case'\'':x=p[-1];e=p;while(*p&&*p!=x)p++;U=newstr(e,p++-e);
  // c?=charclass c1=first char
  // Z'?':case'1':x=*S;if(isnan(*S)){e=dchars(*S);x=e?*e:0;};if(c=='1'){U=x;break;}
  //*S= isalpha(x)?'a':isdigit(x)?'d':isspace(x)?'s':x=='_'?'_':'o';

  // TODO: need to free e???

  /// > ./alf -d <alf-printers.tst

  // but for ca% ???? errro
  //#define SP(P) do{*S=newstr(e=P,-1);}while(0)

  #define SP(P) do{*S=newstr(e=P,-1);free(e);}while(0)

  // TODO: buffer "type" to not create things for the GC?
  Z'a':{d=POP;s=sncat(0,dchars(POP),-1);U=d;c=*p++;case'e':case's':case'q':case'%':switch(c){
    Z'b':{char a[]={' ',0};SP(sncat(s,a,-1));} // TODO: use c fix sncat
    Z'e':{char a[]={*S,0};SP(sncat(s,a,-1));} // TODO: use c fix sncat
    Z'q':SP(sdprinq(s,*S)); Z'%':{e=--p;while(*p&&!isspace(*p))p++;
	char* f=strndup(e,p-e);SP(sdprintf(s,f,*S));free(f);}
    case's':p++;default:p--;SP(sdprinc(s,*S));}}}
      //    goto next;default:p-=2;goto error;}}}

// -- printing
Z'.': dprint(POP);pc(' '); Z'e':pc(POP); Z't':P("%*s.",(int)*S,M+L S[-1]);S-=2;
Z'p': dprint(POP); Z'\'': U= *p++; Z'"': while(*p&&*p!='"')pc(*p++); p++;
// -- hash/atoms/dict
Z'#': switch(*p++){ Z'a'...'z':case'A'...'Z':case'_':p--;U=atom(parsename(&p));
  Z',': S--;set(*S,dlen(*S),S[1]); Z':': S-=2;set(*S,S[1],S[2]);
  Z'@': S--;*S=get(S[1],*S); Z'!': S-=3;set(S[3],S[2],S[1]);
  Z' ': case'\n': case 0: if ((e=dchars(*S))) *S=atom(e);
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
   *S=atomaddr(*S);break;default:goto error;}
// string/stack/misc functions
////////////////////////////////////////////////////////////////////////////////
Z'$': x=1; switch(c=*p++){ Z'.':prstack(); case'n':pc('\n'); Z'd':x=S-K;U=x;
  Z'$':n=POP;A=S-n; Z'0'...'9':U=A[p[-1]-'0']; Z'h':P("%lx\n",L POP);Z'q':S=1+K;
  Z'!':A[*p++-'0']=POP; Z's':x=POP;case' ':while(x-->=0)pc(' ');
  Z'"':case'\'':e=H;while(*p&&*p!=c)*H++=*p++;*H++=0;if(*p)p++;U=e-M;U=H-e-1;
  Z'D':dump(); Z'?':*S=lxfind(*S); Z'K': prK(); goto next; default:p--;}/*err*/
default: error: P("\n[%% Undefined op: '%s']\n", p-1);p++;} goto next;
}

// Simple peep-hole optimization
//
// Removes extra spaces-fib 19-27% faster!
// CAVEAT: generally not a good idea...
// 
// TODO: doesn't rem spc in >c"fooo" 35<
// TODO: `1@ == $1
// TODO: #foo => base 128 number
// TOOD: 'f ... might get ignored till '?
// 
char* opt(char* p) { char *s= p; while(s&&s[0]&&s[1]&&s[2]){switch(s[0]){
  case'$':s++;break;
  case'"':while(*s&&*++s!='"'){}break;case'#':case')':s++;parsename(&s);break;
  case'c': if (s[1]=='a')s++; if(s[1]=='%'){while(*s&&!isspace(*s))s++;break;} if(isspace(s[1]))s++; break;
  case'`': break; case'0'...'9': if(isdigit(s[2]))break;
  default: if(!isspace(s[1])) break; memmove(s+1, s+2, strlen(s+2)+1);continue;
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
    # - atomify string (ret other)

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

    $? = find lex var on stack frame
    
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
    cr - carriage return

    c" - managed null-terminated string
    c' -    same
  ( c\" - read quoted )
  ( c\' -   same )
    cc - count=length of mngd/"ptr"/#atom
         TODO: name co?

    // TODO: make chainable
    ( cae == ce ca )
    ce - char emit to string
    cq - quoted
    cs - stringify to managed string
    c%1s - snprintf stringify mngd str
         ( C%8.g C%1s )
    (- same in append mode )
  ( ca - ambigious TODO:? )
    cae - append char
    caq - append quoted
    cas - append stringified
    c%1s - append snprintf...
    
  ( cm - cmove ? TODO: )
  ( c= - compare strings/all -1 0 +1 )

    ( -- Char type operators - TODO: )
  ( c! - )
  ( c@ - )
  ( c, - )

  ( - code written but 'excluded' - )
  ( cp = get pointer to string ? )
  ( c# - get nth char? (S N - C) )
  ( c1 - ORD: get first char f str/chr )
  ( c? - charclass _ a=alhpa d=digit o=other s=spc )
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

#ifdef alfTEST

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
    alf(opt(ln), 0, 0, 0);
    DEBUG(printf("\t[%% %ld ops]\n", nn); nn=0);
    if (S<=K) { P("\n%%STACK underflow %ld\n", S-K); } // DEBUG
    if (S>=K+SMAX) { P("\n%%STACK overrun\n"); } // DEBUG
    if (!deq(K[SMAX-1],error)) { P("\n%%STACK corrupted/overrun to cons/var storage\n"); } // DEBUG
  }
}
#endif
