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
    - Lists
    - Constants: `nil` `#t` `#f` `()`
    - Characters `#\d` `#\newline`
- Comments
- Special forms: `cond` `define` `define-macro` `lambda` `quote`
- Builtins: `+` `-` `car` `cdr` `cons` `cadr` (`caaaar`, etc) `empty?` `eq?` `first` (`second`, `third`...) `last` `list` `list?` `nil?` `number?` `print` `procedure?` `rest`

Unfinished features:
- lambda varargs support
- `(define (f arg1 arg) ...)` syntax
- Data types
    - Strings
- Special forms: `and` `do` `for-each` `if` `let` `letrec` `or` `set!` `set-car!` `set-cdr!`
- Garbage collection
- Input/output, either interactively from stdin or files.
- Most of the standard library
    - Builtins: `*` `/` `<` `apply` `display` `error` `eval` `quotient` `random`
    - Builtins: `call/cc`
    - Builtins: `~` `^` `|`
    - Functions: `<=` `=` `>` `>=` `abs` `append` `boolean?` `equal?` `even?` `expt` `filter` `length` `map` `max` `member?` `min` `newline` `not` `null?` `odd?` `repeated` `square` `write`

Non-features (will never be added):
- Data types
    - Numbers: floats, bignums, complex numbers, rationals
    - Vectors
- Builtins for floats: `cos` `expt` `log` `sin` `sqrt` `tan`
