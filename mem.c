#define SMAX 1024
double S[SMAX]={0}; int sp=0,memsize=0; char *M=0, *H=0, *F['Z'-'A'+1]={0};

#define T S[sp-1]
#define POP S[--sp]
#define U S[sp++]

#define L (long)
#define SL (sizeof L)
#define P printf
#define align() while((H-M)%SL||!(H-M))H++

void initmem(size_t sz) { H=M=calloc(memsize= sz, 1); } // TODO: ?zero S F
