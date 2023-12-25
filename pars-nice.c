//80----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "svar.c"

char* parsename(char** p) { static char s[64], i; i=0; while((isalnum(**p) // DEBUG
  ||**p=='_') && i<sizeof(s)-1) s[i++]=*(*p)++; s[i]= 0; return s;} // DEBUG



int debug= 0; // DEBUG
#define DEBUG(D) if (debug) do{D;}while(0);

// Results and Attributes
#define NV 256
char *V[NV]= {0}, *R[128]= {0}, *A[NV]= {0}; int nv= 0;

// TODO: nv++ and nr? hmmm
void newV(int nr) { nv++; free(V[nr]); V[nr]=0; free(A[nr]); A[nr]= 0; }

int eor(char* r) { return !r || !*r || *r=='\n' || *r=='|' && r[-1]!='\\'; }

// find start of attr val string
char* attrval(int nr, char a) { for(int i=nr+1;i<NV;i++){ char* s= A[i];
    // TODO: not MAXRES...
    DEBUG(if (debug) if (s) printf("find.A[%d] = '%s'\n", i, s));
    while(s&&*s&&(s=strchr(s, ' ')))if(s[1]==a&&s[2]=='=') return s+3; else s++;
  }
  return 0;
}

// Using named Rule, parse String do only N matches
char* parseR(char r, char* s, int n); // FORWARD
char* pR(char* r, char* s, int n) { nv++; return parseR(*r, s, n); }

//#include "nantypes.c"
// TODO: remove
char* sncat(char* s, char* x, int n) { int i= s?strlen(s):0, l= x?strlen(x):0; // DEBUG
  if (n<0 || n>l) n= l; s= realloc(s, 1024*((i+n+1024)/1024)); s[i+n]= 0; // DEBUG
  return strncpy(s+i, x?x:"", n), s; // DEBUG
}

// Generate (add to *G) from [Rule] stop at ENDchar nr being $0 V[NR]
int gen(char** g, char* r, char end, int nr) { int n, l; char *or=r, *v, *e;
DEBUG(if (debug>3) printf("GEN: '%s'\n", r));
while(*r && *++r!=end && *r) { switch(*r){
  case'$': r++;
    if (*r=='#') { char e[]={ ln+'0', 0 };
      *g=sncat(*g,e,-1); break;
    } else if (*r==':') { r++;
      printf("$: GEN *r='%c'\n", *r);
      v= V[*r-'0'+nr+1];
      printf("$: GEN v='%s'\n", v);
      n= findvar(v);
      printf("$: GEN: findvar '%s' => %d\n", v, n);
      if (!n) {
	printf("GEN: name='%s' is undefined\n", v); exit(1);
      }
      char s[10]= {0};
      if (n>0) 
	sprintf(s, "%d", n);
      else {
	sprintf(s, "$%d", ln+n+1);
      }
      *g=sncat(*g,s,-1);
      break;
    } else if(isdigit(*r)){ v=V[*r-'0'+nr+1]; l=-1; } else if (isalpha(*r)) {
    DEBUG(if (debug) printf("ATTRVAL= '%s'\n", r))
    e= v= attrval(nr, *r); if (!e) break; while(*e && *++e && *e!=' '){}; l= e-v;
   } else if ((e=strchr("\"\"''(){}[]<>", *r)) && isdigit(*++r)) { // $"1 quoted
    v= V[*r-'0'+nr+1]; *g= sncat(*g, e+0, 1);
    DEBUG(if (debug>1) printf("GEN e=%s r=%s v=%s \n", e, r, v)); // DEBUG
    while(*v){ if(*v==e[1]) *g=sncat(*g,"\\",1); *g=sncat(*g,v++,1); } v=e+1;l=1;
   }
   *g= sncat(*g, v, l); break;
  case '\\': r++; default: *g= sncat(*g, r, 1);
  }
  DEBUG(if (debug>2) printf("  V[%d]='%s'\n", nr, *g));
  }
  return r-or;
}



#define Z goto next; case 

// Parse a Rule for String using max N matches starting at results at NR
//   n==-1 infinity
//   n==1 one time (for ? + *)
// Returns:
///  NULL=fail
//   rest of string (unparsed)

char* parse(char* r,char* s,int n, int nr){char *p=0, *os=s, *t, *m;
  int on=n, onr=nv, x, oln=ln;
  // TODO: need to undo actions of
  // :E ::var :=var :X ...
  // copy of global/vars ???
  DEBUG(if (debug>2) printf("    parse '%s' '%s' %d\n", r, s, n));
  // TODO: move while below next?
  while(n-- && r && s) {next: while(isspace(*r))r++;
    // don't include in match
    if(*r)while(isspace(*s))s++;
DEBUG(if (debug>3)printf("     next '%s' (%d)\n\t   of '%s' left=%d\n",r,*r,s,n));

switch(*r){ case 0: case '\n': case '\r': case '|': return s; // end
Z'(': Z'{': assert(!"TODO: implement");
Z'[': r+= 1+gen(V+nr, r, ']', nr);
Z':': switch(x=r[1]) {
  Z'E': enterframe(); oln=ln; r+=2;
    printf("ACTION: $E oln=%d gn=%d\n", oln, gn);
  Z':': case'=': { char *name= 0;
      r+= 1;
      printf("r===='%s'\n", r);
      r+= gen(&name, r, ' ', nr);
      printf("r===='%s'\n", r);
      printf("name=='%s'\n", name);
      // This is allowing global var to be set/defined if not declared
      if (x=='=' || !frame)
	printf("SETVAR\n"),setvar(name);
      else 
	printf("NEWVAR\n"),x=newvar(name);
      ///sprintf(s, " n=%d", ln);
      //A[nr]= sncat(A[nr], s, -1);
      //printf("\nATTR[%d]='%s'\n", nr, A[nr]);
      printf("r AFTER = '%s'\n", r);
      printf("ACTION: $: '%s' ln=%d gn=%d\n", name, ln, gn);
    }
  Z'X': exitframe(); ln=oln; r+=2;
    printf("ACTION: $X oln=%d, gn=%d\n", oln, gn);
    goto next;
  default: A[nr]= sncat(A[nr], " ", 1); r+= 1+gen(A+nr, r, ' ', nr);
}
  
Z'?':case'+':case'*':x=*r++;newV(nr);while(s&&*s){p=s;s=parse(R[*r],s,-1,nr);
  if(x=='?'){r++;s=s?s:p;goto next;}if(x=='+'&&!s)goto fail;}  r++;s=s?s:p;

// % qoting/charclass
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

Z'A'...'Z': if ((p=pR(r++, s, -1))) s= p; else goto fail;
Z'\\': r++; default: if (*s==*r++) s++; /* matched */ else fail: {
  DEBUG(if (debug>2) printf("     FAIL '%s' '%s'\n", r-1, s));
  while(*r && !eor(r++)){}; if (eor(r) && r[-1]!='|') return NULL;
  // restore start state
  s=os; n=on; nv=onr;
  DEBUG(if (debug>2) printf("     |OR  '%s' '%s'\n", r-1, s));
  } // fail
} // switch
} // while
  return s;
}

/// capture

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
  x=parse(R[r],s,n,nr); if(!V[nr])V[nr]=x?strndup(s,x-s):x; nv=nr-1;return x;}

char* test(char r, char* s){ nv=0; char* e=parseR(r,s,-1); if(!e||*e)printf(
    "%%%s %c->'%s'\n",e?(*e?"MORE":"OK!"):"FAIL",r,e);
  printf("%s\n",V[nv+1]);return e;}

char rule=0; int dlm= '\n';
void readparser(FILE* f) { char *ln= 0; size_t z= 0; int n;
  while((n=getdelim(&ln, &z, dlm, f))>0) { if (ln && ln[n-1]=='\n') ln[n-1]= 0;
    DEBUG(printf("> %s\n", ln))
    if (!ln) break; if (!*ln || *ln=='#') continue;
    if (*ln=='<') readparser(fopen(ln+1, "r")); else
    if (ln[1]=='=') R[ln[0]]=strdup(ln+2); else switch(*ln){
    case '*': dlm=0; case '?': rule=ln[1]; break; default: test(rule, ln);
    } } free(ln); }

// ENDWCOUNT

int main(int argc, char** argv) {
  assert(sizeof(long)==8);
  assert(sizeof(void*)==sizeof(double));
  assert(sizeof(void*)==sizeof(long));
  assert(sizeof(void*)==sizeof(long));

  // parse arguments
  do{
    if (0==strcmp("-d", argv++[0])) debug++;
  } while(--argc);

  // init shortnames
  for(char*s="dawin\"'({[<"; *s; s++) {
    char x[]={'%',*s,0};
    R[*s]=strdup(x);
  }

  readparser(stdin);
}
