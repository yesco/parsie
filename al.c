//80----------------------------------------------------------------------------
// ALphabetical Forth byte code

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

// enable to trace memory allocations
//#include "malloc-trace.c"

#include "mem.c"
#include "nantypes.c"
#include "obj.c"

// total tokens processed
long nn=0;

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

D append(D a, D b) {if(!iscons(a))RET b;D r=nil,c=r;do{D n=cons(car(a),b);
  if(deq(r,nil))r=c=n;else c=setcdr(c,n);}while(iscons((a=cdr(a))));RET r;}

D map(D f, D l, D all, D r, D opt, int k) { if (deq(l, nil)) RET nil;
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
    RET cons(e, map(f, cdr(l), all, r, opt, k+1)); } RET nil; }

char* alf(char*p,D*A,int n,D*E,int iff){assert(!"not ALF it's AL!\n");} // DEBUG

char* al(char*o,char*p,D*A,int n,D*E){ long x;char*e=0,*s,c,*X=0;D d;size_t z;
// Temp vars: L x ; *e,*s,c ; D d
// (a is argument names string)

//  8 bytes fib 35 => 5.76s
// 10 bytes fib 35 => 6.19s
// 16 bytes fib 35 => 4.37s
// 20 bytes fib 35 => 4.37s
// 26 bytes fib 35 => 4.77s
// 32 bytes fib 35 => 4.47s
char a[16];*a=0;if(!p)RET 0; int iff=0; // TODO: remove ALF

#define Z goto next; case

// access local/arguemnt var using c
#define VAR A[(*a)?strchr(a,c)-a+1:c-'a'+1]

DEBUG(P("\n===AL >>>%s<<<\n", p))
next: DEBUG(prstack();P("\t>%.10s ...\n",p));
x=0;nn++;switch(c=*p++){case 0:case')':case']':RET(p);Z' ':Z'\n':Z'\t':Z'\r':

Z'`': switch(*p++) { Z'#': U=n; Z'0'...'9': U='0'-p[-1]-1; Z'A'...'Z':
 case'a'...'z':case'_':p--;U=atom(parsename(&p));
  case'$': T=atomaddr(T);break;default:goto error;}

Z'"':U=readstr(&p,'"'); Z'#':T=!istyped(T); Z'$':T=isstr(T);// "" nup strp
// -- quote
Z'\'': if(*p=='?'&& p++)T=isatom(T); else if(*p=='$'){p++;char*s=dchars(T);
  T=s?reader(&s,0):error; } else U=reader(&p,1);

//Z'?': c=*p++;while(*p&&*p!=c)putchar(*p++); p++;
Z'?':switch(c=*p++){ Z'[':c=']';case'"':case'\'':while(*p&&*p!=c)pc(*p++);p++;
  Z'a'...'z':P(" %c=",c);dprint(VAR);case'_':pc(' ');
    goto next;default:p--;goto error;
}

// -- JUMPS (forward only!) // JMP: +1..
Z (129)...(255):p+=c-128; Z 128:p=o;spc(&p);if(*p=='\\')while(*p&&!isspace(*p))
  p++; for(int i=0;i<n;i++)A[i+1]=S[-n+i+1]; S-=n; p++;  // tail rec

// -- object functions
Z';': U=obj(); Z':': S-=2;set(T,atomize(S1),S[2]);
Z',': S--;set(T,dlen(T),S1); Z'.': S--;T=get(T,atomize(S1));
// -- setq get-val
Z'@': T= *(D*)m(T,A,n); Z'!': *(D*)m(T,A,n)= S[-1]; S-=2;

Z'B':while(iscons(T)) {if(deq(S[-1],car(T))){S--;T=S1;goto next;}
  T=cdr(T);}  S--;T=nil; Z'E': e=dchars(POP);al(e,e,A,n,E);// memBer Eval
Z'A':T=car(T); Z'C':S--;T=cons(T,S1); Z'D':T=cdr(T); // cAr Cons CDr
//Z'F': //  ( Format Function )
Z'G':while(iscons(T)&&!deq(car(car(T)),S[-1]))T=cdr(T); S--;T=S1;//Gassoc
Z'H': S--;T=append(T,S1); // (H)append */
//Z'J': // ?? prog1 - ? Jump?
Z'I': if(POP)p++; Z'K': T=iscons(T); Z'R': al(o,o,S,0,0); // If Konsp Recurse
Z'L': x=POP;d=nil; while(x-->0)d=cons(T--,d); U=d; // List (<n items>... n -- L)
Z'M':S--;d=map(T,S1,S1,nil,nil,0);U=d; Z'^':A++;*A=T;S=A;RET(p-1);//retMap

// TODO:
//Z'N': S--;if(isobj(S1)T=get(S1,T);else{while(T-- >0)S1=cdr(S1); T=car(S1);} // Nth

Z'N': S--;if(isobj(S1))T=get(S1,T);else{while(T-- >0)S1=cdr(S1); T=car(S1);} // Nth
// --- Ordinal + lengthO or just fOld?
Z'O': if(isobj(T))T=dlen(T);else{x=0;while(iscons(T)&&++x)T=cdr(T);T=x;}
Z'P': pc('\n');dprint(POP); Z'T':pc('\n'); Z'W':dprint(POP); // Print Terpri Pc

//Z'Q': // eQual
Z'S': switch(c=*p++){ Z'A':S--;T=setcar(T,S1); Z'D':S--;T=setcdr(T,S1);
Z'a'...'z':VAR=T--; goto next; default: p--; goto error; }

//Z'V': // reVerse
Z'X': P("%s",e=sdprinq(0,POP));free(e); Z'Y':e=0;z=0;if(getline(&e,&z,stdin)
  >=0)s=e,U=reader(&s,0);else U=error;free(e);
Z'U':T=deq(T,nil)||deq(T,undef);

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

// TODO: BUG: if remove value from stack: then will miss it, it's like S isn't A-n at "^" time???
Z'\\': if(*p>='a')strcpy(a,parsename(&p));n=*a?strlen(a):n+1;A=S-n; // \ambda
Z'a'...'z':U=VAR;

// -- Math operators
Z'%': S--;T=L T%L S1; Z'~': T=!T; // % ~not
#define OP(op,e) Z #op[0]: S--;T=T op##e S1;
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

Z'@': T= *(D*)m(T,A,n); Z'!': *(D*)m(T,A,n)= S[-1]; S-=2;
// - stack
Z'd':S1=T;S++;Z'\\':S--;Z'o':S1=S[-1];S++;Z's':{D d=T;T=S[-1];S[-1]=d;}
// - numbers / functions call/define
//Z'A'...'Z': al(F[p[-1]-'A'],0,0,0,0); Z'^': A[1]= T; S=A+1; RET p-1;
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
//    alf(f?dchars(f):F[L m-'A'],s+1,S-s,c,0); *s=T;S=s;}
  //DEBUG(P("\n\tCALL: o="); dprint(s[1]); P(" nom="); dprint(nom); P(" m="); dprint(m); pc('\n'););
// -- numbers / math
Z'0'...'9': p--;U=reader(&p,0); // slow
Z'~': S--;T=deq(T,S1);
Z'%': S--;T=L T%L S1; Z'z': T=!T;Z'n':T=-L T;
#define OP(op,e) Z #op[0]: S--;T=T op##e S1;
OP(+,);OP(-,);OP(*,);OP(/,);OP(<,);OP(>,);OP(=,=);OP(|,|);OP(&,&);
// -- memory
Z'h': U=H-M; Z'm':x=T;T=H-M;H+=x; Z'a':H+=L POP;
Z'g': case ',': align(); if (p[-1]=='g') goto next; memcpy(H,&POP,SL); H+=SL;
//Z'@': T= *(D*)m(T,A,n); Z'!': *(D*)m(T,A,n)= S[-1]; S-=2;
// -- cX - char | string
/*
Z'c': s=0;switch(c=*p++){ Z'r':pc('\n'); Z'c':T=dlen(T); Z'=':S--;T=dcmp(T, S1);
  Z'"':case'\'':x=p[-1];e=p;while(*p&&*p!=x)p++;U=newstr(e,p++-e);
  // c?=charclass c1=first char
  // Z'?':case'1':x=T;if(isnan(T)){e=dchars(T);x=e?*e:0;};if(c=='1'){U=x;break;}
  // T= isalpha(x)?'a':isdigit(x)?'d':isspace(x)?'s':x=='_'?'_':'o';

  // TODO: need to free e???

  /// > ./alf -d <alf-printers.tst

  // but for ca% ???? errro
  //#define SP(P) do{T=newstr(e=P,-1);}while(0)

  #define SP(P) do{T=newstr(e=P,-1);free(e);}while(0)

  // TODO: buffer "type" to not create things for the GC?
  Z'a':{d=POP;s=sncat(0,dchars(POP),-1);U=d;c=*p++;case'e':case's':case'q':case'%':switch(c){
    Z'b':{char a[]={' ',0};SP(sncat(s,a,-1));} // TODO: use c fix sncat
    Z'e':{char a[]={T,0};SP(sncat(s,a,-1));} // TODO: use c fix sncat
    Z'q':SP(sdprinq(s,T)); Z'%':{e=--p;while(*p&&!isspace(*p))p++;
	char* f=strndup(e,p-e);SP(sdprintf(s,f,T));free(f);}
    case's':p++;default:p--;SP(sdprinc(s,T));}}}
      //    goto next;default:p-=2;goto error;}}}
*/
// -- printing
Z'.': dprint(POP);pc(' '); Z'e':pc(POP); Z't':P("%*s.",(int)T,M+L S[-1]);S-=2;
Z'p': dprint(POP); Z'\'': U= *p++; Z'"': while(*p&&*p!='"')pc(*p++); p++;
// -- hash/atoms/dict
Z'#': switch(*p++){ Z'a'...'z':case'A'...'Z':case'_':p--;U=atom(parsename(&p));
  Z',': S--;set(T,dlen(T),S1); Z':': S-=2;set(T,S1,S[2]);
  Z'@': S--;T=get(S1,T); Z'!': S-=3;set(S[3],S[2],S1);
  Z' ': case'\n': case 0: if ((e=dchars(T))) T=atom(e);
  Z'?': T=typ(T); Z'^': U=obj(); goto next; default: goto error; }
// -- loop/if
Z'}': RET(iff?p:0); Z'{': while(!((e=alf(p, A, n, E, 0)))){}; p= e;
// -- bitops
//#define LOP(op,e) Z#op[0]: S--; T=(L S1) op##e L T;
//Z'b': switch(*p++){ LOP(&,);LOP(|,);LOP(^,); Z'~': T= ~L T; }
// -- ` address of stuff
Z'`': switch(*p++) { Z'#': U=n; Z'0'...'9': U='0'-p[-1]-1; Z'A'...'Z':
 case'a'...'z':case'_':p--;U=atom(parsename(&p)); case' ':case 0:case'\n':
   T=atomaddr(T);break;default:goto error;}
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

default: error: P("\n[%% Undefined op: '%c' >>%s\n", p[-1], p-1);p++;} abort();
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
//
// NOTE:  in ./al cannot remove ' '
//   generated by '}'
//   and used by num 'name ? =blank
char* _opt(char*,char,int); // FORWARD
char* _sopt=0; void opt(char* p) { _sopt= p; _opt(p,0,0); }

char* _opt(char* p, char e, int obj){while(p&&*p){char c;if(!*p||*p==e)RET p;
    //P("OPT:");pal(p);P("\n"); // DEBUG
  switch(c=*p){case')':case'}':case']':P("\n%%ERROR: opt ");
    e?P("expected '%c' (%d)",e,e):P("unexpected '%c", c);
    P(" at\n%.*s <--HERE---> %s\n", (int)(p-_sopt), _sopt, p); abort();
  case'"':p++;while(*p&&*p!='"')p++; break; case'\'':p++;if(isalnum(*p)){
   parsename(&p);break;} obj=1; case'{':if(!obj)goto jump; case'(':case'[':
    {char*x=strchr("(){}[]",*p); if(x)p= _opt(p+1,x[1],1);break;}
  case'R': if (p[1]=='^') { *p= 128; } break;// tail rec
  case'I': jump: { if(*p=='I')p++; if(*p>=128) break; // idempotent
    char*e=_opt(p+1, '}',0); *e=' '; int x=e-p+(c=='I'); *p=128+x; p=e;
    assert(128+x<=255);
  }break; } p++; } RET p?p-1:0; }

// ENDWCOUNT

void pal(char* p) {
  while(p&&*p)
    if (*p==128) P("[TailRecurse]"),p++;
    else if (*p>128) P("[+%d]",*p++-128);
    else pc(*p++);
}

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

  ' ' -- space/delimiter
   ! - Forth ! (write mem)
   " - string
   # - numberp
   $ - stringp
   % - mod
   & - and TODO: short-circuit
   ' - quoting functions
   '? - atomp symbolp atom
   '32 - number (no need)
   'atm - atom
   '{} - obj/array
   '[] - array/obj
   '(1 ; 3)' - get value from stack 2'(1;3) => (1 2 3)
     like lisp `(1 ,(pop) 3)
     TODO: only allow in ` ?
   ( --
   ) --
   * - mul
   + - plus
   , - push (O v -- O)
   - - minus
   . - getprop (O A -- v)
   / - div
   0-9 - number
   : - setprop (O A v--O)
   ; - new Obj TODO: use '{} ???
   < - lt
   = - eq
   > - gt
   ?"foo" - print string
   @ - Forth @ (read mem f addr/atm)
   A - cAr
   B - memB (uses eq)
   C - Cons
   D - cDr
   E - Eval (al)
   F - Forth prefix
   G -- (G)assoc
   H -- (H)append
   I - If
   J -- (prog1/progn)
   K - Konsp
   L - n List (<n items> n -- L)
   M -- Mapcar [\raekv ...] L
   N -- Nth (of list/arr)
   O - Ordinal (length list/arr)
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

--- TODO: possibly ???

Sa-Sz - set param/local var
S'a-S'z --
SU - toupper
SL - tolower
SS - tostring
SI - tointeger
SD - todouble (SF)
SC - tocons(list)
SO - toobj
SJ - tojson
SA - SetcAr
SD - SetcDr
SR - SetRecons (C a d -- C/C')
F[]

?"foo"
?'foo'
?[foo]

?{T} - if T
?{T}{E} - if T E

?c - ischar 0-255
  ?^ = isctrl 0-31
  ?8 = isutf8 128-255
  ?p - isprint 32-126
    ?n - isalnum
      ?a - isalpha
        ?l - islower
	?u - isupper
      ?d - isdigit
    ?s - isspace
    ?. - ispunct

?# - !istyped == double:
  ?N - isnan
  ?I - isinfinity
  ?f - isfloat
  ?i - isinteger

?T istyped
?U isnull/isundef
?A isatom
?S isstring
?C iscons
?O isobj (no have any now!)

?? type

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
  //   TODO: -v? = variable size
  //   TODO: -h = heap size
  //   TODO: -m = mem size
  int errstop=0,verbose=0,quiet=0;

  while(--argc && argv++) { if(0);
    else if (0==strcmp("-q", *argv)) quiet++,verbose=0;
    else if (0==strcmp("-v", *argv)) verbose++,quiet=0;
    else if (0==strcmp("-d", *argv)) debug++,quiet=0;
    else if (0==strcmp("-E", *argv)) errstop=1; else {P("%%ERROR: alf - no such option '%s'\n", *argv);abort();}
  }

  initmem(16*1024); inittypes();

  char *p;

  p= strdup("666666 42 1 I{111}{222}F.F.F\\\n");
  pal(p);
  opt(p);
  pal(p);
  al(p,p,0,0,0); pc('\n');
  
  p= strdup("666666 42 0 I{111}{222}F.F.F\\\n");
  pal(p);
  opt(p);
  pal(p);
  al(p,p,0,0,0); pc('\n');

  // read-eval
  char* ln= NULL; size_t sz= 0;
  int err=0;
  while((quiet||P("\n> ")) && getline(&ln, &sz, stdin)>=0) {
    if (!ln||!*ln||*ln=='\n') continue;
    if (!quiet) pal(ln);
    opt(ln);
    if (verbose) {P("O "); pal(ln);}

    al(ln,ln,0,0,0);
    DEBUG(printf("\t[%% %ld ops]\n", nn); nn=0);
    if (S<=K) { P("\n%%STACK underflow %ld\n", S-K); err=1; } // DEBUG
    if (S+1>=K+SMAX) { P("\n%%STACK overrun\n"); err=1; } // DEBUG
    if (!deq(K[SMAX-1],error)) { P("\n%%STACK corrupted/overrun to cons/var storage\n"); err=1; } // DEBUG
    if (G-K+1>=GMAX) { P("\n%%GLOBALS overrun to cons/var storage\n"); err=1; } // DEBUG
    if (C-K+1>=CMAX) { P("\n%%CONS exhausted\n"); err=1; } // DEBUG
    if (err && errstop) abort();
  }
}
#endif
