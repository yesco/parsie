// Bind(ing)s
//
// (<) 2023 Jonas S Karlsson
//
// Store bindings for a lexical
// system during parsing/running

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#define SL (sizeof(long))

// simple dictionary
#define DICTSIZE 16*1024
char dict[DICTSIZE]= {0};
int dix= 0;

// register atom, set val first time
int atom(char* s, long val) {
  int i= 0;
  while(i<dix) {
    if (0==strcmp(s,dict+i))
      return i;
    i+= strlen(dict+i)+1+SL;
  }
  strcpy(dict+dix,s);
  dix+= strlen(s)+1;
  memcpy(dict+dix,&val,SL);dix+=SL;
  assert(dix<DICTSIZE);
  return i;
}

// get value for atom
long atomval(int i) {
  long r;
  memcpy(&r, dict+i+strlen(dict+i)+1,SL);
  return r;
}

char* atomstr(int i) { return dict+i; }

// bindings
#define BINDSMAX 1024
int binds[BINDSMAX]= {0}, frm[BINDSMAX]= {0}, off[BINDSMAX]= {0}, bix= 0, frame=0, locals=0;

int bindadd(int atom) {
  binds[bix]= atom;
  frm[bix]= frame;
  off[bix]= locals++;
  assert(bix<BINDSMAX);
  return bix++;
}

int bindaddn(char* s) {
  int a= atom(s, 0);
  return bindadd(a);
}

int bindframe(int f) {
  int of= frame;
  frame= f;
  locals= 0;
  return bindadd(-1);
}

void bindpop() {
  for(int i=bix-1; i>=0; i--)
    if (binds[i]==-1) {
      bix= i; frame= off[i];
      binds[i]=0; frm[i]=0; off[i]=0;
      return;
    }
  assert(!"bindpop: one to many pop:s!");
}

int bindfind(int atom) {
  for(int i=bix-1; i>=0; i--)
    if (binds[i]==atom) return i;
  // TODO: ok for lang that creates global binding by default for undef var? (then need store separate)
  //printf("bindfind: No such binding: '%s'\n", atomstr(atom));
  //exit(1);
  return -1;
}

// this is runtime information
// SLOW
int findvar(char* s) {
  int a= atom(s, 0);
  int i= bindfind(a);
  assert(i);
  int f= frm[i];
  int o= off[i];
  return f+o;
}

// ENDWCOUNT
void F(char* s) {
  int a= atom(s, 0);
  int i= bindfind(a);
  if (i>=0)
    printf("%2d '%s'\t%3d %2d %2d\n", i, atomstr(a), binds[i], frm[i], off[i]);
  else
    printf("%2d '%s' - not found!\n", i, atomstr(a));
}

int main(int argc, char**argv) {
  if (1) { // test dictionary
    int nl= atom("null", 0);
    int foo= atom("foo", 14);
    int bar= atom("bar", 42);
    printf("null=%d foo=%d bar=%d\n", nl, foo, bar);
    printf("names: %s %s %s\n", atomstr(nl), atomstr(foo), atomstr(bar));
    {
      long vnl= atomval(nl);
      long vfoo= atomval(foo);
      long vbar= atomval(bar);
      printf("NULL=%ld foo=%ld bar=%ld\n", vnl, vfoo, vbar);
    }

    int a= atom("abba", 0);
    int b= atom("bubba", 0);
    int c= atom("circ", 0);
    
    // binds
    printf("\n--binds\n");

    int fg= bindframe(7);
    int ga= bindadd(a);
    int gb= bindadd(b);
    printf("fg=%d ga=%d gb=%d\n", fg, ga, gb);
    F("abba"); F("bubba"); F("circ"); printf("\n");

    int fn= bindframe(11);
    int nb= bindadd(b);
    int nc= bindadd(c);
    F("abba"); F("bubba"); F("circ"); printf("\n");

    int fs= bindframe(17);
    int sa= bindadd(a);
    int sc= bindadd(c);
    F("abba"); F("bubba"); F("circ"); printf("\n");
    
    printf("<<<<<\n");
    bindpop();
    F("abba"); F("bubba"); F("circ"); printf("\n");

    bindpop();
    F("abba"); F("bubba"); F("circ"); printf("\n");

    bindpop();
    F("abba"); F("bubba"); F("circ"); printf("\n");

    exit(3);
  }
}
