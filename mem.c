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
// S - the current stack pointer *S=TOP
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
double *K,*S,*C; long mmax=0; char *M=0, *H=0, *F['Z'-'A'+1]={0};

#define POP *S--
#define U *++S

#define L (long)
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
void* m(double d, double* A, int n) {long x=L d;if(isnan(d))return dchars(d);
  return x<0?A-x-1:(x>MLIM?M+x-MLIM:x<KSZ-SMAX?(void*)(K+SMAX+x):0); }

void initmem(size_t sz) { S=1+(K=calloc(KSZ, SL)); C=K+SMAX; H=1024+(M=calloc(mmax= sz, 1)); }
