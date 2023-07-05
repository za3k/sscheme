#ifndef __TYPES
#define __TYPES

// Scheme Types

struct sval;
struct scons;

//   sexp (an sexp evaluates to an sval. we happen to use the same representation for both)
typedef struct sval sexp;

typedef struct sval {
  enum stype { CONSTANT, ENV, ERROR, FUNCTION, MACRO, NUMBER, PAIR, PRIMITIVE, SPECIAL_FORM, STRING, SYMBOL } tag;
  union {
    // CONSTANT, SPECIAL_FORM don't use any data at all.
    // Used by ENV
    struct senv {
        struct sval *frame; // List of (name, value) pairs
        struct sval *parent;
    } env;
    // Used by ERROR
    char *error;
    // Used by FUNCTION
    struct sclosure {
        sexp *parameters; // (unevaluated) list of parameter symbols
        sexp *body; // Unevaluated function body
        struct sval *env;
    } closure;
    // Used by MACRO
    struct sval *macro_procedure;
    // Used by NUMBER
    int smallnum;
    // Used by PAIR.
    // ENV is (frame, parent) -- in other words, a list of frames.
    // FUNCTION is (<parameters>, <body>, <env>)
    struct scons {
        struct sval *car;
        struct sval *cdr;
    } list;
    // Used by PRIMITIVE
    struct sval* (*primitive)(struct sval*);
    // Used by STRING and SYMBOL
    char *symbol;
  } body;
  // Allocator and garbage collector. These are each one bit.
  unsigned char allocated;
  unsigned char in_use;
  unsigned char marked;
} sval;

// The helpers
sval* error(char *msg, ...);
sval* make_cons(sval *car, sval *cdr);
sval* make_int(int i);
sval* make_symbol(char* name);
sval* make_string(char* str);
sval* make_env(sval* env);

sval* make_prim(sval* primitive(sval*));
sexp* make_function(sexp *parameters, sexp *body, sval *env);
sexp* make_macro(sexp *function);
sval* make_character_constant (char c);

#endif // __TYPES
