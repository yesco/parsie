#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

int debug= 0; // DEBUG
#define DEBUG(D) if (debug) do{D;}while(0);

// Results and Attributes
#define MAXRES 256
int nres= 0;
char *res[MAXRES]= {0}, *R[128]= {0}, *attr[MAXRES]= {0};

int eor(char* r) {
  return !r || !*r || *r=='\n' || *r=='|' && r[-1]!='\\';
}

// find start of attr val string
char* attrval(int nr, char a) {
  // TODO: not MAXRES...
  for(int i= nr+1; i<MAXRES; i++) { char* s= attr[i];
    DEBUG(if (debug) if (s) printf("find.ATTR[%d] = '%s'\n", i, s));
    while(s && *s && (s= strchr(s, ' '))) if (s[1]==a && s[2]=='=') return s+3; else s++;
  } return NULL;
}

// Using named Rule, parse String do only N matches
char* parseR(char r, char* s, int n); // FORWARD
char* pR(char* r, char* s, int n) { nres++; return parseR(*r, s, n); }

// return new string with S followed by A, free(s)
char* add(char* s, char* a, int n) { char* r= malloc((s?strlen(s):0)+n+1);
  strncat(strcpy(r, s?s:""), a?a:"", n); free(s); return r; }

// Generate (add to *G) from [Rule] stop at ENDchar nr being $0 res[NR]
int gen(char** g, char* r, char end, int nr) { int n, l; char *or=r, *v, *e;
  DEBUG(if (debug>3) printf("GEN: '%s'\n", r));
  while(*r && *++r!=end && *r) { switch(*r){
    case '$': r++; if (isdigit(*r)) { v= res[*r-'0'+nr+1]; l=99999; } else if (isalpha(*r)) {
	DEBUG(if (debug) printf("ATTRVAL= '%s'\n", r))
	e= v= attrval(nr, *r); if (!e) break; while(*e && *++e && *e!=' '){}; l= e-v;
      } else if ((e=strchr("\"\"''(){}[]<>", *r)) && isdigit(*++r)) {
	v= res[*r-'0'+nr+1]; *g= add(*g, e+0, 1);
	printf("GEN e=%s r=%s v=%s \n", e, r, v);
	while(*v) { if (*v==e[1]) *g=add(*g, "\\", 1); *g= add(*g, v++, 1); }
	*g= add(*g, e+1, 1); break;
      }
      *g= add(*g, v, l); break;
    case '\\': r++; default: *g= add(*g, r, 1);
    }
    DEBUG(if (debug>2) printf("  res[%d]='%s'\n", nr, *g));
  }
  return r-or;
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

// Parse a Rule for String using max N matches starting at results at NR
//   n==-1 infinity
//   n==1 one time (for ? + *)
// Returns:
///  NULL=fail
//   rest of string (unparsed)
char* parse(char* r,char* s,int n,int nr){char*p=0,*os=s,*t;int on=n,onr=nres,x;
  DEBUG(if (debug>2) printf("    parse '%s' '%s' %d\n", r, s, n));
  // TODO: move while below next?
 next: while(n-- && r && s) {
    while(isspace(*s)) s++;

    DEBUG(if (debug>3)printf("     next '%s' (%d)\n\t   of '%s' left=%d\n",r,*r,s,n));
#define Z goto next; case 
    switch(*r){
    case 0: case '\n': case '\r': case '|': return s; // end
    Z' ': case '\t': while(isspace(*s))s++; r++;

    Z'(': Z'{': assert(!"implement"); // TODO: implement
    Z'[': { // result
      DEBUG(if (debug>2) printf("BEFORE  RES='%s'\n", res[nres]));
      r+= 1+gen(res+nr, r, ']', nr);
    }
    Z':': attr[nr]= add(attr[nr], " ", 1); r+= 1+gen(attr+nr, r, ' ', nr);

    // TODO: too clever to reuse instead of clear...
    case '?': case '+': case '*':
      printf("== res %d\n", nr); // DEBUG
      // TODO: is nres right? "passthrough doesn't seem to work for $s %" etc..."
      // TODO: several places?
      nres++;free(res[nr]); res[nr]=0; free(attr[nr]); attr[nr]= 0;
    switch(*r){
    case'?': case '+': p=s; s=parse(R[r[1]],s,-1,nr);
      if (*r=='?') { r+=2; s=s?s:p;goto next; }
      if (!s) goto next;
      // fallthrough
    case'*': r++;
      while(s&&*s) { p=s; s=parse(R[*r],s,-1,nr); }
      r++; s=s?s:p;
    }
    // TODO: also too clever encoding?
    Z'%': ++r;
      if ((p=strchr("\"\"''(){}[]<>", *r=='s'?*s:*r))) {
	// quoted string
        if (*s!=*p) return NULL;
	// TODO: several places?
	nres++;free(res[nres+nr]);res[nres+nr]=0;free(attr[nres+nr]);attr[nres+nr]= 0;
	// move till unquoted endchar
	while(*++s&&*s!=p[1]) {
	  if (*s=='\\') s++;
	  res[nres+nr]=add(res[nres+nr], s, 1);
	}
	// TODO: result without quotes?
	r++; s++; goto next;
      }

      if (*r=='e'&&!*s) { r++; goto next; }
      p=s;
      do if ((x=(*r=='_'
          || strchr("anw",*r) && isalpha(*s)
	  || strchr(p==s?"di":"dwin", *r) && isdigit(*s))))
	  s++;
	else if (p==s) goto fail;
      while(strchr("in", *r) && x);
      r++;

    Z'A'...'Z': if ((p=pR(r++, s, -1))) s= p; else goto fail;
    Z'\\': r++; default:
      if (*s==*r++) s++; /* matched */ else fail: {
        DEBUG(if (debug>2) printf("     FAIL '%s' '%s'\n", r-1, s));
        while(*r && !eor(r++)){}; if (eor(r) && r[-1]!='|') return NULL;
	  // restore start state
        s=os; n=on; nres=onr;

        DEBUG(if (debug>2) printf("     |OR  '%s' '%s'\n", r-1, s));
      } // fail
    }
  }
  return s;
}

/// capture
char* parseR(char r, char* s, int n) { int nr=++nres;
  DEBUG(if (debug>1) printf("  parseR '%c' (%d)\n", r, r));

  // TODO: several places?
  free(res[nr]); res[nr]=0; free(attr[nr]); attr[nr]= 0;

  char *x= parse(R[r],s,n,nr);

  // TODO: combine repeats? concat && nres-- after call parseR
  // TODO: matching with ? seems to "fail" ???
  // TODO: for matching only subrule "| X" default should be [$1] ???

  if (!res[nr]) res[nr]=x?strndup(s, x-s):x;

  DEBUG(if (debug>1) printf("  ->%c.res[%d]='%s'\n", r, nr, res[nr]));

  nres= nr-1; return x;
}

char* test(char rule, char* s) { nres= 0;
  char* e= parseR(rule, s, -1);
  if (!e || *e) printf("%%%s %c-> '%s' RES=>'%s'\n\n", e?(*e?"UNPARSED":"MATCHED!"):"FAILED", rule, e, res[nres+1]);
  printf("%s\n", res[nres+1]); return e;
}

//  Z '?': r++; p=s; s=parse(R[*r],s,1); r++; s=s?s:p;
//  Z '*': r++; do{p=s; s=parse(R[*r],s,1);}while(s && *s);r++;s=p;
//  Z '+': r++; s=parse(R[*r],s,1); while(s&&*s){p=s;s=parse(R[*r],s,1);};r++;s=p;


// TODO: count loader/reader?

// # comment
// A=foo|bar|X
// ?A
// foo
// barf
// @
// A=fie fum
// foo
// fie
// *A
//  fie                space ignored
// fum
void readparser(FILE* f) { char rule, *ln= NULL; size_t z= 0, d='\n';
  while(getdelim(&ln, &z, d, f)>0) {
    if (ln && ln[strlen(ln)-1]=='\n') ln[strlen(ln)-1]= 0;
    DEBUG(printf("> %s\n", ln))
    if (!ln) break;
    if (!*ln) continue;
    if (ln[1]=='=' && isalpha(*ln) && isupper(*ln)) R[ln[0]]= strdup(ln+2);
    else if (*ln=='#') ; // comment
    else if (*ln=='?') rule=ln[1];
    else if (*ln=='@') for(int i=0; i<127; i++) { free(R[i]); R[i]=NULL; }
    else if (*ln=='*') { rule=ln[1]; d=0; }
    //    else if (*ln=='$' && strchr("[{(<", ln[1])) { parsgen(ln+1, "", NULL); free(pgen); pgen= NULL; }
    else test(rule, ln);
  }
  free(ln);
}

// ENDWCOUNT

int main(int argc, char** argv) {
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
