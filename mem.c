#define SMAX 1024
double K[SMAX]={0}; int sp=0,memsize=0; char *M=0, *H=0, *F['Z'-'A'+1]={0};

#define T K[sp-1]
#define POP K[--sp]
#define U K[sp++]

#define L (long)
#define SL (sizeof L)
#define P printf
#define align() while((H-M)%SL)H++

void initmem(size_t sz) { H=1024+(M=calloc(memsize= sz, 1)); } // TODO: ?zero S F
