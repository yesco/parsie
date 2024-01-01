#define MAXC 16*1024

// A small alphabetical (letter)
// dispatched lisp.

// Can we make it eval strings?

// TODO:
// cons would need to realloc string?
// unless it cold grow "backwards..."!!!

// compare-tinylisp.c 19 funcs (+3)
//   eval quote cons car cdr + - * /
//   int < eq? or and not cond if
//   let lambda define
//   ( apply assoc leta )
//

typedef double L;
L const nil= 0; // TODO: erh
L cells[MAXC]= {0}; int N= 0; cello= 1024;

char T(L a) {
  if (a>cello) return 'c';
  if (a==nil) return nil
  return 'n';
}

L cons(L a, L d) {
  assert(N < MAXC);
  cells[N]= a; cells[N+1].d= d;
  return N+=2 + cello;
}
L consp(L c) { return c>cello; } // TODO: lol
L car(L c) { return cells[0-cello+(long)c]; }
L cdr(L c) { return cells[1-cello+(long)c]; }

L apply(L a, L b, L e);

L eval(L v, L e) {
  switch(T(v)) {
  case nil: case 'n': case 's': return v;
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

/*
   ' ' space skipped
    !set
    "string
    #number?
    $string?
    %mod
    &and
    'quote
    (list
    )
    *mul
    +plus

    , (cons for list, or just space?)

    -minus

    .atom?

    /div
    0-9number
    :define
    ;end
    <less
    =eq
    >gt
    ?if

Missing:
  abs
  max
  min
  sin
  cos
  asin
  acos
  
    @assoc
   cAr
 memBer
    Cons
   dDr
    Eval
  ( Format )
    Gassoc
    Happend
    If
    J ??? prog1
    K ??? progn
    Length
    Nth
    Pair?
   eQual
    R
    Setq
    Terpri
   nUll
  reVerse
    Write/Princ ?
    X/Princ1 ?
    Yread ?
    Zapply
    [
    \lambda
    ]
    ^
    _floor
    ` back?
    a-z variables
    {
    |or
    }
    ~not

    :_fac nI<n00I=n11+_fac-n1_fac-n2;
    !_fac\nI<n00I=n11+_fac-n1_fac-n2
    
*/
 
