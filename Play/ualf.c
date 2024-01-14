#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

double S[1024]= {0};
int s= 0;

char* skip(char* p){ int n=1;while(n&&*p)if(*p=='?'&&p[1]!='{')p+=2;else n+=(*p=='{')-(*p=='}'),p++;return p;}

// TODAY?:  break is faster!!! LOL WTF
#define Z goto next; case
// HOWEVER: doesn't work for nested switches... lol
//#define Z break; case

char* alf(char* p) { int v; char* x;
  //printf("ALF>>> '%s'\n", p);
  while(p && *p) {
  next:
    //printf("--- %c\n", *p);
    switch(*p++) {
    Z' ': Z'\t': Z'\n': Z'\r':
    Z 0: return p;
    Z'0'...'9': v=0; p--;
      while(isdigit(*p))
	v= v*10 + *p++-'0';
      S[s++]= v;

    Z'+': S[s-2]+= S[s-1]; s--;
    Z'-': S[s-2]-= S[s-1]; s--;
    Z'*': S[s-2]*= S[s-1]; s--;
    Z'/': S[s-2]/= S[s-1]; s--;

    Z'<': S[s-2]= S[s-2]<S[s-1]; s--; goto next;

    Z'd': S[s]= S[s-1]; s++;
    Z'\\': s--;

    Z'.': printf("%.7g ", S[--s]);

    Z'}':
      //printf("<<<< } '%s'\n", p);
      return p;
    Z'{': while((/*printf("LOOP\n"),*/alf(p))){};
      //printf("SKIPPING\n");
      p= skip(p);
    Z'?': if (!S[--s]) {
	//printf("FALSE\n");
	switch(*p++) {
	case '}': return p;
	case ']': return 0;
	default: assert(!"not come here");
	}
      } else /*printf("TRUE\n"),*/p++;

      goto next;
    default: p--; printf("ERROR: char '%c' %d -- '%s'\n", *p, *p, p); exit(1);
    }
  }
  return p;
}

int main() {
  char* ln= 0; size_t l= 0;
  while(getline(&ln, &l, stdin)>=0)
    alf(ln);
  free(ln);
}
