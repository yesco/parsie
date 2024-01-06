const long SMAX=1024L,VMAX=1024*1024L,MLIM=1E9L,KSZ=SMAX+VMAX;

// -- Memory
// We use 3 different memory regiops:
//
//   K = D-store
//   M = memory char store / heap
//   hp= "atom heap"
// 
// K - array of D values
//     (stack+play+globals)
// S - the current stack pointer *S=TOP
// C - current C
//
// K= 0 .. SMAX-1      =  stack
//    SMAX+2 .. VMAX   =  globals
//    VMAX .. CMAX     =  conses
//
// (as address for ! and @: Moffset+MLIM)
// See:m() in alf.c
//
// 
// M= 0 ..
//

// K[SMAX+1] - TODO: free list (?)
//   or keep separate so not overwrite?

// Safety:
//   K[0] = error; // detect underrun
//   K[SMAX] = error; // detect overrun
double *K,*S,*C; long mmax=0; char *M=0, *H=0, *F['Z'-'A'+1]={0};

#define POP *S--
#define U *++S

#define L (long)
#define SL (sizeof L)

#define align() while((H-M)%SL)H++

#define P printf
#define pc putchar


void initmem(size_t sz) { S=1+(K=calloc(KSZ, SL)); C=K+SMAX+2; H=1024+(M=calloc(mmax= sz, 1)); }
