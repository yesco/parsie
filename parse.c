// Parse.c
//
// 
//

////////////////////////////////////////////////////////////////////////////////
// not as generic as I like to think?
//int till(char c, char** p, char q) { int n=0;
//  while(*p && **p && *(*p)++!=c) if (**p==q)(*p)++,n++; n++; RET n; }

// Results and Attributes
#define NV 256
char *V[NV]= {0}, *R[128]= {0}, *A[NV]= {0}; int nv= 0;

// TODO: nv++ and nr? hmmm
void newV(int nr) { nv++; assert(nv<NV); free(V[nr]); V[nr]=0;
  P("\nnewV: %d DELETE '%s'\n", nr, A[nr]);
  free(A[nr]); A[nr]= 0; }

//int eor(char* r) { RET !r || !*r || *r=='\n' || *r=='\r' || *r=='|' && r[-1]!='\\'; }

int eor(char* r) { RET !r || !*r || *r=='|' && r[-1]!='\\'; }

// find start of attr val string
char* attrval(int nr, char a) {
  for(int i=0;i<NV;i++){ char* s= A[i]; // DEBUG
    if (s || V[i]) P("...A[%d]='%s'\t V[%d]='%s'\n", i, s, i, V[i]); } // DEBUG
  
  for(int i=nr+2;i<NV;i++){ char* s= A[i];
    // TODO: not MAXRES...
    DEBUG(if (debug) if (s) printf("find:%c A[%d] = '%s'\n", a, i, s));
    while(s&&*s&&(s=strchr(s, ' ')))if(s[1]==a&&s[2]=='=') RET s+3; else s++;
  }
  RET 0;
}

// Using named Rule, parse String do only N matches
char* parseR(char r, char* s, int n); // FORWARD
char* pR(char* r, char* s, int n) { nv++; RET parseR(*r, s, n); }

// Generate (add to *G) from [Rule] stop at ENDchar nr being $0 V[NR]
int gen(char** g, char* r, char end, int nr) { int n, l; char *or=r, *v, *e;
  debug+=5; // DEBUG
DEBUG(if (debug>3) printf("GEN: '%s'\n", r));
while(*r && *++r!=end && *r) {
  DEBUG(  P("GEN.next: '%s'\n", r));
  switch(*r){
  case'{':v=0;r+=1+gen(&v,r,'}',nr);alf(v,S,n,0,0);free(v);
  case'$': r++; n=0;
    // TODO: literal text //if (*r=='$') //
    if (*r=='.') { r++;*g=sncat(*g,"\"before>>>\"",-1);
			    *g=sdprinc(*g,POP);
			    *g=sncat(*g,"\"<<<after\"",-1); break;;
    }
    else if (*r=='#') { char e[]={ ln+'0', 0 }; *g=sncat(*g,e,-1); break; }
    else if (*r==':') { char s[10]= {0}; r++; v= V[*r-'0'+nr+1]; n= findvar(v);
      if (!n) {	printf("\nReferenceError: %s is not defined\n", v); RET 0; } //exit(1); } // TODOL: longjmp(err);
      if (n>0) sprintf(s, "%d", n); else sprintf(s, "`%d", ln+n+1); // DEBUG
      *g=sncat(*g,s,-1); break;
    } else if(isdigit(*r)){ v=V[*r-'0'+nr+1]; l=-1; } else if (isalpha(*r)||*r=='$') {
      if (*r=='$') { n=r[1]-'0'-1;r+=2; }
      DEBUG(if (debug) printf("ATTRVAL= %d %d '%s'\n", n, nr+n, r-3));
      e= v= attrval(nr+n-3, *r); if (!e) break; while(*e && *++e && *e!=' '){}; l= e-v;
    } else if ((e=strchr("\"\"''(){}[]<>", *r)) && isdigit(*++r)) { // $"1 quoted
      v= V[*r-'0'+nr+1]; *g= sncat(*g, e+0, 1);
      DEBUG(if (debug>1) printf("GEN e=%s r=%s v=%s \n", e, r, v)); // DEBUG
      while(*v){ if(*v==e[1]) *g=sncat(*g,"\\",1); *g=sncat(*g,v++,1); } v=e+1;l=1;
    } else { break; /* ERROR */ }

    // used by many branch, unl break
    *g= sncat(*g, v, l); break;
  case '\\': r++; default: *g= sncat(*g, r, 1);
  }
  DEBUG(if (debug>2) printf("  [%d]='%s'\n", nr, *g));
  }
 debug-=5;
  RET r-or;
}

// Error diagnostics:
//   prog(ress): for each char pos:
//     A..Z = last succesful rule
//     a..z = failed rule (look last pos)
// TODO: expected
char *toparse=0, *prog=0, xr=0, *xp=0, *xs=0;

// Parse a Rule for String using max N matches starting at results at NR
//   n==-1 infinity
//   n==1 one time (for ? + *)
// Returns:
///  NULL=fail
//   rest of string (unparsed)

char* parse(char* r,char* s,int n, int nr,char cur){char *p=0, *os=s, *t, *m;
  int on=n, onr=nv, x, oln=ln;
  // TODO: need to undo actions of
  // :E ::var :=var :X ...
  // copy of global/vars ???
  DEBUG(if (debug>2) printf("    parse '%s' '%s' %d\n", r, s, n));
  // TODO: move while below next?
  while(n-- && r && s) {while(isspace(*r))r++;if(*r)while(isspace(*s))s++;
DEBUG(if (debug>3)printf("     next '%s' (%d)\n\t   of '%s' left=%d\n",r,*r,s,n));
switch(*r){case 0: case '|': RET s;
#define Z goto next; case 
  Z'[':r+=1+gen(V+nr,r,']',nr); Z'(':
  Z'{':t=0;r+=1+gen(&t,r,'}',nr);alf(t,S,n,0,0);free(t);

// -- actions
Z':': switch(x=r[1]) {
  Z'E': enterframe(); oln=ln; r+=2; Z'X': exitframe(); ln=oln; r+=2;
  Z':': case'=': { char *name= 0; r+= 1; r+= gen(&name, r, ' ', nr);
   if (x=='=' || !frame) setvar(name); else x=newvar(name); } 
   goto next; default: A[nr]= sncat(A[nr], " ", 1); r+= 1+gen(A+nr, r, ' ', nr); }
Z'?':case'+':case'*':x=*r++;newV(nr);while(s&&*s){p=s;s=parse(R[*r],s,-1,nr, *r);
  if(x=='?'){r++;s=s?s:p;goto next;}if(x=='+'&&!s)goto fail;}  r++;s=s?s:p;
// -- % qoting/charclass
Z'%':++r;if((p=strchr("\"\"''(){}[]<>",*r=='s'?*s:*r))){ if(*s!=*p)goto fail;
newV(nr);r++;  while(*++s&&*s!=*p){if(*s=='\\')s++;
  V[nv+nr]=sncat(V[nv+nr],s,1);}  s++;goto next;}
  // char class: %a %d %w %i %n %e
  // The T string classify curent
  // char, returning list of classes
  // "XYZZZZ" X skipped if first ch
  // XY will take more chars
  p=s; do{ t=isdigit(*s)?"nidw":isalpha(*s)?" nwa":*s=='_'?" nw":*s?0:"  e";
    m= t?strchr(t+(s==p), *r):0; if (!m && s==p) goto fail;
  } while(*++s && m && m-t<2);  s-=!m; r++;
// --- rules recursion
Z'A'...'Z': if ((p=pR(r++, s, -1))) s= p; else goto fail;
// --- match (and \quoted) char
Z'\\': r++; default: if (*s==*r++) {s++; goto next;} else fail: {
  if(s>=xs){xr=cur;xp=r-1;xs=s;}//printf("\nEXPECT s='%.10s'\nr='%.10s'\n", s, r);}
  DEBUG(if (debug>2) printf("     FAIL '%s' '%s'\n", r-1, s));
  while(*r&&!eor(r++)){}; if (r[-1]!='|') RET 0; // FAIL
  // OR
  if(s-os>=0)prog[s-toparse+1]=tolower(cur); s=os;n=on;nv=onr;
  DEBUG(if (debug>2) printf("     |OR  '%s' '%s'\n", r-1, s));
  } // fail
} // switch
  next:if (s-os>=0) if (prog[s-toparse+1]!='\n')  prog[s-toparse+1]=cur;
}// while
  RET s;
}

// --- rules
// is a single Capital follows by =
//
//   D=0|1|2|3|4|5|6|7|8|9
//   A=a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|_
//   W=A|D
//   N=A*W
//   D=%d
//   X=fib\(+D\)
//   L=D,L|D     -- right recursion ok
//
// Undefined rule always matches...
// (space is ignored during matching)
// (no newlines)

// --- catching & generating output
// (default result is verbatim input)
//
//   L=D,L [$1 $2 +] | D
//
//   "1,2,3"   =>   "1 2 3 + +"
//   A= int %n = %i; [_$1 $2 `$1] :t=int
//
//      $"1 $'2 $<3 $(4 $[5 $<6 - quote N

// --- char classes
//   %a = match alphabet and _
//   %d = match digit
//   %e = match end of text
//   %w = match word char (alphanum _)
//   %i = integer
//   %n = identifier (%a%w...)
//
// -- string matcher
//     ($1 always gives unquoted)
//     (default is verbatim)
//
//   %" = "foo\"bar"
//   %' = 'c' or 'foo\'bar'
//     (not handling nesting:???)
// TOOD: remove!
//   %( = ( fo( oooo ( \) foo bar)
//   %[ = [ fo[ oooo [ \] foo bar]
//   %{ = { fo{ oooo { \} foo bar}
//   %< = < fo< oooo < \> foo bar>
//   %s = START fo? START \END foo bar END
//     handle any style string

// --- multiple matches PREFIX!
//   ?R = optionall match rule R
//   +R = match rule R once or more
//   *R = match rule R 0 or more times
//   (R must be a NonTerminal)
//
// TODO: +R redundant == R*R
// TODO: ?R redundant == S=R| and ?S
// TODO: *R redundant == S=RS|R|
//
//

char* parseR(char r, char* s, int n){ int nr=++nv; nv--; newV(nr); char *x;
  x=parse(R[r],s,n,nr,r); if(!V[nr])V[nr]=x?strndup(s,x-s):x; nv=nr-1;RET x;}

// Blank out comments
// ... // c/c++ comment
// ^# script comment
// ... /* comm- \n pascal -ent */
// ... (* comm- \n lisp   -ent *)

// TODO: parametrice/config/input 9 loc
char* comments(char* os) { int c= 0; char *e= 0, *s= os; if (!s) RET os;
  while(*s) { c=0; e=0; switch(*s++) {
    case'\'':case'"': c=s[-1]; while(*s!=c){if(*s=='\\')s++; s++;break;c=0;}
    case'(': if (*s!='*') break;  c=1; e="*)";
    case'/': if(!c){if(*s!='/'){if(*s!='*')break;e="*/";}else e="\n";}  c=1;
    case'\n': if (!c){ if(*s!='#') break; e="\n"; }
      if (!e) break; s--; while(*s&&e&&strncmp(s, e, strlen(e))) *s++= ' '; c=0;
      for(int i=0;i<strlen(e);i++){if(*s&&!isspace(*s))*s++=' ';else s++;} e=0;
    } }
  RET os;
}
