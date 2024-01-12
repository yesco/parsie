//80----------------------------------------------------------------------------
// driver.c - repl/loop/driver
//
// The driver integrates parse and alf,
// parsing command arguemnts and loading
// files, providing a rudimentary command
// interface for interaction.
//
// Giving it '-' as filename will make it
// an repl.
// 
// The Poly Universal Compiler (driver)
// Is invoked by:
//
//   ./puc [-d [-d ...] ] js.bnf -e '*P' fib.js -e '?S' -
//
// This loads the definition for
// javascript first, runs fib.js,
// then plops you into a command repl.



// Variables/Letters used
//
// A - (inside alf: Arguments)
// B (from M?)
// C - Cells here
// D - type double
// E - prev stackframe F-- lol
// F - alf: functions
// G
// H - here for M
// I
// J
// K - K storage
// L - macro "(long)"
// M - byte Memory storage rename to B?
// N
//(O)
// P - printf
// Q 
// R - Rules of parser
// S - (alf) stack pointer top
// T
// U - pUsh
// V
// X
// Y - parse; magic calls stack 
// Z


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

int debug= 0; // DEBUG
#define DEBUG(D) if (debug) do{D;}while(0);

#include "alf.c"
#include "avar.c"
#include "parse.c"

int xalf=0; char rule=0,last=0; int dlm= '\n';

void alfie(char* a) { if (!a) return;
  if (xalf || debug>1) printf("\talf>>>%s<<<\n", a);
  if (xalf>1) return;
  D* s= S; alf(opt(a),0,0,0,0);
  if (s!=S) { P("\nResult: "); for(s++;s<=S;s++) dprint(*s); pc('\n'); }
  DEBUG(printf("\n\tstack "); prstack(); pc('\n'););
  // integrity
  DEBUG(printf(" [%% %ld ops]\n", nn); nn=0;);
  if (S<=K) { P("\n%%STACK underflow %ld\n", S-K); } // DEBUG
  if (S>=K+SMAX) { P("\n%%STACK overrun\n"); } // DEBUG
  if (!deq(K[SMAX-1],error)) { P("\n%%STACK corrupted/overrun to cons/var storage\n"); } // DEBUG
}

char* test(char r, char* s) { xp=xs=0;nv=xr=0;prog=calloc(strlen(toparse=s)+5,1);
  strcat(prog+1,s);*prog=' ';char*e=parseR(r,s,-1);
  if (!e||*e) {
    P("\n>>%s\n>%s\n\n",s,prog);
    printf("\n%%ERROR: rule %c EXPECTED \"%.10s ...\" GOT \"%.10s ...\"\n", xr, xp, xs);
    {P("\n---RULE: (expected)\n %c=",xr);char*x=R[xr];int n=1;while(x<=xp)pc((*x=='\n'?n++:0, *x++)); P("<<<WRONG HERE!\n...\n%% LINE: %d\n\n", n);}
    {P("\n---INPUT:\n");char*x=s;int n=1;while(x<=xs)pc((*x=='\n'?n++:0, *x++)); P("<<<WRONG HERE!\n...\n%% LINE: %d\n\n", n);}
  }
  if(!e||*e)P("\n\t%% %s %c->'%s'",e?(*e?"UNPARSED":"OK!"):"FAIL",r,e);
  int d=debug--;if(debug<0)debug=0;alfie(V[nv+1]);pc('\n');debug=d;return e;}

void readparser(FILE* f); // FORWARD

void oneline(char* ln, int n) { if(!ln)return; if(ln[n-1]=='\n') ln[n-1]=0;
  // TODO: why cannot keep \n when concat?
  // remove here and add if append?
  // maybe rule cannot end with \n???
  if (isupper(*ln) && ln[1]=='=') {R[last=ln[0]]=strdup(ln+2); return;}
  DEBUG(printf("%c> %s\n", rule?rule:'?',ln));

  #define Q return; case
  switch(*ln){case 0:case'#':case'\n':case'\r':;Q'<':readparser(fopen(ln+1,"r"));
  Q' ':if(ln[1]=='|') case'|':R[last]= sncat(sncat(R[last],"\n",-1),ln,-1);
  Q'=': if (isupper(ln[1])) printf("\n%c=%s\n\n", ln[1], R[ln[1]]);
    P("Free:\t");for(char c='A';c<='Z';c++)pc(R[c]?'.':c); P("\n\n");
  Q'@':alfie(ln+1);Q'*':if(ln[1])dlm=0;case '?':rule=ln[1];if(*ln=='?')dlm='\n';
      if(!rule){for(char c='A';c<='Z';c++)if(R[c])printf("%c=%s\n",c,R[c]);
	P("\n\nParse Usage:\n\t# comment\n\tR=...|foo|bar|\n\t |...\t- define rule\n\t?R \t- set rule to match\n\t*R\t- set match ALL to rule\n\t3+4*2\t- match this\n\n\t?\t- list rules\n\t=R\t- list rule R\n\t=\t- list free rules\n\t<file\t- include file\n\t-d\t- inc debug level\n\t@ 3 4+.\t- run alf\n");}return;}
  DEBUG(if (debug>1) printf("\tTEST>%s<\n", ln));
  test(rule, comments(ln)); }

void readparser(FILE* f) { char *ln= 0; size_t z= 0; int n; if (!f) return;
  while( (f!=stdin || fprintf(stderr,"%c> ", rule?rule:'?')) && (n=getdelim(&ln, &z, dlm, f))>0)
    oneline(ln, n);
  free(ln);
}

// ENDWCOUNT

int main(int argc, char** argv) {
  assert(sizeof(long)==8);
  assert(sizeof(void*)==sizeof(double));
  assert(sizeof(void*)==sizeof(long));
  assert(sizeof(void*)==sizeof(long));

  // TODO: should we could main in LOC?
  
  // alf
  initmem(16*1024); inittypes();

  // init shortnames
  for(char*s="dawin\"'({[<"; *s; s++) { char x[]={'%',*s,0}; R[*s]=strdup(x); }
  for(char*s=".,:;-_"; *s; s++) { char x[]={'\\',*s,0}; R[*s]=strdup(x); }

  // parse arguments
  while(--argc && *++argv) {
    DEBUG(printf("\n--ARG: %s\n", *argv));
    if (0==strcmp("-d", *argv)) debug++;
    else if (0==strcmp("-a", *argv)) xalf=1;
    else if (0==strcmp("-n", *argv)) xalf=2;
    else if (0==strcmp("-e", *argv)) {argv++;argc--;oneline(*argv,strlen(*argv));}
    else if (0==strcmp("-", *argv)) readparser(stdin);
    else if (**argv=='-') { fprintf(stderr, "%% Unknown option %s\n", *argv); exit(1); }
    else {DEBUG(if (debug>1) printf("\n%%FILE: %s\n", *argv));readparser(fopen(*argv, "r"));}
  }

  //readparser(stdin);
}
