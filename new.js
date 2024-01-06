var o= {sum: 0};

o.add= function F(a,b) {
    var r= a+b;
    this.sum += r;
    return r;
}

console.log(o);

console.log(o.add(11, 22));

console.log(o.add(1100, 2200));

console.log(o.sum);
