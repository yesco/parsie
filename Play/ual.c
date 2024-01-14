// minimal impl
//
// 3.69s for ./ual 10000 ...
// 4.95s for ./ua < al-tailrec.al
// -- 34% slower!
//
// +2.5s for named \a same-same 50%
//  6.23s ./ual \a 
//    5.84 with !isspace...
//    3.85 no strchr !!!!
//      almost no overhead...
//  7.62s ./al
// -- 22% slower 
// something is amiss in al.c
// also in alf.c

// fib 35
//  4.15s ./ual
//  4.70s ./au <fib-lambda.al

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define P printf
#define pc putchar

double S[1024*1024]= {0};
int s= 0;

void prstack() {
  P("\n: ");
  for(int i=0; i<s; i++)
    P("%.8g ", S[i]);
  P("\n");
}

char* parsename(char** p) { static char s[64], i; i=0; while((isalnum(**p)
  ||**p=='_') && i<sizeof(s)-1) s[i++]=*(*p)++; s[i]= 0; return s;}

char* skip(char* p){ int n=1;while(n&&*p)if(*p=='?'&&p[1]!='{')p+=2;else n+=(*p=='{')-(*p=='}'),p++;return p;}

#define Z goto next; case

char* al(char* o, char* p) { int v; char* x;
  double *A=0; int n=0; char a[32]={0};
  //printf("AL>>> '%s'\n", p);
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

    Z'=':  S[s-2]= S[s-2]==S[s-1]; s--;
    Z'<': S[s-2]= S[s-2]<S[s-1]; s--;

    Z'P': P("\n%.7g ", S[--s]);
    Z'T': P("\n");

    Z'\\': // \ambda
      if(*p>='a')
	strcpy(a,parsename(&p));
      n= *a?strlen(a):n+1;
      A=S+s-n-1;

    Z'a'...'z': // local var
      if(*a) {
	//S[s++]=A[strchr(a,p[-1])-a+1];
	char* x=a;
	while(*x && *x++!=p[-1]){};
	S[s++]=A[x-a];
      } else
	S[s++]=A[p[-1]-'a'+1];

    Z'$': prstack();

    Z'R': al(o,o);

    Z'^':
      //prstack();
      S[s-n-1]=S[s-1];s+=-n;
      //prstack();
      return p-1; // ret

    Z'I': if(S[--s]) p++; // If (== ?skip)
    Z (129)...(255): p+=p[-1]-128; // JMP: +1..
    Z 128: p=o;
    //P("[TAIL]\n");
    //pc('\n'); prstack(); pc('\n');
// somehow not good? isspace
    //while(*p && isspace(*p))p++;
    while(*p==' ')p++;
    //if (*p=='\\') while(*p && !isspace(*p))p++;
    if (*p=='\\') while(*p!=' ')p++;
      // TODO: doesn't actually improve!!!
      o=p-1; // LOL
      //printf("RECURSE: %s\n", p);
      for(int i=0; i<n; i++)
	A[i+1]=S[s-n+i];
      s-=n;
      //pc('\n'); prstack(); pc('\n');
      p++;
      // tail rec

      goto next;
    default: p--; printf("ERROR: char '%c' %d -- '%s'\n", *p, *p, p); exit(1);
    }
  }
  return p;
}

void pal(char* p) {
  while(p&&*p)
    if (*p==128) P("[TailRecurse]"),p++;
    else if (*p>128) P("[+%d]",*p++-128);
    else pc(*p++);
}

char* opt(char* p) {
  while(p&&*p) { char c;
    //P("OPT:");pal(p);P("\n");
    switch(c=*p){
    case 0: return p;
    case'"': p++;while(*p&&*p!='"')p++; break;
    case'R': if (p[1]=='^') { // tail rec
	*p= 128;
      } break;
    case'\'': p++;parsename(&p);
     
      break;
    case'}': *p=' ';return p;
    case'I':  case '{': {
      if (*p=='I')p++;
      if (*p>=128) break; // idempotent
      char*e=opt(p+1);
      // => 128...== +1...
      int x=e-p+(c=='I');
      assert(x);
      assert(128+x<=255);
      *p=128+x;
      p=e;break;}
    }

    p++;
  }
  return p?p-1:0;
}

int main() {
  char* ln= 0; size_t l= 0;
  while(getline(&ln, &l, stdin)>=0) {
    P("\n");
    opt(ln);
    P("OPT> "); pal(ln); P("\n");
    al(ln, ln);
  }
  free(ln);
}
