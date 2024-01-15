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
#endif // EDEBUG

#include "mem.c"

// Variable Name Bindings on Stack
#include "nantypes.c"

#include "obj.c"


// total tokens processed
long nn=0;

#define Z goto next; case

// (Map [\vkor ..] '(1 2 3) r-value)
// TODO: [] and {} iterator obj
//   TODO: do numbers first?
//   TOOD: generalize print?
// [\vkor 1]'(1 2 3)M
// callback:
//  \v                       value
//  \kv                  key value
//  \rkv             ref key value
//  \alkv        all ref key value
//  \ralkv  rest all ref key value

// opt:
//   'noresult
//   'foldr
//   'foldl
//   'filter
//   'tree
//   'flatten
//   'bottomup
//   'exist
//   'every
//   'none
char* al(char*o,char*p,D*A,int n,D*E); // FORWARD

D map(D f, D l, D all, D r, D opt, int k) { if (deq(l, nil)) return nil;
  //P("Map"); dprint(f); dprint(l); dprint(k); P("\n");
  // hard control params all opt!
  if (iscons(l)) { D e,*z= S; {U=r;U=all;U=l;U=k;U=car(l);
      // A,n for access to var?
      // TODO: how about closure?
      char* x= dchars(f);
      //P("\tf=%s\n", x);
      al(x,x,0,0,0);e=POP;
    } S= z;
    // TODO: not recurse... stack
    // TODO: need setcdr...
    return cons(e, map(f, cdr(l), all, r, opt, k+1)); } return nil; }

char* alf(char*p,D*A,int n,D*E,int iff){assert(!"not ALF it's AL!\n");} // DEBUG

char* al(char*o,char*p,D*A,int n,D*E){ long x; char*e=0,*s,c,*X=0; D d;
//  8 bytes fib 35 => 5.76s
// 10 bytes fib 35 => 6.19s
// 16 bytes fib 35 => 4.37s
// 20 bytes fib 35 => 4.37s
// 26 bytes fib 35 => 4.77s
// 32 bytes fib 35 => 4.47s
char a[16];*a=0;if(!p)return 0; int iff=0; // TODO: remove ALF

// Temp vars: L x ; *e,*s,c ; D d
// (a is argument names string)

// Make sure a is free:d
//#define RET(r) do{free(a);return r;}while(0)
#define RET(r) return r

DEBUG(P("\n===AL >>>%s<<<\n", p))
next: DEBUG(prstack();P("\t>%.10s ...\n",p));
x=0;nn++;switch(c=*p++){case 0:case')':case']':RET(p);Z' ':Z'\n':Z'\t':Z'\r':

Z'"': U= readstr(&p, '"');
Z'#': *S=!istyped(*S);
Z'$': *S=isstr(*S);
Z'\'': if(*p==' ') { p++; char*s= dchars(POP); U=s?reader(&s):error; }
  else U=reader(&p);
Z'?': c=*p++;while(*p&&*p!=c)putchar(*p++); p++;

// -- JUMPS (forward only!) // JMP: +1..
Z (129)...(255):p+=c-128; Z 128:p=o;spc(&p);if(*p=='\\')while(*p&&!isspace(*p))
  p++; for(int i=0;i<n;i++)A[i+1]=S[-n+i+1]; S-=n; p++;  // tail rec

// -- object functions
Z';': U=obj(); Z':': S-=2;*S=get(*S,atomize(S[1]));set(*S,S[1],S[2]);
Z',': S--;set(*S,dlen(*S),S[1]); Z'.': S--;*S=get(*S,atomize(S[1]));
//Z'!': S-=3;set(S[3],S[2],S[1]);

Z'B': while(iscons(*S)) {if(deq(S[-1],car(*S))){S--;*S=S[1];goto next;}
  *S=cdr(*S);}  S--;*S=nil; Z'E': e=dchars(POP);al(e,e,A,n,E);// memBer Eval
Z'A':*S=car(*S); Z'C':S--;*S=cons(S[0],S[1]); Z'D':*S=cdr(*S); // cAr Cons CDr
//Z'F': //  ( Format Function )
Z'G': while(iscons(*S)&&!deq(car(car(*S)),S[-1]))*S=cdr(*S); S--;*S=S[1];// (G)assoc
//Z'H': // (H)append */
//Z'J': // ?? prog1 - ? Jump?
Z'I': if(POP)p++; Z'K': *S=iscons(*S); Z'R': al(o,o,S,0,0); // If Konsp Recurse
Z'L': x=POP;d=nil; while(x-->0)d=cons(*S--,d); U=d; // List (<n items>... n -- L)
Z'M':S--;d=map(*S,S[1],S[1],nil,nil,0);U=d; Z'^':S[-n]=*S;S-=n;RET(p-1);//retMap
Z'N': S--;while(S[0]-- >0)S[1]=cdr(S[1]); *S=car(S[1]);
Z'O': x=0;while(iscons(*S)&&++x)*S=cdr(*S); *S=x; // --- Ordinal + lengthO or just fOld?
Z'P': pc('\n');dprint(POP); Z'T':pc('\n'); Z'W':dprint(POP); // Print Terpri Pc

//Z'Q': // eQual
//Z'S': // Setq
//Z'V': // reVerse
// prin1 (Y)read null?
Z'X': P("%s",e=sdprinq(0,POP));free(e); Z'Y': {e=0;s=0;size_t z=0; if(getline(
  &e,&z,stdin)>=0)p=e;U=reader(&p);free(e);} Z'U':*S=deq(*S,nil)||deq(*s,undef);

//Z'Z': // Zapply
//Z'[': // quotation:
// `backquote
// - means application f`3
// - `foo${b}ar` = JS-templates
// - common lisp macros 
// - `(progn (setq ,v1 ,e) (setq ,v2 ,e))
//   ^ backquote   ^   ^  insert here    
// APPLY: f $ g $ h == f`g`h 
//   how about h@ g@ f@ in AL?
// 
//Z'`' back? => addr?

// faster here than at the beginning, LOL
// inline makes 5.04s instead of 5.70s for fib-l.al...
Z'0'...'9':{D v=0;p--;while(isdigit(*p))v=v*10+*p++-'0';U=v;}

// -- lambda functions and parameters
// ( \lambda reverse debruijn index
// and use letters instead a..z )

// named (1) is 25% slower! fib 35
// TODO: if call al() for loop?
//   it'll do \x wrong... works
//   because jump!
Z'\\': if(*p>='a')strcpy(a,parsename(&p));n=*a?strlen(a):n+1;A=S-n; // \ambda
Z'a'...'z':if(*a)U=A[strchr(a,c)-a+1];else U=A[c-'a'+1]; // local var

// -- Math operators
Z'%': S--;*S=L*S%L S[1]; Z'~': *S=!*S; // % ~not
#define OP(op,e) Z #op[0]: S--;*S=*S op##e S[1];
OP(+,);OP(-,);OP(*,);OP(/,);OP(<,);OP(>,);OP(=,=);OP(|,|);OP(&,&);


////////////////////////////////////////
// ALF (legacy) TODO: use include?

// :::BEGIN ALF

// No faster if not included...?
#ifndef NOALF // DEBUG
  #define ALF NOALF // DEBUG
#endif // DEBUG
#ifdef ALF // DEBUG
Z'F': switch(c=*p++){

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
Z'0'...'9': p--;U=reader(&p);
Z'~': S--;*S=deq(*S,S[1]);
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
Z'}': RET(iff?p:0); Z'{': while(!((e=alf(p, A, n, E, 0)))){}; p= e;
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
}
#endif
/// ::: END ALF

default: error: P("\n[%% Undefined op: '%s']\n", p-1);p++;} goto next;
}

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

void pal(char* p) {
  while(p&&*p)
    if (*p==128) P("[TailRecurse]"),p++;
    else if (*p>128) P("[+%d]",*p++-128);
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
char* _opt(char*,char); // FORWARD
char* _sopt=0; void opt(char* p) { _sopt= p; _opt(p, 0); }

char* _opt(char* p, char e) {
  while(p&&*p) { char c;
    //P("OPT:");pal(p);P("\n");
    if (!*p || *p==e) return p;
    switch(c=*p){
    case')':case'}':case']':
      P("\n%%ERROR: opt expected '%c' (%d) at\n%.*s <--HERE---> %s\n", e, e, (int)(p-_sopt), _sopt, p); abort();
    case'\'': p++;
    case'(': case'[':
      if (isalnum(*p)) { parsename(&p); break; }
      if (*p=='('||*p=='{'||*p=='[') p= _opt(p+1, *p=='('?')':*p=='{'?'}':']'); break;
    case'"': p++;while(*p&&*p!='"')p++; break;
    case'R': if (p[1]=='^') { *p= 128; } break;// tail rec
    case'I': case '{': {
      if (*p=='I')p++;
      if (*p>=128) break; // idempotent
      char*e=_opt(p+1, '}');
      assert(*e=='}');
      *e= ' ';
      // => 128...== +1...
      int x=e-p+(c=='I');
      assert(x);
      assert(128+x<=255);
      *p=128+x;
      p=e;
    }break;
    }

    p++;
  }
  return p?p-1:0;
}

// ENDWCOUNT

// Missing:
// - let let*
// - prog prog* prog1 prog2 progn
//    (no need, just need drop!)
// - fold reduce remove reVerse
// - max min sqrt
// - funcall == apply ?

// how to define variables?

// list of functions
// - https://homepage.divms.uiowa.edu/~luke/xls/tutorial/techreport/node87.html

/*
  
  & - bit32 and
  | - bit32 or
  () - implicitly quoted list
  
  Vd - new local var (O) (???)
  
  OR just remap # to N and
  use same as ALF...
  (these are too funny)
  ; - new Object (- O)
  . - get property (O 'foo .)
  : - put property (O 'foo 3 : - O)
  , - push (O 3 ; 4 ; - O) [3,4])
  F\ - TODO: how to drop ?
  ?"foo" - print foo

   _foo

  !"#$%&'()*+,-./0123456789:;<=>?
       & ()  , .           :;   

  @ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_
         GH J  MN    S  V  YZ[ ] _

  `abcdefghijklmnopqrstuvwxyz{|}~
  `                           |

  ' '
   ! - Forth ! (write mem)
   " - string
   # - numberp
   $ - stringp
   % - mod
   & - and TODO: short-circuit
   '
   ( --
   ) --
   * - mul
   + - plus
   , --
   - - minus
   . --
   / - div
   0-9 - number
   : -- define/defun
   ; --
   < - lt
   = - eq
   > - gt
   ? - not eq nil (see U)
   @ - Forth @ (read mem)
   A - cAr
   B - memB (use eq)
   C - Cons
   D - cDr
   E - Eval
   F - Forth prefix
   G -- (G)assoc
   H -- (H)append
   I - If
   J -- (prog1/progn)
   K - Konsp
   L - n List (<n items> n -- L)
   M -- Mapcar
   N -- Nth
   O - Ordinal (length) 0[1+]Fold
   P - Print (\n val)
   Q -- eQual
   R - Recurse
   S -- Setq Sa-Sz SA=SetcAr;SD
     3       `a !  == set! a
     3 4C    ` @   == car
     3 4C    ` 1+@ == cdr
     7 3 4C  ` !   == set! car
     7 3 4C  ` 1+! == set! cdr
     3       Sa    == Setq a
     7 3 4C  SA    == SetcAr
     7 3 4C  SD    == SetcDr
   T - Terpri
   U - Undefined? / nUll
   V -- reVerse / Var
   W - princ unquoted
   X - prin1 quoted""
   Y -- (Y)read
   Z -- (Z)apply
   [ -- closure (?)
       (means no need to "end")
   \ - \ambda "\\\ bca" or \xyz zyx
   ] -- end closure/interpretation
   ^ - return
   _ -- long name call (== 'foo @ E)

   `a-`z -  -stack ref for ! and @
  "` " - address (of stack item)
   a-z - var/stack ref how frame?
   { - jump to '}' 1I{3}{4}=>3
   | - and TODO: short-circuit
   } - destination from '{'
   ~ -- NOT how C: ~
 DEL -- not visible...

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

  p= strdup("666666 42 1 I{111}{222}F.F.F\\\n");
  pal(p);
  opt(p);
  pal(p);
  opt(p);
  pal(p);
  al(p,p,0,0,0); pc('\n');
  
  p= strdup("666666 42 0 I{111}{222}F.F.F\\\n");
  pal(p);
  opt(p);
  pal(p);
  opt(p);
  pal(p);
  al(p,p,0,0,0); pc('\n');



  P("\n---REPL:\n");
  
  
  // read-eval
  char* ln= NULL; size_t sz= 0;
  while(getline(&ln, &sz, stdin)>=0) {
    P("\nAL > "); pal(ln);
    P("OPT> "); opt(ln); pal(ln);
    al(ln,ln,0,0,0);
    DEBUG(printf("\t[%% %ld ops]\n", nn); nn=0);
    if (S<=K) { P("\n%%STACK underflow %ld\n", S-K); } // DEBUG
    if (S>=K+SMAX) { P("\n%%STACK overrun\n"); } // DEBUG
    if (!deq(K[SMAX-1],error)) { P("\n%%STACK corrupted/overrun to cons/var storage\n"); } // DEBUG
  }
}
#endif
