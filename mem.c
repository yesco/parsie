#define SMAX 1024
double K[SMAX]={0},*S=K; int memsize=0; char *M=0, *H=0, *F['Z'-'A'+1]={0};

#define POP *S--
#define U *++S

#define L (long)
#define SL (sizeof L)
#define P printf
#define align() while((H-M)%SL)H++

void initmem(size_t sz) { H=1024+(M=calloc(memsize= sz, 1)); } // TODO: ?zero S F
