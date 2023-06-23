/* This is a small scheme compiler.
   It is based on the 1986 video lecture series from MIT, "Structure and Interpretation of Computer Programs", which follows the book of the same name, and which is taught by the authors.
   This is tightly based on lecture 10, "Metacircular Evaluator", the most primitive interpreter provided.
*/

// TODO: String, char

#ifndef __TYPES
#define __TYPES

// Scheme Types

struct sval;
struct scons;

struct scons {
    struct sval *car;
    struct sval *cdr;
};

// Internal types
//   sexp (an sexp evaluates to an sval. we happen to use the same representation for both)
typedef struct sval sexp;
//   Environment
struct senv {
    struct {
        //char **names;
        //struct sval **vals;
        struct sval *names; // list of symbols
        struct sval *values; // list of svals
    } frame;
    struct senv *parent;
};
//   Closure (represents a function)
struct sclosure {
    sexp *parameters; // (unevaluated) list of parameter symbols
    sexp *body; // Unevaluated function body
    struct senv *env;
};


// Sh
typedef struct sval {
  enum stype { NUMBER, SYMBOL, CONS, SPECIAL_FORM, PRIMITIVE, FUNCTION, CONSTANT, ERROR } tag;
  union {
    struct scons list; // NIL is also considered the empty list
    char *symbol; // Symbols are parsed to strings instead of ints for convenience
    int smallnum; // Small integer. No bignum support.
    enum {
       FALSE, TRUE, EMPTY_LIST, NIL,
    } constant;
    enum { 
        quote, lambda, cond, // Special forms
    } form;
    struct sval* (*primitive)(struct sval*);
    struct sclosure *closure;
    char *error;
  } body;
} sval;

// The helpers
sval* error(char *msg);
sval* make_cell();
sval* make_cons(sval *car, sval *cdr);
sval* make_int(int i);
sval* make_symbol(char* name);

// XXX How to move defs to .h file instead of using singleton "makers"?
sval* make_true();
sval* make_nil();
sval* make_false();
sval* make_empty();
sval* make_prim(sval* primitive(sval*));
sexp* make_function(sexp *parameters, sexp *body, struct senv *env);

sexp* make_cond();
sexp* make_quote();
sexp* make_lambda();

#endif // __TYPES
