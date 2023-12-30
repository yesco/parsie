const long SMAX=1024L,VMAX=1024*1024L,MLIM=1E9L,KSZ=SMAX+VMAX;
// S - the current stack pointer *S=TOP
// K - array of D values (incl globals)
// C - current C/H 
// K[SMAX+1] - TODO: free list (?)

// Safety:
//   K[0] = error; // detect underrun
//   K[SMAX] = error; // detect overrun
double *K,*S,*C; long mmax=0; char *M=0, *H=0, *F['Z'-'A'+1]={0};

#define POP *S--
#define U *++S

#define L (long)
#define SL (sizeof L)
#define P printf
#define align() while((H-M)%SL)H++

void initmem(size_t sz) { S=1+(K=calloc(KSZ, SL)); C=K+SMAX+2; H=1024+(M=calloc(mmax= sz, 1)); }
