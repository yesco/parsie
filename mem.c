#include <math.h>

const long SMAX=16*1024L,VMAX=1024*1024L,CMAX=VMAX*3,MLIM=1E9L,KSZ=SMAX+VMAX+CMAX;

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
// C - Current "here"
//
// K= 0 .. SMAX-1      =  stack
//    SMAX+2 .. VMAX   =  globals
//    VMAX .. CMAX     =  conses
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
double *K,*S,*Y,*C; long mmax=0; char *M=0, *H=0, *F['Z'-'A'+1]={0};

#define POP *S--
#define U *++S

#define L (long)
#define UL unsigned long
#define SL (sizeof L)

#define align() while((H-M)%SL)H++

#define P printf
#define pc(a) putchar(a)

extern char* dchars(double); // FORWARD

// Interpreation of memory ref
//  -SMAX ..  0  = is stack frame ref
//  0 .. VMAX-1  = D globals
//  DLIM .. +=DSZ= D heap // TODO:
//   ...
//  MLIM ..      = is general M char heap

// X=fixed addr    <X>-moving up/down;
// (@offset)

// /stack\ /locs\ /globs\       /Dcells\
// ^^^^^^^ ^^^^^^ ^^^^^^^       ^^^^^^^^
// K-<S>-<Y>-(SMAX)-<C>-(SMAX+VMAX)-(KSZ)

extern long atomaddr(double); // FORWARD
void* m(double d, double* A, int n) {long x=L d;if(isnan(d))return m(atomaddr(d),A,n);
  return x<0?A-x-1:(x>=MLIM?M+x-MLIM:x<KSZ-SMAX?(void*)(K+SMAX+x):0); }

// TODO: hmmm, not so useful for debug?
//void w(void* p,double v){if(p&&(p>=(void*)K&&p<(void*)(K+KSZ))||(p>=(void*)M&&p<(void*)(M+mmax)))*(D*)p=v;
//  else{printf("\nERROR: w() to address outof bounds: %p", p);}


void initmem(size_t sz) { K=calloc(KSZ,SL);S=K+1; C=K+SMAX;Y=C-2;
  M=calloc(mmax=sz,1);H=M+1024; }

// ENDWCOUNT

extern int dprint(double); // FORWARD

void dump() { char* M= (void*)C;
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
      if (K+a==C) P("\n   ---C"),n=1;
      if (K+a==K+VMAX) P("\n   ------cells"),n=1;
      if (K+a==K+KSZ-1) {P("\n   ------END\n"),n=1;return; }

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
    // just return to continue
  } while(getline(&e,&z,stdin)); 
}

