# Usage: ./c foo.JS
#
# will load JS.bnf and then compile and run foo.JS

#echo "ARGV: $1"
fil=$1
bnf=${1/*./}.bnf
#echo "BNF $bnf"
(cat $bnf ; echo "*P" ; cat $fil) | ./parse | tail -1 > .alf
./alf <.alf




