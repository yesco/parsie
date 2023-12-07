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
  int frame, local;
  char name[1];
} Var;


int varad(int f, int l, char* s) { 
  Var v= {f, l, 0};
  dix-= strlen(s)+offsetof(Var, name)+1;
  memcpy(dict+dix, &v, offsetof(Var, name));
  strcpy(dict+dix+offsetof(Var, name), s);
  return dix;
}  

int varadd(char* s) { 
  return varad(frame, ++local, s);
}

int globaladd(char* s) {
  assert(!"globaladd: not yet implemented");
}

int enterframe(int pos) {
  int r= varad(pos, frame, "--frame--");
  frame= pos;
  local= 0;
  return r;
}  

Var* var(int ix) {
  // TODO: does it need to be aligned?
  return (Var*)(dict+ix);
}

Var* varfind(char* s) {
  int ix= dix;
  while(ix<DICTSIZE) {
    Var* v= var(ix);
    if (0==strcmp(v->name, s)) return(Var*)(dict+ix);
    ix+= offsetof(Var, name)+strlen(v->name)+1;
  }
  return NULL;
}

void exitframe() {
  Var* v= varfind("--frame--");
  frame= v->local;
  dix+= offsetof(Var, name)+strlen(v->name)+1;
  local= var(dix)->local;
}
	     
// ENDWCOUNT
void F(char* s) {
  Var* v= varfind(s);
  if (v)
    printf("'%s'\t%3d %2d\n", v->name, v->frame, v->local);
  else
    printf("'%s' - not found!\n", s);
}

int main(int argc, char**argv) {
  if (0) {

    frame=42; local=99;

    varadd("foo");
    { Var* v= varfind("foo"); if (v) printf("frame=%d local=%d '%s'\n", v->frame, v->local, v->name); }

    varadd("abba");
    { Var* v= varfind("foo"); if (v) printf("frame=%d local=%d '%s'\n", v->frame, v->local, v->name); }

    enterframe(200);
    
    varadd("gurka");
    { Var* v= varfind("foo"); if (v) printf("frame=%d local=%d '%s'\n", v->frame, v->local, v->name); }

    varadd("foo");
    { Var* v= varfind("foo"); if (v) printf("frame=%d local=%d '%s'\n", v->frame, v->local, v->name); }

    { Var* v= varfind("abba"); if (v) printf("frame=%d local=%d '%s'\n", v->frame, v->local, v->name); }

    { Var* v= varfind("gurka"); if (v) printf("frame=%d local=%d '%s'\n", v->frame, v->local, v->name); }

    exitframe();
    { Var* v= varfind("foo"); if (v) printf("frame=%d local=%d '%s'\n", v->frame, v->local, v->name); }

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
