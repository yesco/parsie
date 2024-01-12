a={ q: 35 };
b={__proto__: a};
console.log('a', a);
console.log('b', b);
b.q+= 1000;
console.log('a', a);
console.log('b', b);
