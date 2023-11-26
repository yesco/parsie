#define MAXC 16*1024

// A small alphabetical (letter)
// dispatched lisp.

// Can we make it eval strings?

// compare-tinylisp.c 19 funcs (+3)
//   eval quote cons car cdr + - * /
//   int < eq? or and not cond if
//   let lambda define
//   ( apply assoc leta )
//

typedef double L;
L nul= 0; // TODO: erh
typedef struct C { L a, L d; } C;
C cells[MAXC]= {0}, N= 0;

L apply(L a, L b, L e);

L cons(L a, L d) {
  assert(N < MAXC);
  cells[a].a= a; cells[a].d= d;
  return N++ + 1024;
}

L eval(L v, L e) {
  switch(T(v)) {
  case 0: case 'n': case 's': return v;
  case 'a': return find(v, e);
  case 'c': return apply(car(v), cdr(v), e);
  }
}

L nullp(L v) {
  return eq(car(b), nul);
}

L apply(L a, L b, L e) {
  L f= car(b);
  switch(T(a)) {
  'n': return aref(b);
  'c': return apply(eval(a, e), b, e);
  'a':
    // no-eval
    switch(A1(a)) {
    case '?': return N(eval(f))?eval(car(cdr(cdr(b))), e):eval(car(cdr(b)), e);
    case 'C': return N(eval(f))?eval(car(cdr(cdr(b))), e):apply(f, cdr(cdr(b)), e);
    }

    // eval
    b= evlist(b);
    switch(A1(a)){
    case 'c': return cons(f, car(cdr(b)));
    case 'a': return car(f);
    case 'd': return cdr(f);
    case 'n': return nul;

    case 'l': return b; // list
    case 'q': return car(b); // quote

    case 'm': return N(b)?b:cons(apply(f, car(b), e), apply(a, cdr(b), e));
    case 'A': return eq(f, car(car(b)))?cdr(car(b)):apply(a, cdr(b), e);
    case 'M': return eq(f, car(b))?car(cdr(b)):apply*a, cdr(cdr(b));
	
    case 's': return T(f)=='a'?f:nul;
    case '.': return T(f)=='c'?f:nul;
    case '#': return T(f)=='n'?f:nul;

    case '!': return N(f);
    case '=': return eq(f, car(cdr(b)));
    case '<': return lt(f, car(cdr(b)));

    case '+': return N(f)?0:f+apply(a, cdr(b), e);
    case '-': return N(f)?0:f-apply(a, cdr(b), e); // TODO: hmmm
    case '*': return N(f)?1:f*apply(a, cdr(b), e);
    case '/': return N(f)?1:f/apply(a, cdr(b), e);
    case 'I': return f+1;
    case 'D': return f-1;

    case 'p': return princ(f), f;
    case 'q': return prin1(f), f;
    case 't': return printf("\n"), nul; // terpri

    default: printf("Unknown: '%c'\n", A1(a)); exit(77);
    }
  }

