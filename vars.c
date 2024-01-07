not used,  now using svar.c

// vars
//
// (<) 2023 Jonas S Karlsson
//
// Store bindings for a lexical
// system during parsing/running

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <assert.h>

// simple dictionary
#define DICTSIZE 16*1024
char dict[DICTSIZE]= {0};
int dix= DICTSIZE, frame=0, local=0;

// register name
typedef struct Var {
  int f, l;
  char s[1];
} Var;

#define OFS offsetof(Var, s)
int varad(int f, int l, char* s) { 
  Var v= {f, l, 0};
  dix-= strlen(s)+OFS+1;
  memcpy(dict+dix, &v, OFS);
  strcpy(dict+dix+OFS, s);
  return dix;
}  

int varadd(char* s) { 
  return varad(frame, ++local, s);
}

int globaladd(char* s) {
  assert(!"globaladd: not yet implemented");
}

int enterframe(int o) { int r= varad(o, frame, "--frame--"); frame= o; local= 0; return r;
}  

// TODO: does it need to be aligned?
Var* var(int ix) { return (Var*)(dict+ix); }

Var* varfind(char* s) { int ix= dix;
  while(ix<DICTSIZE) { Var* v= var(ix);
    if (0==strcmp(v->s, s)) return(Var*)(dict+ix); ix+= OFS+strlen(v->s)+1;
  }
  return NULL;
}

void exitframe() { Var* v= varfind("--frame--");
  frame= v->l; dix+= OFS+strlen(v->s)+1; local= var(dix)->l;
}
	     
// ENDWCOUNT
void F(char* s) {
  Var* v= varfind(s);
  if (v)
    printf("'%s'\t%3d %2d\n", v->s, v->f, v->l);
  else
    printf("'%s' - not found!\n", s);
}

int main(int argc, char**argv) {
  if (0) {

    frame=42; local=99;

    varadd("foo");
    { Var* v= varfind("foo"); if (v) printf("frame=%d local=%d '%s'\n", v->f, v->l, v->s); }

    varadd("abba");
    { Var* v= varfind("foo"); if (v) printf("frame=%d local=%d '%s'\n", v->f, v->l, v->s); }

    enterframe(200);
    
    varadd("gurka");
    { Var* v= varfind("foo"); if (v) printf("frame=%d local=%d '%s'\n", v->f, v->l, v->s); }

    varadd("foo");
    { Var* v= varfind("foo"); if (v) printf("frame=%d local=%d '%s'\n", v->f, v->l, v->s); }

    { Var* v= varfind("abba"); if (v) printf("frame=%d local=%d '%s'\n", v->f, v->l, v->s); }

    { Var* v= varfind("gurka"); if (v) printf("frame=%d local=%d '%s'\n", v->f, v->l, v->s); }

    exitframe();
    { Var* v= varfind("foo"); if (v) printf("frame=%d local=%d '%s'\n", v->f, v->l, v->s); }

    exit(1);
  }

  if (1) {
    // binds
    printf("\n--binds\n");

    enterframe(7);
    varadd("abba");
    varadd("bubba");
    F("abba"); F("bubba"); F("circ"); printf("\n");

    enterframe(11);
    varadd("burka");
    varadd("circ");
    F("abba"); F("bubba"); F("circ"); printf("\n");

    enterframe(17);
    varadd("abba");
    varadd("circ");
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
