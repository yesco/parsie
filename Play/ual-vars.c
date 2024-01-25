// ual  35=> 4.19s  
//      35=> 2.95s  40=> 27.81s

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

// -- cheaper:
#define Z goto next; case
// -- more expensive:
//#define Z break; case

int prog[]= {
  0,
  // == fib
  3, '0', 1,
  5, '=', 0, 1, 2,
  3, 'I', 2,
  3, '^', 0,

  3, '1', 3,
  5, '=', 0, 3, 4,
  3, 'I', 4,
  3, '^', 0,

  3, '1', 5,
  5, '-', 0, 5, 6,
  4, 'R', 6, 7,
  
  3, '2', 8,
  5, '-', 0, 8, 9,
  4, 'R', 9, 10,
  
  5, '+', 7, 10, 11,
  3, '^', 11,

  0, 0, 0, 0,
};


int* al(int* o, int* p, double* A) { int v; char* x;
  int n=0; char a[32]={0};
  //printf("AL>>> '%s'\n", p);
  int len=0;
  while(p && *p) {
  next:
    p+= len;
    //printf("--- %d '%c'", p[0], p[1]); for(int i=2; i<p[0]; i++)  printf(" %d:%.8g  ", p[i], A[p[i]]);  printf("\n");
    len= p[0];
    switch(p[1]) {
    Z' ': Z'\t': Z'\n': Z'\r':
    Z 0: return p;

    //Z'"': while(*p && *p!='"')pc(*p++);
    Z'0'...'9':
      //v=0; p--;
      //while(isdigit(*p))
      //v= v*10 + *p++-'0';
      //S[s++]= v;
      A[p[2]]= p[1]-'0';

      // 5, '+', [2], [3], [4],
    Z'+': A[p[4]]= A[p[2]] + A[p[3]];
    Z'-': A[p[4]]= A[p[2]] - A[p[3]];
    Z'*': A[p[4]]= A[p[2]] * A[p[3]];
    Z'/': A[p[4]]= A[p[2]] / A[p[3]];

    Z'=': A[p[4]]= A[p[2]] == A[p[3]];
    Z'<': A[p[4]]= A[p[2]] <  A[p[3]];

    //Z'P': P("\n%.7g ", S[--s]);
    //Z'T': P("\n");

    //Z'D': S[s]= S[s-1]; s++;
    //nZ'S': { double f= S[s-2];
      //S[s-2]= S[s-1]; S[s-1]= f; }
    Z'\\': // \ambda
      //if(*p>='a')
      //strcpy(a,parsename(&p));
      //n= *a?strlen(a):n+1;

      // hardcoded
      n=1;

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

    Z'R': al(o,o,A+p[2]);
      A[p[3]]= A[p[2]+1];

    Z'^':
      //prstack();
      //A[1]=S[s-1];s=A-S+2;
      A[1]=A[p[2]];

      // HMMM
      s=A-S+2; 

      //prstack();
      return p-1; // ret

      // If (== ?skip)
    Z'I': if(!A[p[2]]) {
	//printf("LEN=%d\n", len);
	p+= len; len= p[0];
	//printf("%d '%c'\n", len, p[1]);
	goto next;
      }
      //Z (129)...(255): p+=p[-1]-128; // JMP: +1..
    Z 128: p=o; assert(!"not done");
    //P("[TAIL]\n");
    //pc('\n'); prstack(); pc('\n');
// somehow not good? isspace
    //while(*p && isspace(*p))p++;
    //while(p[1]==' ')p++;
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
    default: p--; printf("ERROR: char '%c' %d -- \n", p[1], p[1]); exit(1);
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

int main(int argc, char** argv) {
  S[1]=atoi(argv[1]);
  al(prog+1, prog+1, S+1);
  printf("fib %.8g => %.8g\n", S[1], S[2]);

  exit(0);

  char* ln= 0; size_t l= 0;
  while(getline(&ln, &l, stdin)>=0) {
    P("\n");
    opt(ln);
    P("OPT> "); pal(ln); P("\n");
    //al(ln, ln);
  }
  free(ln);
}
