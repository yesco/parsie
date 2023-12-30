# Parsie And ALF

A Minimalist Parser and Compiler/code Generator with built-in VM Interpeter.

## Parser

The Parser uses a simple BNF-described grammar. It employes a recursive descent BNF-interpreter. Each rule use a single Capital character as it's name. If you want to match a capital it needs to be backquoted

    E=  ...expression...
    S=  ...statement..
    T= integer|string|real
    V= var %n: T = E; V | var %n:T; V |
    B= begin *V *S end
    P= procedure %n; B.
    S= \Foo\Bar

It'll backtrack when it doesn't match and will seek the next alternative "|" in the rule.

If a rule ends with "|" it allows match  with nothing, still satiesfying the rule. This is useful for doing list matching.

    L=foo,L|bar,L|L
    ?L
    foo,bar,bar,foo

### Input format
    # comment
    A=foo|bar|X         Any capital refs a rule
                        (X empty match "")
    ?A                  match rule A
    foo
    barf                leftover: f
    fie                 leftover: fie

    X=fieX|fumX|        Right recursion ok
    foo                 (still matching A)
    fum
    fiefumfie

    *A                  match A to rest of file
     fie                spaces ignored
    fum f i e           match> fie\nfum f i e<
    
### character classes
    %a = match alphabet and _
    %d = match digit
    %e = match end of text
    %w = match word char (alphanum _)
    %i = integer
    %n = identifier (%a%w...)

### string matcher
    ($1 always gives unquoted)
    (default is verbatim)

    %" = "foo\"bar"
    %' = 'c' or 'foo\'bar'
       (not handling nesting:???)
    %( = ( fo( oooo ( \) foo bar)
    %[ = [ fo[ oooo [ \] foo bar]
    %{ = { fo{ oooo { \} foo bar}
    %< = < fo< oooo < \> foo bar>
    %s = START fo? START \END foo bar END
	   handle any style string

### Regexp style
PREFIX regexp chars ? + * match optional, at least one, any number. This is greedy matching.

These don't work well with capturing; these may be removed as they are redundant with right-recursive rules.

    //   ?R = optionall match rule R
    //   +R = match rule R once or more
    //   *R = match rule R 0 or more times
    //   (R must be a NonTerminal)

## Generating results

The output generated by a rule is by default all text matched, but can be controlled by explicitly defining it per parse alternative:

   > B=a [1] | b [2] | c [3]
   > C=BC [$1+$1] | [0]
   > ?C
   > abc
   1+2+3+0

Note: Results generated are cleared on eacch succesive "|"-or-branch. The generated results can be several in one result.

   > C=B [$1] [+] C [$2] | [9]

Basically, the results are concatenated for the successful branch.

## Attributes (PEG?)

It is possible to set one character attributes, these are also cleared at each branch. They are searched from *ANY* resulting rules, these may be useful to carry addition type information about.

   > T= int :t=i :z=8 | char :t=c :z=1
   > Z= sizeof\(T\) [$z]
   > ?Z
   > sizeof(int)
   8
   > sizeof(char)
   1
   
NOTE: imlemented but not yet used.

## Actions

All these parsing functionality so far mentioned has been purely translational. Now, to handle scope, bindings, and variables in a sensible manner, we introduce *actions*. These are *NOT* retractable during parsing - even partial matches will run the rules and does not (currently) reset at choice of a different OR-branch that matches.

   ::$1   (if toplevel define global "$1")
   :E     (enter local scope)
   ::$1     (create new local "$1")
   :=$1   (local if in scope, else global)
   :X     (exit local scope)

NOTE: the text after "::" is expanded using normal generative rules of "[...]".

And in rules, to generate references to global variables and local variables:

   $:$1             => address
   $#               => number of locals

The address generated/inserted is a positive (>=0) if it's a global variable, and it's statically allocated in increasing order.

If it's a local variable, it generates a negative number, thusly indicating that it's relative to the current stack frame.

NOTE: Currently, nested functions ala JavaScript/PASCAL/ADA/ALGOL/GCC are not generating useful references. TODO: ;-)

NOTE: Currently, also because of this, there is no function for closures. This is somewhat more complicated, as it requires a dynamic/GC approach; either using a copying closure generator, or usage of dynamic function scope for functions containing captured variables.
   
# Name of this project

small parser+interpreter:

grokie - to grok a little -ie
  this might be better as it also implies understanding. As it can interpret generated code once the input program is parsed - it "groks", at least a little bit.

parsie - small parser?

parslet - a tiny parser

pars - forth like 4th letters, lol


## Speed

Compared to plain C-implementation, at first this alphabetical byte-code VM-interpreter may feel slow.

However, it's all about context:

We're using a recursive fibonacci implementation, with n=35.

Comparing with various interpreters and compilers these are rough numbers:


| implementation| time s | comment |
| ./alf raw	|  5.04s | hand-coded |
| ./pas (alf)	|  7.79s | compiled from pascal |
| ./pas (opt)   |  6.80s | `1@ => $1 |
| ./js (alf)	|  6.34s | compiled from js |
| clang -O3	|  0.17s | hand-coded |
| clang -Ofast	|  0.11s | hand-coded |
| picolisp	|  5.26s | |
| guile		|  1.40s | FASTEST lisp, template JIT |
| nodejs	|  0.89s | JIT... |
| perl		| 15.87s | not so good |
| python	|  5.72s | used to be slower? |
| gforth	|  2.25s | quite ok |
| ecl (lisp)	| 27.05s | worst in test! |

The winner is of course optimized C. Nodejs is a decent second hand. I though it would have been a bit faster. Perl is just supergood at text and regular expressions, but as a generic language and doing recursion is just sucky funny. Guile is doing very well, it uses a simplistic (?) template JIT. picolisp is just a VM, similarly to alf.

parsi and alf is only some weeks old, and not much time has been spent on optimizing performance. It is also in no way feature compllete and thus cannot be compared to the fullfledged other languages, in general.

Most overhead is relating to parameter passing and manipulation.

That the parsi-alf compiled JS is faster than PASCAL is interesting; it's an artifact of that there is an explicit "return" statement in JS. One could add "Exit;" and that would give similar speedup. This is because of the textual interpretation of the alf-code of if blocks taking time to process.

This performance figures are set to be updated and improved over time.
