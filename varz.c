#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <assert.h>

#include "nantypes.c"

// register name
#define MAXVAR 1024
int fr=0,loc=0,vix= 0; struct var { int a,f,l; } V[MAXVAR]={0};

int _varadd(int f, int l, int a) { V[vix]= (struct var){a,f,l}; return vix++; }

int varadd(int a) { return _varadd(fr, ++loc, a); }

int globaladd(char* s) {
  assert(!"globaladd: not yet implemented");
}

#define HPSIZE 16*1024
char hp[HPSIZE]= {0};

int bindenter(int o) { int r= _varadd(o, fr, 0); fr= o; loc= 0; return r; }

int nilo; data nil;

void initvarz() { nil= atom("nil",hp,HPSIZE); assert(DAT(nil)==nilo);}

int bindfindid(int a) {int i=vix; while(i--)if(V[i].a==a)return i; return -1;}
int bindfind(char* s) { return bindfindid(nameadd(s,hp,-1)); }
int bindadd(char* s) { return varadd(nameadd(s,hp,HPSIZE)); }
void bindexit() { int i= bindfindid(nilo); fr= V[i].f; vix= i; loc= V[i].l; }

// ENDWCOUNT

#ifdef varzTEST

void F(char* s) {
  int i= varfinds(s);
  if (i<0)
    printf("'%s' - not found!\n", s);
  else
    printf("'%s'\t%3d %2d\n", hp+V[i].a+1, V[i].f, V[i].l);
}

int main() {
  if (1) {
    initvarz();
    
    // binds
    printf("\n--binds\n");

    enterframe(7);
    varadds("abba");
    varadds("bubba");
    F("abba"); F("bubba"); F("circ"); printf("\n");

    enterframe(11);
    varadds("burka");
    varadds("circ");
    F("abba"); F("bubba"); F("circ"); printf("\n");

    enterframe(17);
    varadds("abba");
    varadds("circ");
    F("abba"); F("bubba"); F("circ"); printf("\n");
    
    printf("<<<<<\n");
    exitframe();
    F("abba"); F("bubba"); F("circ"); printf("\n");

    exitframe();
    F("abba"); F("bubba"); F("circ"); printf("\n");

    exitframe();
    F("abba"); F("bubba"); F("circ"); printf("\n");

    exit(3);
  }
}

#endif
