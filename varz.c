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
void glbladd(char* s) { int a=nameadd(s);
  while((H-M)%8 || H>M==0)H++; // align
  data* p=AVPTR(atom(s));
  p->u= H-M;
  //printf("== glbladd: %s @ %ld\n", s, H-M);
  printf(" >>> u= %ld\n", p->u);

  // BUG:
  // TODO: if defined sevea times it allocates more! lol
  *(data*)H= (data){.d=4711.42};
  H+= 8;
  //prnames();
}

void bindenter(int o) { int r= _varadd(o, fr, 0); fr= o; loc= 0; }
int bindfindid(int a) {int i=vix; while(i--)if(V[i].a==a)return i;return -1;}
int bindfind(char* s) { int i=bindfindid(nameadd(s));
  //printf("== bindfind: %s @ %d\n", s, i);
  if (i>0) return i;
  else {
    data* p= AVPTR(atom(s));
    //printf(" >>> u= %ld\n", p->u);
    int i= p->u;
    //printf("  == GLOBFIND: %s @ u=%ld => %d\n", s, p->u, i);

    // TODO: it's 0 if not found!
    
    // TODO: why is it negative???
    return -i-1;
  }
}
void bindadd(char* s) { if(!fr)glbladd(s);else _varadd(fr, ++loc, nameadd(s));}
void bindexit() { int i= bindfindid(nilo); fr= V[i].f; vix= i; loc= V[i].l; }

// ENDWCOUNT

#ifdef varzTEST

void F(char* s) {
  int i= bindfind(s);
  if (i<0)
    printf("'%s' - not found! %d\n", s, i);
  else
    printf("'%s'\t%3d %2d\n", hp+V[i].a+1, V[i].f, V[i].l);
}

int main() {
  if (1) {
    THIS GIVES GARBAG now???
    inittypes();
    
    // binds
    printf("\n--binds\n");

    bindenter(7);
    bindfind("abba");
    bindfind("bubba");
    F("abba"); F("bubba"); F("circ"); printf("\n");

    bindenter(11);
    bindfind("burka");
    bindfind("circ");
    F("abba"); F("bubba"); F("circ"); printf("\n");

    bindenter(17);
    bindfind("abba");
    bindfind("circ");
    F("abba"); F("bubba"); F("circ"); printf("\n");
    
    printf("<<<<<\n");
    bindexit();
    F("abba"); F("bubba"); F("circ"); printf("\n");

    bindexit();
    F("abba"); F("bubba"); F("circ"); printf("\n");

    bindexit();
    F("abba"); F("bubba"); F("circ"); printf("\n");

    exit(3);
  }
}

#endif
