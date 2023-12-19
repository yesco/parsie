//80------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <assert.h>

// TODO: remove
#ifdef nantypesTEST // DEBUG
#include "nantypes.c" // DEBUG
#endif // DEBUG

// register name
int fr=0,loc=0,vix= 0; struct var { int a,f,l; } V[1024]={0};

int _varadd(int f, int l, int a) { V[vix]= (struct var){a,f,l}; return vix++; }

void glbladd(char* s){int a=nameadd(s);align();*(D*)AVPTR(atom(s))=H-M;H+=8;}

void bindenter(int o) { int r= _varadd(o, fr, 0); fr= o; loc= 0; }
int bindfindid(int a) { int i= vix;while(i--)if(V[i].a==a)return i; return -1;}
int bindfind(char* s) { int i=bindfindid(nameadd(s)); if (i>0) return i;
  //printf("== bindfind: %s @ %d\n", s, i);
  D* p= AVPTR(atom(s)); unsigned long l= d2u(*p);
  DEBUG(printf(" >>> u= %ld\n", l)); return -i-1; // TODO: why is it negative??? because -bindfind is the call!!
  //printf("  == GLOBFIND: %s @ u=%ld => %d\n", s, p->u, i);
  // TODO: it's 0 if not found!
}
void bindadd(char* s) { if(!fr)glbladd(s); else _varadd(fr,++loc,nameadd(s));}
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
    bindadd("abba");
    bindadd("bubba");
    F("abba"); F("bubba"); F("circ"); printf("\n");

    bindenter(11);
    bindadd("burka");
    bindadd("circ");
    F("abba"); F("bubba"); F("circ"); printf("\n");

    bindenter(17);
    bindadd("abba");
    bindadd("circ");
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
