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

#define Z goto next; case

char* alf(char*p,D*A,int n,D*E,int iff){assert(!"not ALF it's AL!\n");}

char* al(char*o,char*p,D*A,int n,D*E,int iff){long x;char*e=0,*s,c;D d;if(!p)return 0;
DEBUG(P("\n===AL >>>%s<<<\n", p))
next: DEBUG(prstack();P("\t>%.10s ...\n",p));
x=0;nn++;switch(c=*p++){case 0:case';':case')':case']':return p;Z' ':Z'\n':Z'\t':Z'\r':

case 'l': switch(c=*p++){
  Z'"': ; // string
  Z'#': *S=!istyped(*S);
  Z'$': *S=isstr(*S);
  Z'%': ; // mod
  Z'&': ; //  &and
  Z'\'':U=atom(parsename(&p)); // 'quote
  Z'(': ; // (list // TODO: '(a b) ...
  Z')': ; // )
  Z'*': ; // *mul
  Z'+': ; // +plus
  Z',': ; // ??? , (cons for list, or just space?)
  Z'-': ; // -minus
  Z'.': *S=isatom(*S); // .atom?
  Z'/': ; // /div
  //Z'0-9number
  Z':': ; // :define
  Z';': ; // ; end
  Z'<': ; // <less
  Z'=': ; // =eq
  Z'>': ; // >gt
  Z'?': *S=!deq(*S,nil); // !null

}

// JUMPS (forward only!)
Z (129)...(255): p+=c-128; // JMP: +1..
Z 128: p=o; // tail rec

Z'^': return p-1;

//Z'@': // @assoc
Z'A': *S=car(*S);
//Z'B': // memBer
Z'C': S--;*S=cons(S[0],S[1]); // Cons
Z'D': *S=cdr(*S);
Z'E': e=dchars(POP);al(e,e,A,n,E,0); // Eval
//  ( Format )
//Z'G': // (G)assoc
//Z'H': // (H)append
Z'I': if(POP)p++; // If (== ?skip)
//Z'J': // ?? prog1 - ? Jump?
// KOND? LOL
//
// a1=I{11 }{ a2=I{22 }{ a3=I{33    }}}
// a1=I{11 K} a2=I{22 K}{a3=I{33 K} ]
//         >---------->---------->--*

// SWITCH w break
//
// a1=I{11 }{a2=I{22[}}}a3=I{]33 [} ]

// SWITCH 11 fallthrough
// 
// switch(a){
// case 1: printf("11 ");
// case 2: printf("22 "); break;
// case 3: printf("33 ");
// default: printf("44 ");
// }

// K = break
//
// a1=I{11.   [ }
//     {a2=I{ ]  22. K [ }
//          {a3=I{     ] 33. [ }
//               {           ] 44. [
//               }
//          }
//     }                           ]

// a1=I{0}{a2=I{1}{a2=I{2}{1n}}}
// [11.][22.K][33.]\0[44.] \
// d0zI{...}1-
//     >-^  -1
// 0    1     2    3

// [ a1=I{J2}{ a2=I{J3}{ a3=I{J4}{ J1}}} J0
//   _1 11. _2 22. J0 _3 33. _1 44. _0]
//
//   J0 = end, J1 = default  [] = nesting

Z'K': *S=iscons(*S); // Konsp
//Z'L': // Length? List? varargs... use C
//Z'N': // Nth
//  Pair?
//Z'Q': // eQual
Z'R': al(o,o,A,n,0,0);
//Z'S': // Setq
Z'T': pc('\n'); // Terpri
Z'U': *S=deq(*S,nil)||deq(*s,undef); // null?
//Z'V': // reVerse
Z'W': dprint(POP); // Write/Princ ?
Z'X': printf("%s", e=sdprinq(0,POP)); free(e); // X/Princ1 ?
//Z'Y': // Yread ?
//Z'Z': // Zapply
//  [
//Z'\\': // \lambda
//  ]
//  ^
//Z'_': // _floor
// ` back?
Z'~': *S=!*S; // ~not
//case'a'...'z': // a-z variables
// {
//Z'|': // |or
// }

Z'@': *S= *(D*)m(*S,A,n); Z'!': *(D*)m(*S,A,n)= S[-1]; S-=2;
// - stack
Z'd':S[1]=*S;S++;Z'\\':S--;Z'o':S[1]=S[-1];S++;Z's':{D d=*S;*S=S[-1];S[-1]=d;}
// - numbers / functions call/define
//Z'A'...'Z': al(F[p[-1]-'A'],0,0,0,0); Z'^': A[1]= *S; S=A+1; return p-1;
Z':': e=strchr(p,';'); if(e) F[*p-'A']=strndup(p+1,e-p),p=e+1;
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
//Z'(':{x=S-K;U=E?E-K:0;D*s=S;p=alf(p,A,n,E,1);D n=atom(parsename(&p));
//    D m=deq(n,nil)?POP:get(s[1],n),f=funf(m),*c=fune(m);
//    P(" [FUN:");dprint(n);P(" '");dprint(m); P("' %ld] ", c?c-K:0);
//    alf(f?dchars(f):F[L m-'A'],s+1,S-s,c,0); *s=*S;S=s;}
  //DEBUG(P("\n\tCALL: o="); dprint(s[1]); P(" nom="); dprint(nom); P(" m="); dprint(m); pc('\n'););
// -- numbers / math
Z'0'...'9':{D v=0;p--;while(isdigit(*p))v=v*10+*p++-'0';U=v;}
//Z'~': S--;*S=deq(*S,S[1]);
Z'%': S--;*S=L*S%L S[1]; Z'z': *S=!*S;Z'n':*S=-L*S;
#define OP(op,e) Z #op[0]: S--;*S=*S op##e S[1];
OP(+,);OP(-,);OP(*,);OP(/,);OP(<,);OP(>,);OP(=,=);OP(|,|);OP(&,&);
// -- memory
Z'h': U=H-M; Z'm':x=*S;*S=H-M;H+=x; Z'a':H+=L POP;
Z'g': case ',': align(); if (p[-1]=='g') goto next; memcpy(H,&POP,SL); H+=SL;
//Z'@': *S= *(D*)m(*S,A,n); Z'!': *(D*)m(*S,A,n)= S[-1]; S-=2;
// -- cX - char | string
/*
Z'c': s=0;switch(c=*p++){ Z'r':pc('\n'); Z'c':*S=dlen(*S); Z'=':S--;*S=dcmp(*S, S[1]);
  Z'"':case'\'':x=p[-1];e=p;while(*p&&*p!=x)p++;U=newstr(e,p++-e);
  // c?=charclass c1=first char
  // Z'?':case'1':x=*S;if(isnan(*S)){e=dchars(*S);x=e?*e:0;};if(c=='1'){U=x;break;}
  // *S= isalpha(x)?'a':isdigit(x)?'d':isspace(x)?'s':x=='_'?'_':'o';

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
*/
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
Z'}': return iff?p:NULL; Z'{': while(!((e=alf(p, A, n, E, 0)))){}; p= e;
// -- bitops
//#define LOP(op,e) Z#op[0]: S--; *S=(L S[1]) op##e L *S;
//Z'b': switch(*p++){ LOP(&,);LOP(|,);LOP(^,); Z'~': *S= ~L *S; }
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
  Z'D':dump(); Z'K': prK(); goto next; default:p--;}/*err*/
default: error: P("\n[%% Undefined op: '%s']\n", p-1);p++;} goto next;
}


void pal(char* p) {
  while(p&&*p)
    if (*p>=128) P("[+%d]",*p++-128);
    else pc(*p++);
}

// Simple peep-hole optimization
// 
// Actually, it's more an "compiler"
//
// 
// This destructively changes the str.
//
// NOTE: don't use result!
//
//   "..." skip
//   {...}   => "[+4]... ""
//at { put jump to } repl w ' '
//   I{T}    => "I[+4]T "
//   I{T}{E} => "I[+4]T [+3]F }"
char* opt(char* p) {
  while(p&&*p) { char c;
    //P("OPT:");pal(p);P("\n");
    switch(c=*p){
    case 0: return p;
    case'"': p++;while(*p&&*p!='"')p++; break;
      // TODO: list?
    case'\'': p++; parsename(&p); break;
    case'}': *p=' ';return p;
    case'I':  case '{': {
      if (*p=='I')p++;
      if (*p>=128) break; // idempotent
      char*e=opt(p+1);
      // => 128...== +1...
      *p=128+e-p+(c=='I');
      p=e;break;}
    }

    p++;
  }
  return p?p-1:0;
}
// ENDWCOUNT

/*
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

#ifdef alTEST

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

  char *p;

  p= strdup("666666 42 1 I{111}{222}..\\\n");
  pal(p);
  opt(p);
  pal(p);
  opt(p);
  pal(p);
  al(p,p,0,0,0,0); pc('\n');
  
  p= strdup("666666 42 0 I{111}{222}..\\\n");
  pal(p);
  opt(p);
  pal(p);
  opt(p);
  pal(p);
  al(p,p,0,0,0,0); pc('\n');



  P("\n---REPL:\n");
  
  
  // read-eval
  char* ln= NULL; size_t sz= 0;
  while(getline(&ln, &sz, stdin)>=0) {
    P("\nAL :"); pal(ln);
    P("OPT:"); opt(ln); pal(ln);
    al(ln,ln,0,0,0,0);
    DEBUG(printf("\t[%% %ld ops]\n", nn); nn=0);
    if (S<=K) { P("\n%%STACK underflow %ld\n", S-K); } // DEBUG
    if (S>=K+SMAX) { P("\n%%STACK overrun\n"); } // DEBUG
    if (!deq(K[SMAX-1],error)) { P("\n%%STACK corrupted/overrun to cons/var storage\n"); } // DEBUG
  }
}
#endif
