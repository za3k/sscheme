#ifndef __TYPES
#define __TYPES

// Scheme Types

struct sval;
struct scons;

//   sexp (an sexp evaluates to an sval. we happen to use the same representation for both)
typedef struct sval sexp;

typedef struct sval {
  enum stype { UNALLOCATED, CONSTANT, ENV, ERROR, FUNCTION, MACRO, NUMBER, PAIR, PRIMITIVE, SPECIAL_FORM, STRING, SYMBOL } tag;
  union {
    // Used by PAIR, ENV, FUNCTION, and MACRO
    // PAIR is (<car> . <cdr>)
    // A list is (), or any pair where its <cdr> is a list.
    //
    // ENV is a list of frames.
    // FUNCTION is (<parameters> . (<body> . <env>))
    // MACRO is (<procedure> . ())
    struct scons {
        struct sval *car;
        struct sval *cdr;
    } list;
    // Used by ERROR
    char *error;
    // Used by NUMBER
    int smallnum;
    // Used by PRIMITIVE
    struct sval* (*primitive)(struct sval*);
    // Used by STRING and SYMBOL
    char *symbol;
    // CONSTANT, SPECIAL_FORM don't use any data at all.
  } body;
  // Allocator and garbage collector. These are each one bit, conceputally.
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

// PAIR
sval* _car(sval *pair);
sval* _cdr(sval *pair);
void _setcar(sval *pair, sval *v);
void _setcdr(sval *pair, sval *v);
// ENV
sval* _env_frame(sval *env);
sval* _env_parent(sval *env);
void _set_env_frame(sval *env, sval *v);
// MACRO
sval* _macro_procedure(sval *macro);
// FUNCTION
sval* _function_args(sval *function);
sval* _function_body(sval *function);
sval* _function_env(sval *function);

#endif // __TYPES
