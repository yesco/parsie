#include <math.h>
#include <assert.h>

#ifndef DEBUG
int debug= 0; // DEBUG
#define DEBUG(D) if (debug) do{D;}while(0);
#endif // DEBUG

const long SMAX=16*1024L,GMAX=1024*1024L,CMAX=GMAX*1600,MLIM=1E12L,KSZ=SMAX+GMAX+CMAX;

// -- Memory Layout
// We use 3 different memory regiops:
//
//   K = D-store
//   M = memory char store / heap
//   hp= "atom heap"
// 
// K - array of D values
//     (stack+play+globals)
// S - the current stack pointer *S=top
// G - Global "here"
// C - Cells "here"
//
// K= 0 .. SMAX-1      =  stack
//    SMAX+2 .. GMAX-1 =  globals
//    GMAX .. CMAX-1   =  conses
//
// (as address for ! and @: Moffset+MLIM)
// See:m() below
//
// 
// M= 0 ..
//

// K[SMAX+1] - TODO: free list (?)
//   or keep separate so not overwrite?

// Safety:
//   K[0] = error; // detect underrun
//   K[SMAX] = error; // detect overrun
//
double *K,*S,*Y,*G,*C; long mmax=0; char *M=0, *H=0, *F['Z'-'A'+1]={0};

#define POP *S--
#define U *++S
#define T (*S)
#define S1 S[1]

#define L (long)
#define UL unsigned long
#define SL (sizeof L)

// TODO: remove?, not really used
#define align() while((H-M)%SL)H++

#define RET return
#define P printf
#define pc(a) putchar(a)

// Interpreation of memory ref
//  -SMAX ..  0     = stk frm ref
//  0...G...GMAX-1  = D globals
//  GMAX...C...CMAX = D heap
//
//  MLIM ..      = is general M char heap

// X=fixed addr    <X>-moving up/down;
// (@offset)

// /stack\ /locs\ /globs\       /Dcells\
// ^^^^^^^ ^^^^^^ ^^^^^^^       ^^^^^^^^
// K-<S>-<Y>-(SMAX)-<G>-(G)-(@GMAX)-<C>-(KSZ)

extern long atomaddr(double); // FORWARD
void* m(double d, double* A, int n) {long x=L d;if(isnan(d))RET m(atomaddr(d),A,n);
  RET x<0?A-x-1:(x>=MLIM?M+x-MLIM:x<KSZ-SMAX?(void*)(K+SMAX+x):0); }

// TODO: hmmm, not so useful for debug?
//void w(void* p,double v){if(p&&(p>=(void*)K&&p<(void*)(K+KSZ))||(p>=(void*)M&&p<(void*)(M+mmax)))*(D*)p=v;
//  else{printf("\nERROR: w() to address outof bounds: %p", p);}


void initmem(size_t sz) { K=calloc(KSZ,SL);S=K+1; G=K+SMAX;Y=G-2; C=K+GMAX;
  assert(KSZ<MLIM);
  M=calloc(mmax=sz,1);H=M+1024; }

// ENDWCOUNT

extern int dprint(double); // FORWARD

void prstack(){P("\t:");for(double* s= K+2; s<=S; s++){dprint(*s);pc(' ');}} // DEBUG

void dump() { char* M= (void*)G;
  for(int i=0; i<*S;) { int n= S[-1]; P("\n%04x ", n);
    for(int j=0;j<8;j++) P("%02x%*s", M[n+j], j==3, "");  P("  ");
    for(int j=0;j<8;j++) P("%c", M[n+j]?(M[n+j]<32||M[n+j]>126?'?':M[n+j]):'.');
    double d= *(double*)(M+n);
    //x=TYP(d); if ((x&0x0ff8)==0x0ff8) {
    //if (x>32*1024) x=-(x&7); else x= x&7; } else x=0; P(" %2ld:", x);
    dprint(d);S[-1]+=8;i+=8;} P("\n");extern void prstack(); prstack(); }

void prK() {
  char*e=0; size_t z=0; long n=0, a=0;
  do { int p=0;
    for(int p=0; p<32;) {
      if (K+a==K) P("\n   ------K"),n=1;
      if (K+a==S+1) P("\n   ^--S"),n=1;
      if (K+a==Y) P("\n   v--Y"),n=1;
      if (K+a==K+SMAX) P("\n   ------globals"),n=1;
      if (K+a==G) P("\n   ---G"),n=1;
      if (K+a==C) P("\n   ---C"),n=1;
      if (K+a==K+GMAX) P("\n   ------cells"),n=1;
      if (K+a==K+KSZ-1) {P("\n   ------END\n"),n=1;RET; }

      double d= K[a]; // DEBUG
      if (!n && d==0 && !K[a+1]) { a++;  continue; }

      p++;
      if (a%2==0) P("\n   %5ld : ", a);
      n++;

      P("%*s", 10-dprint(d), " ");

      if (d==0 && !K[a+1]) { n=0; }
      a++;
    }
    fputc('>', stderr);
    // just RET to continue
  } while(getline(&e,&z,stdin)); 
}

