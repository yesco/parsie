var x=12345678901234567890;
function F(n) {
    //x=x+1; // 10.5s instead of 6.83
    if (n==0) return 0;
    else if (n==1) return 1;
    else return F(n-1)+F(n-2);
}

console.log("START");
console.log(x);
x=0;
console.log(F(35));
console.log(x);
console.log('END');

