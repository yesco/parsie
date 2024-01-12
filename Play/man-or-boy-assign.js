// pascal style assign result
function A(k, x1, x2, x3, x4, x5) {
  var a;
  function B() {
    var b;
    k -= 1;
    a= b= A(k, B, x1, x2, x3, x4);
    return b;
  }
  if (k <= 0) a= x4()+x5(); else B();
  return a;
}

// this uses lambda wrappers around the numeric arguments
function X(n) {
  return function () {
    return n;
  };
}
console.log(A(10,X(1),X(-1),X(-1), X(1), X(0)));
