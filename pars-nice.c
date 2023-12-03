#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

int debug= 0; // DEBUG
#define DEBUG(D) if (debug) do{D;}while(0);

#define MAXRES 256
int nres= 0;
char *res[MAXRES]= {0}, *pgen= NULL, *R[128]= {0};

int eor(char* r) {
  return !r || !*r || *r=='\n' || *r=='|' && r[-1]!='\\';
}

char* parseR(char r, char* s, int n); // FORWARD
char* pR(char* r, char* s, int n) {
  nres++;
  return parseR(*r, s, n);
}

// return new string with S followed by A, free(s)
char* add(char* s, char* a, int n) {
  char* r= malloc((s?strlen(s):0)+n+1);
  strncat(strcpy(r, s?s:""), a?a:"", n);
  free(s);
  return r;
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
//   L=D,L [$1 $2 +] | D
//
//   "1,2,3"   =>   "1 2 3 + +"

// --- char classes
//   %a = match alphabet and _
//   %d = match digit
//   %e = match end of text
//   %w = match word char (alphanum _)
//   %i = integer
//   %n = identifier (%a%w...)
//
//   %" = "foo\"bar"
//   %' = 'c' or 'foo\'bar'
//     (not handling nesting:)
//   %( = ( fo( oooo ( \) foo bar)
//   %[ = [ fo[ oooo [ \] foo bar]
//   %{ = { fo{ oooo { \} foo bar}
//   %< = < fo< oooo < \> foo bar>

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
//   rest of string (unparsed) it's "" if done.
char* parse(char* r, char* s, int n, int nr) {
  char*p= NULL, *os= s, *t;
  int on= n, onr= nres, x;
  DEBUG(if (debug>2) printf("    parse '%s' '%s' %d\n", r, s, n));
 next:
  while(n-- && r && s) {
    while(isspace(*s)) s++;

    DEBUG(if (debug>3)printf("     next '%s' (%d)\n\t   of '%s' left=%d\n",r,*r,s,n));

    switch(*r){

    case 0: case '\n': case '\r': case '|': return s; // end
    case ' ':case '\t': while(isspace(*s))s++; r++; goto next; // skip

    case '(': case '{': goto next; // TODO: implement
    case '[': { // result
      DEBUG(if (debug>2) printf("BEFORE  RES='%s'\n", res[nres]));
      while(*++r!=']') {
	switch(*r){
	case '$':
	  x=*++r-'0'+nr+1;
	  res[nr]= add(res[nr], res[x], 999999);
	  break;
	case '\\': r++; // fallthrough
	default: res[nr]= add(res[nr], r, 1);
	}
	DEBUG(if (debug>2) printf("  res[%d]='%s'\n", nr, res[nr]));
      }
      r++;
      goto next;
    }

    // TODO: too clever to reuse instead of clear...
    case '?': case '+':
      p=s;
      s=pR(r+1,s,1);
      if (*r=='?') { r+=2; s=s?s:p;goto next; }
      if (!s) goto next;
      // fallthrough
    case'*':
      r++;
      while(s&&*s) {
	p=s;
	s=pR(r,s,1);
      }
      r++; s=p; goto next;

    // TODO: also too clever encoding?
    case '%':
      if ((p=strchr("\"\"''(){}[]<>",*++r))) {
	// quoted string
	// move till unquoted endchar
	while(*++s&&*s!=p[1])
	  if (*s=='\\') s++;

	r++; s++; goto next;
      }

      if (*r=='e'&&!*s) { r++; goto next; }
      p=s;
      do {
	if ((x=(*r=='_'
          || strchr("anw",*r) && isalpha(*s)
	  || strchr(p==s?"di":"dwin", *r) && isdigit(*s))))
	  s++;
	else if (p==s) goto fail;
      } while(strchr("in", *r) && x);
      r++; goto next;

    case 'A'...'Z': if ((p=pR(r++, s, -1))) {
	s= p; goto next;
      } else goto fail;
    case '\\': r++; // fallthrough
    default:
      if (*s==*r++) s++; // matched
      else fail: {

	  DEBUG(if (debug>2) printf("     FAIL '%s' '%s'\n", r-1, s));
	  while(*r && !eor(r++)){};
	  if (eor(r) && r[-1]!='|') return NULL;
	  // restore start state
	  s=os; n=on; nres=onr;

	  DEBUG(if (debug>2) printf("     |OR  '%s' '%s'\n", r-1, s));
	} // fail
    }
  }
  return s;
}

/// capture
char* parseR(char r, char* s, int n){
  DEBUG(if (debug>1) printf("  parseR '%c' (%d)\n", r, r));
  int nr=++nres;

  free(res[nr]); res[nr]=0;

  char *x= parse(R[r],s,n,nr);

  // TODO: combine repeats? concat && nres-- after call parseR
  // TODO: matching with ? seems to "fail" ???
  // TODO: for matching only subrule "| X" default should be [$1] ???

  if (!res[nr]) res[nr]=x?strndup(s, x-s):x;

  DEBUG(if (debug>1) printf("  ->%c.res[%d]='%s'\n", r, nr, res[nr]));

  nres= nr-1;
  return x;
}

// ENDWCOUNT

char* test(char rule, char* s) {
  nres= 0;
  char* e= parseR(rule, s, -1);
  if (!e || *e) printf("%%%s %c-> '%s' RES=>'%s'\n\n", e?(*e?"UNPARSED":"MATCHED!"):"FAILED", rule, e, res[nres+1]);
  printf("%s\n", res[nres+1]);
  return e;
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
// fie
// fum
void readparser(FILE* f) {
  char rule, *ln= NULL;
  size_t z= 0, d='\n';
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

int main(int argn, char** argv) {
  // parse arguments
  do{
    if (0==strcmp("-d", argv++[0])) debug++;
  } while(--argn);

  // init shortnames
  for(char*s="dawin\"'({[<"; *s; s++) {
    char x[]={'%',*s,0};
    R[*s]=strdup(x);
  }

  readparser(stdin);
}
