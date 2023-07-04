#ifndef __TYPES
#define __TYPES

// Scheme Types

struct sval;
struct scons;
extern long cells_used;

//   sexp (an sexp evaluates to an sval. we happen to use the same representation for both)
typedef struct sval sexp;

typedef struct sval {
  enum stype { CONSTANT, ENV, ERROR, FUNCTION, MACRO, NUMBER, PAIR, PRIMITIVE, SPECIAL_FORM, STRING, SYMBOL } tag;
  union {
    struct scons {
        struct sval *car;
        struct sval *cdr;
    } list;
    char *symbol; // Symbols are parsed to strings instead of ints for convenience
    int smallnum; // Small integer. No bignum support.
    // Constants just use pointer comparison. No data.
    // Forms just use pointer comparison. No data.
    struct sval *macro_procedure;
    struct sval* (*primitive)(struct sval*);
    //   Closure (represents a function)
    struct sclosure {
        sexp *parameters; // (unevaluated) list of parameter symbols
        sexp *body; // Unevaluated function body
        struct sval *env;
    } closure;
    // Environment
    struct senv {
        struct sval *frame; // List of (name, value) pairs
        struct sval *parent;
    } env;
    char *error;
  } body;
} sval;

// The helpers
sval* error(char *msg, ...);
sval* make_cell();
sval* make_cons(sval *car, sval *cdr);
sval* make_int(int i);
sval* make_symbol(char* name);
sval* make_string(char* str);
sval* make_env(sval* env);

sval* make_empty();
sval* make_prim(sval* primitive(sval*));
sexp* make_function(sexp *parameters, sexp *body, sval *env);
sexp* make_macro(sexp *function);
sval* make_character_constant (char c);

#endif // __TYPES
