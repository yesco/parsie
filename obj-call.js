// Object.call exammple
//
// From - https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Function/call

function Product(name, price) {
  this.name = name;
  this.price = price;
}

function Food(name, price) {
  Product.call(this, name, price);
  this.category = 'food';
}

console.log(new Food('cheese', 5).name);
// Expected output: "cheese"
