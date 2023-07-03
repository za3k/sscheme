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
    - Lambda functions (including closures and varargs)
    - Pairs, Lists
    - Constants: `nil` `#t` `#f` `()`
    - Characters `#\d` `#\newline`
    - Strings
- Comments
- Special forms: `and` `cond` `define` `define-macro` `if` `lambda` `or` `quote` `set!`
- Functions: `+` `-` `/` `*` `<` `<=` `>` `>=` `abs` `append` `assf` `assoc` `assv` `assq` `boolean?` `car` `cdr` `char?` `char->integer` `cons` `cadr` (`caaaar`, etc) `empty?` `eq?` `eqv?` `equal?` `even?` `expt` `filter` `first` (`second`, `third`...) `integer->char` `last` `length` `list` `list?` `list->string` `list-tail` `list-ref` `map` `member?` `min` `nil?` `not` `number?` `odd?` `pair?` `print` `procedure?` `remove` `remq` `remv` `rest` `reverse` `set-car!` `set-cdr!` `string->list` `string->symbol` `symbol->string`
- About 60K compiled statically.

Unfinished features:
- lambda varargs support
- Special forms: `do` `for-each` `let` `letrec`
- Garbage collection
- Detect infinite loops or out of memory
- Print where errors happen
- Input/output, either interactively from stdin or files.
- Proper tail recursion
- Most of the standard library
    - Builtins: `display` `write` `read` `read-char` `eval` `random`
    - Functions: `newline`
    - Hygenic macros: `let-syntax` and `syntax-rules`
- Builtins: `~` `^` `|`

Non-features (will never be added):
- Data types
    - Numbers: floats, bignums, complex numbers, rationals
    - Vectors
- Builtins for floats: `cos` `expt` `log` `sin` `sqrt` `tan`
- Builtins: `call/cc`
