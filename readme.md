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
- Syntax: `;` `and` `cond` `define` `define-macro` `do` `if` `lambda` `let` (including named let) `let*` `letrec` `or` `quote` `set!`
- Functions: `+` `-` `/` `*` `<` `<=` `>` `>=` `abs` `append` `assf` `assoc` `assv` `assq` `boolean?` `car` `cdr` `char?` `char->integer` `cons` `cadr` (`caaaar`, etc) `empty?` `eq?` `eqv?` `equal?` `even?` `expt` `filter` `first` (`second`, `third`...) `integer->char` `last` `length` `list` `list?` `list->string` `list-tail` `list-ref` `map` `member?` `min` `nil?` `not` `number?` `odd?` `pair?` `print` `procedure?` `remove` `remq` `remv` `rest` `reverse` `set-car!` `set-cdr!` `string->list` `string->symbol` `symbol->string`
- About 88K compiled statically.

Unfinished features:
- Print where errors happen
- Garbage collection
- Proper tail recursion
- Input/output, either interactively from stdin or files.
- Some of the standard library
    - Builtins: `display` `write` `read` `read-char` `newline`
    - Builtins: `eval`
    - Builtins: `random`
    - Hygenic macros: `let-syntax` and `syntax-rules`
    - Builtins for floats: `cos` `expt` `log` `sin` `sqrt` `tan`
    - Builtins: `call/cc`
- Nonstandard builtins: `~` `^` `|`
- Data types
    - Numbers: floats, bignums, complex numbers, rationals
    - Vectors

Bug reports are very welcome.
- Please give me any input which causes a segfault.
- Please give me any input that returns an error and shouldn't.
- Please give me any input that returns the wrong value.
- I will politely write down inputs that should give an error but don't.
