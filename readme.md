This is a small scheme interpreter.
I wrote it as an educational project to teach myself.

It is based on the 1986 video lecture series from MIT, "Structure and Interpretation of Computer Programs", which follows the book of the same name, and which is taught by the authors.
This is tightly based on [lecture 10](https://www.youtube.com/watch?v=aAlR3cezPJg), "Metacircular Evaluator", the most primitive interpreter provided. The main additions are some extra primitive types and a simple (unsafe) macro system.

You can probably write some basic programs with it. Just don't expect nice error messages.

Usage is:

```
$ make eval
$ ./eval "((lambda (x y) y) 3 4)"
    ((:lambda (:x :y) :y) 3 4)
    4
$ ./eval "(+ 3 4)"
    (:+ 3 4)
    7
```

Finished features:
- Basic data types
    - Numbers (integers only)
    - Symbols
    - Builtin functions
    - Lambda functions (including closures)
    - Pairs, Lists
    - Constants: `nil` `#t` `#f` `()`
    - Characters `#\d` `#\newline`
    - Strings
- Comments
- Special forms: `cond` `define` `define-macro` `if` `lambda` `quote`
- Builtins: `+` `-` `append` `assf` `assoc` `assv` `assq` `car` `cdr` `cons` `cadr` (`caaaar`, etc) `empty?` `eq?` `filter` `first` (`second`, `third`...) `last` `length` `list` `list?` `list-tail` `list-ref` `map` `nil?` `not` `number?` `pair?` `print` `procedure?` `remove` `remq` `remv` `rest` `reverse`
- About 60K compiled statically.

Unfinished features:
- lambda varargs support
- `(define (f arg1 arg) ...)` syntax
- Special forms: `and` `do` `for-each` `let` `letrec` `or` `set!` `set-car!` `set-cdr!`
- Garbage collection
- Detect infinite loops or out of memory
- Print where errors happen
- Input/output, either interactively from stdin or files.
- Most of the standard library
    - Builtins: `*` `/` `<` `apply` `display` `error` `eval` `quotient` `random`
    - Builtins: `call/cc`
    - Builtins: `~` `^` `|`
    - Functions: `<=` `=` `>` `>=` `abs` `boolean?` `equal?` `even?` `expt` `max` `member?` `min` `newline` `null?` `odd?` `repeated` `square` `write`

Non-features (will never be added):
- Data types
    - Numbers: floats, bignums, complex numbers, rationals
    - Vectors
- Builtins for floats: `cos` `expt` `log` `sin` `sqrt` `tan`
