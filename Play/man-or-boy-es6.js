var x = n => () => n;

var a = (k, x1, x2, x3, x4, x5) => {
  var b = () => a(--k, b, x1, x2, x3, x4); //decrement k before use
  return (k > 0) ? b() : x4() + x5();
};

console.log(a(10,x(1),x(-1),x(-1),x(1),x(0)));
