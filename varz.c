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
void varadd(int a) { _varadd(fr, ++loc, a); }
int globaladd(char* s) { return nameadd(s); }

void bindenter(int o) { int r= _varadd(o, fr, 0); fr= o; loc= 0; }
int bindfindid(int a) {int i=vix; while(i--)if(V[i].a==a)return i; return -1;}
int bindfind(char* s) { return bindfindid(nameadd(s)); }
void bindadd(char* s) { return varadd(nameadd(s)); }
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
