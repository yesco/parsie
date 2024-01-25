//80----------------------------------------------------------------------------
// Alphabetical Lisp byte code
//
// (c) 2014 Jonas S Karlsson
//      (jsk@yesco.org)

// This file defines a "LISP-in-Forth".
//
// It's built as an alternative to
// ALF-The ALphabetical Forth byte code.
//
// It's a stack-machine language and
// each word is typically one letter
// or symbol.
//
// For a full reference see:
// - ALphabetical LISP list at end
// - Source code, words grouped by type
// - all-letters.al - a test file
//
// As ALF, AL uses NaN-encoding of
// other data types. Values have types.
// 
// The following value (types) exist:
// - C double numbers; int54 NaN Inf
// - Symbols, immutable, interned
// - Strings, immutable, managed
// - Objects, prototypical, ala JS
//   these can behave like an array
//   and/or a property/record collection
// - Cons, lisp list data type
// - Closure/Function, callable
//
// The system is prepared for a GC.
// The garbage collector will do simple
// mark and sweep, at well-defined
// moments.
//
// As Forth-style words don't know how
// many arguemnts are provided. Most
// functions take a fixed number.
//
//     3 4 + 7 +  => 14
//     1  2  C    => (1 . 2)
//     'a 'b C    => (a . b)
//     'a'bCD     => b
//
//  1 2 3 4 5 'nil CCCCC => (1 2 3 4 5)
//
// An exception is L(ist) that takes
// an additonal argument being number
// of elements on the stack.
//
//  'a 'b 'c 'd 'e 5 L => (a b c d e)
//  '(a [1 2] 3 e)     => (a [1 2] 3 e)
//  '{1 a:2 3 c:4 5}=>[1 3 5 a:2 c:4]
//
// The quote: '-reader can read all
// data-types (except closures?),
// and will use the appropriate
// constructors.
//
// === \ambda and Functions
//
// Consider:
//
//   > 3 4 5 * + P
//   23
//
// How do we write this as a function?
//
// One way is just plain string:
//
//   > 3 4 5 " * + " E P
//   23
//
// E stands for Eval. This is crude and
// it just pops values of the stack.
//
// We can store it in a variable:
// 
//   > " * + " 'foo !
//
//   > 3 4 5 'foo @ E P
//   21
//
//   > 1 7 10 'foo @ E P
//   71
//
// A simplified form of lambda is
// provided:
//
//   > 3 4 \\ ab+^    | reads till EOL
//   > P
//   7
//
// Notice how there is a \ for each
// implicit parameter--essentially,
// just counting how many. Also,
// NOTE: a ^ marks a return. There
// can be several of these. What it
// does it removes the arguments,
// moves the single result down
//
// The very same can be written as
//
//   > 3 4 \\xy xy+^
//
// === Recursion
//
// That's all dandy, now hwo about
// something more real?
//
// Let's calculate the length of a list!
//
//  > '(a 2 [3 4] 7) \l lKI{lDR1+^}{0^}
//  > P
//  4
//
// WTF? Ok, let's dissect this line-noise...
//
//   \l lKI{lDR1+^}{0^}
// 
//   \l   - (lambda (l) ...
//   l    - get value of parameter l
//   K    - Kons? (is it a cons-cell?)
//   I    - If (not 0)
//   {    - { .. } is the THEN block
//     l    - get value of l
//     D    - cDr of it
//     R    - Recurse  (Recurse (cDr l))
//     1    - push 1 on the stack
//     +    - add them
//     ^    - return the sum
//   }    - end of THEN
//   {    - start of ELSE
//     0    - put 0 on stack
//     ^    - return it
//   }    - end of ELSE
//
// This is a prefix encoding of LISP:
//
//   (\ambda (l)
//     (If (Kons? l)
//         (return (+ (Recurse (cDr l)) 1))
//        (return 0) ) )
//
// AL's 18 characters represent 62 in LISP.
// LISP uses 18 cons:es, and 13 symbols.
// (maybe a bit easier to read though...)
//
// Here is a simple Mapcar function:
//
//   
// Try to decode it on your own, using
// the summary below.


// /===================================\
// | Cheatsheet AL - Alphabetical Lisp |
// =====================================
//
//  assoc/G    \ambda          Recurse
//  append/H   Ordinal/length  read/Y
//  cAr        nList           SetcAr
//  cDr        Mapcar          SetcDr
//  Cons       memBer          setq/!
//  Eval       not/~           Setq/Sa-Sz
//  eq/=       Nth             Terpri
//  eQual      Print           nUll/Undef
//  If         prin1/X
//  Konsp      princ/Write
// 
//      + - * / %  &=bitand |=bitor
//
//            -- If --
// 1 1 = I{"Then"}{"Else"}P => Then
// 1 0 = I{"Then"}{"Else"}P => Else
// 3 4 {This works like a comment!} + P
//
//          -- \ambda --
// 3 4 5  *+       3 4 5 \\\  bc*a+ ^
// 3 4 5 "*+" E    3 4 5 \xyz yz*x+ ^
// 3 4 5 [*+] E    3 4 5 \xyz *+    ^
//
//          READ      SET
// Args:    a b c     3 Sa 4 Sc
// Globals: 'foo @    42 'foo !
//
//        -- by data type --
// Testers:  #/number? Kons? nUll ?'=atom
// Numbers:  # + - * / % & |
// Strings:  "a string" 
// Atoms:    'a  "foo"#$  
// IO:       33P       ==  T3W3W  
//           ?["foo"]  ==  "foo"X
// Lists:    Kons? Cons cAr cDr nList
//           Nth Ord/length Mapcar
//           memBer G/assoc H/append
// Control:  Eval I{Then}{Else} I{Then}
//           Recurse ^/return
// Address   `foo == 'foo`$ 
// Args:     a-z == '0-`9 42Sc `# =#args
// Objects:  '(1 [2 3 {a:4 5 6}] 7 8)
//      ==   "(1 [2 3 {a:4 5 6}] 7 8)" '$
// Template  7'b 33'{a: ; ;:42 [1 ; 42]}
//       ==        '{a:33 b:42 [1 7 42]}
//           '(3 4) '(1 2 @   5 6)
//       ==         '(1 2 3 4 5 6)

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

// enable to trace memory allocations
//#include "malloc-trace.c"

#include "mem.c"
#include "nantypes.c"

// total tokens processed
long nn=0;

char* alf(char*p,D*A,int n,D*E,int iff){assert(!"not ALF it's AL!\n");} // DEBUG

char* al(char*o,char*p,D*A,int n,D*E);// FORWARD
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

// TODO: ??? opt:
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
// TODO: not recurse... stack
// TODO: need setcdr...
// TODO: A,n for access to var?
D map(D f, D l, D all, D r, D opt, int k) {if(!iscons(l))RET nil;
  D e,*z= S; {U=r;U=all;U=l;U=k;U=car(l); char*x=dchars(f);
  al(x,x,0,0,0);e=POP;}S=z; RET cons(e,map(f,cdr(l),all,r,opt,k+1));}

// TODO: potentially BUG encoding
// if followed by a jump...
//
//     'foo{jump over me}
//     _foo{skip}

// TODO: remove isidigit?
D parseatom(char** p) {
  if(isdigit(**p)){int n=0;while(isdigit(**p)){n=n*10+**p-'0';(*p)++;}RET n;}
  if(**p>=128) {int n=0,s=0;while(**p>=128){n=n+((**p-128)<<s);s+=7;(*p)++;}
    RET K[SMAX+n*2];  } RET atom(parsename(p));}

// al parses code/a function/lambda
// o=p when call first; "R" resets p=o
char* al(char*o,char*p,D*A,int n,D*E){ long x;char*e=0,*s,c,*X=0;D d;size_t z;
// Temp vars: L x ; char*e,*s,c ; D d ; size_t z ;

// Argument names string: xyz=3 args
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

e=p;x=0;nn++;switch(c=*p++){case 0:case')':case']':RET(p);Z' ':Z'\n':Z'\t':Z'\r':

// Address  `atm-addr  `$=addr-of-TOPatom  `#=num-args  `0-`9=addr-argN
Z'`': switch(*p++) { Z'#': U=n; Z'0'...'9': U='0'-p[-1]-1; Z'A'...'Z':
  case'a'...'z':case'_':p--;U=parseatom(&p);
  case'$': T=atomaddr(T); goto next;default:goto error; }

// -- $stringp  "a string"
Z'$':T=isstr(T); Z'"':U=readstr(&p,'"');

// -- `atom  '?=atomp/symbolp  '$=atom-from-str
Z'\'': if(*p=='?'&& p++)T=isatom(T); else if(*p=='$'){p++;char*s=dchars(T);
  T=s?reader(&s,0):error; } else U=reader(&p,1);

// -- (Global Variables)  !  @  SetcAr  SetcDr  Sa-Sz
Z'@': T= *(D*)m(T,A,n); Z'!': *(D*)m(T,A,n)= S[-1]; S-=2;
Z'S': switch(c=*p++){ Z'A':S--;T=setcar(T,S1); Z'D':S--;T=setcdr(T,S1);
Z'a'...'z':VAR=T--; goto next;default:p--;goto error; }

// -- ?"printme" ?'..' ?[..]   ?a=print" a=... " (debug)
Z'?':switch(c=*p++){
  default:if(!isalpha(c)){p--;goto error;} P(" %c=",c);dprint(VAR);
  case'_':P(" "); Z'?':P(" [");dprint(T);P("] ");
  Z'[':c=']';case'"':case'\'':while(*p&&*p!=c)pc(*p++);p++; }

// -- Konsp? Cons cAr cDr nList memBer Gassoc Happend Mapcar Nth Ordina/length
Z'K': T=iscons(T); Z'C':S--;T=cons(T,S1); Z'A':T=car(T); Z'D':T=cdr(T);
Z'M': S--;d=map(T,S1,S1,nil,nil,0);U=d;   Z'U':T=deq(T,nil)||deq(T,undef);
Z'L': x=POP;d=nil; while(x-->0)d=cons(T--,d); U=d; Z'H':S--;T=append(T,S1);
Z'O': if(isobj(T))T=dlen(T);else{x=0;while(iscons(T)&&++x)T=cdr(T);T=x;}
Z'N': S--;if(isobj(S1))T=get(S1,T);else{while(T-- >0)S1=cdr(S1);  T=car(S1);}
Z'G': while(iscons(T)&&!deq(car(car(T)),S[-1]))T=cdr(T); S--;T=S1;
Z'Q': S--;T=!dcmp(T,S1); Z'B': S--;while(iscons(S1)) {if(deq(T,car(S1))){
  T=S1;goto next;} S1=cdr(S1);} T=nil;

// -- Control flow:  Eval  If  Recurse  128=tailrecurse  >128=skipchars 
Z'E': e=dchars(POP);al(e,e,A,n,E); Z'I': if(POP)p++; //Z'J': // ?? prog1
Z'_': e=dchars(*(D*)m(parseatom(&p),A,n));al(e,e,A,n,E); Z'R': al(o,o,S,0,0);
Z(129)...(255):p+=c-128; Z 128:p=o;spc(&p);if(*p=='\\')while(*p&&!isspace(*p))
  p++; for(c=0;c<n;c++)A[c+1]=S[-n+c+1]; S-=n;p++; Z'^': A++;*A=T;S=A;RET(p-1);

// -- Print Terpri Write/princ X/prin1 (not return val)
Z'P': pc('\n'); case'W': dprint(POP); Z'T':pc('\n');
Z'X': P("%s",e=sdprinq(0,POP));free(e);
Z'Y':e=0;z=0;if(getline(&e,&z,stdin)>=0)s=e,U=reader(&s,0);else U=error;free(e);

// -- Objects: ;create :set .get Ordinal/length
Z',': S--;set(T,dlen(T),S1);    Z':': S-=2;set(T,atomize(S1),S[2]); 
Z'.': S--;T=get(T,atomize(S1)); Z';': U=obj(); 

// faster here than at the beginning, LOL
// inline makes 5.04s instead of 5.70s for fib-l.al...
Z'0':if(isdigit(*p))x=8; if(*p=='x')p+=2,x=16; if(*p=='b')p+=2,x=2;
case'1'...'9':{x=x?x:10;d=0;p--;while(isdigit(*p))d=d*x+*p++-'0';
U=isxdigit(*p)||*p=='.'?strtod(e,&p):d;}
Z'a'...'z':{D*z=&VAR;if(!A||z>S||z<K)P("Arg err: %c\n",c);U=*z;}

// -- \ lambda functions and parameters  a-z vars ------------^
Z'\\': o=p-1;n=1;while(*p=='\\')n++,p++;if(*p>='a')strcpy(a,parsename(&p));n=*a?strlen(a):n;A=S-n;

// -- Numbers and Math operators AND or &bit, OR or |bit, NOT or ~inv
#define OP(op,e) Z #op[0]: S--;T=T op##e S1;
Z'#':T=!istyped(T); Z'%': S--;T=L T%L S1; Z'~':T=!T; Z'&': S--;T=L T&L S1;
OP(+,);OP(-,);OP(*,);OP(/,);OP(<,);OP(>,);OP(=,=);   Z'|': S--;T=L T|L S1;

// -- include the body of alf.c
// There is no slowdown even if switch is bigger...
#ifndef NOALF // DEBUG
  #define ALF // DEBUG
#endif // DEBUG
#ifdef ALF // DEBUG
Z'F': switch(c=*p++){ // DEBUG
  #include "alf-body.c" // DEBUG
} // DEBUG
#endif // DEBUG

default: error: P("\n[%% Undefined op: '%c' >>%s\n", p[-1], p-1);p++; abort();
}}

// Simple peep-hole optimization
// 
// Actually, it's more an "compiler"
//
// 
// This destructively changes the str.
//
//   "..." skip
//   {...}   => "[+4]... ""
//at { put jump to } replace w ' '
//
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
// - let let* Variables?
// - prog prog* prog1 prog2 progn
// - fold reduce remove reVerse
// - max min sqrt

// list of lisp functions
// - https://homepage.divms.uiowa.edu/~luke/xls/tutorial/techreport/node87.html

/*

  == List of all LISP op=codes
  ( used are  : " L - ")
  ( unused are: " L --  ")

  ^@ - 0 - end zero terminated string
  ^a -- 1 -- TODO: 7 object types?
  ^b -- 2
  ^c -- 3
  ^d -- 4
  ^e -- 5
  ^f -- 6
  ^g -- 7
  ^h -- 8 \b -- backspace
  ^i -  9 \t - tab in code
  ^j -  10 \n - in code ?
  ^k -- 11 -- 
  ^l -  12 form feed - new page in code ?
  ^m -  13 \r - in code ?
  ^n -- 14 -- free
  ^o -- 15 -- free
  ^p -- 16 >>>> TODO: 16 codes 16-31
  ^q -- 17 
  ^r -- 18 
  ^s -- 19 
  ^t -- 20 
  ^u -- 21
  ^v -- 22
  ^w -- 23
  ^x -- 24
  ^y -- 25
  ^z -- 26
  ^[ - 27
  ^\ - 28
  ^] - 29 (ESC)
  ^^ - 30
  ^_ - 31 <<<<< 16 codes
 ' ' - 32 in code

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
          ()                 ;   

  @ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_
            J           V   Z[ ] _

  `abcdefghijklmnopqrstuvwxyz{|}~
  `                            

  ' ' -- space/delimiter
   ! - Forth ! (write mem)
   " - string
   # - numberp
   $ - stringp
   % - mod
   & - and TODO: short-circuit
   ' - quoting functions
   '? - atomp symbolp
   '32 - number (no need)
   '".." -- template string
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
   0-9 - number TODO decimal/0x/0b/07
   : - setprop (O A v--O)
   ; - new Obj TODO: use '{} ???
   < - lt
   = - eq
   > - gt
   ?"foo'bar" - print string
   ?'foo"bar"
   ?[foo'"bar]
   ?a-?c debug print " a=42 "
   ?? debug print TOP " [42] "
   @ - Forth @ (read mem f addr/atm)
   A - cAr
   B - memB (uses eq)
   C - Cons
   D - cDr
   E - Eval (al)
   F - Forth prefix
   G - (G)assoc
   H - (H)append
   I - If
   J -- (prog1/progn)
   K - Konsp
   L - n List (<n items> n -- L)
   M - Mapcar [\raekv ...] L
   N - Nth (of list/arr)
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
   Y - (Y)read
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
 128 - TailRecursion
 129-255 - skipNchars

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

#include<sys/time.h>

D stime(void) {
    struct timeval tv;

    gettimeofday(&tv,NULL);
    return ((D)tv.tv_sec)+(D)tv.tv_usec/1000000;
}

// options
int errstop=0,verbose=0,quiet=0;

void bench(char* ln, int f) {
  P("\n");
  char* name= ln;
  while(*ln && !isspace(*ln))ln++;
  *ln++= 0;
  spc(&ln);
  DEBUG(printf("name>%s\n", name));
  DEBUG(printf("LN>%s\n", ln));
  opt(ln);
  DEBUG(P("calibrating...\n"));
  D t=stime();
  // calibrate
  long n=0;
  D* s= S;
  while(stime()-t<0.1) {
    S= s;
    al(ln,ln,0,0,0);
    n++;
  }
  n=n*42;
  DEBUG(P("running... %ld\n", n));
  // run
for(int j=0;j<3;j++) {
  nn=0;
  t= stime();
  for(long i=n; i; i--) {
    S= s;
    al(ln,ln,0,0,0);
  }
  t=stime()-t;

  P("BENCH %-10s %5.3fMops %5.3fMips", name, f*(n+0.0)/1000/1000/t, nn/t/1000/1000);
  if (verbose) P(" %5.3fs %5.3fM", t, f*(n+0.0)/1000/1000);
  P("\n");

  nn=0;
}

return; 

  // maybe if we duplicate the ops
  // the al call overhead is "gone"
  if (0==strcmp("bench", name)) return;

  for(int i=1; i<=4; i++) {
    dstr y= sncat(0,"bench ",-1);
    for(int j=1; j<=i; j++)
      y= sncat(y, ln, -1);
    printf("RUN>%s\n", y);
    bench(y,i);
    free(y);
  }
}

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
  while(--argc && argv++) { if(0);
    else if (0==strcmp("-q", *argv)) quiet++,verbose=0;
    else if (0==strcmp("-v", *argv)) verbose++,quiet=0;
    else if (0==strcmp("-d", *argv)) debug++,quiet=0;
    else if (0==strcmp("-E", *argv)) errstop=1; else {P("%%ERROR: alf - no such option '%s'\n", *argv);abort();}
  }

  initmem(16*1024); inittypes();

  char *p;

  if (0) {
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
  }

  // read-eval
  char* ln= NULL; size_t sz= 0;
  int err=0;
  while((quiet||P("\n> ")) && getline(&ln, &sz, stdin)>=0) {
    if (!ln||!*ln||*ln=='\n') continue;
    if (0==strncmp("BENCH ",ln,6)) {bench(ln+6,1);continue;}
    if (!quiet) pal(ln);
    opt(ln);
    if (verbose) {P("O "); pal(ln);}

    D s=stime();
    al(ln,ln,0,0,0);
    s=stime()-s;
    if (verbose) {P("\n[%% %ld ops in %5.3fs %5.3f Mops]\n", nn, s, nn/s/1000/1000); nn=0;};
    if (S<=K) { P("\n%%STACK underflow %ld\n", S-K); err=1; } // DEBUG
    if (S+1>=K+SMAX) { P("\n%%STACK overrun\n"); err=1; } // DEBUG
    if (!deq(K[SMAX-1],error)) { P("\n%%STACK corrupted/overrun to cons/var storage\n"); err=1; } // DEBUG
    if (G-K+1>=GMAX) { P("\n%%GLOBALS overrun to cons/var storage\n"); err=1; } // DEBUG
    if (C-K+1>=CMAX) { P("\n%%CONS exhausted\n"); err=1; } // DEBUG
    if (err && errstop) abort();
    //gc();
  }
}
#endif
