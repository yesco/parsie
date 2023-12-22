N=%n
V=var N; ::$1
U=N [$:1]
X=V [$1] :E [ <enter>{ ] V [$2] U U [  <global>foo=$3 <local>bar=$4  ] V [$5] U U [  <local>foo=$6 <local>bar=$7  ] :X [ <exit>} ] U [  <global>foo=$8]
?X
var foo; var bar; foo bar var foo; foo bar foo



