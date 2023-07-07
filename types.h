#ifndef __TYPES
#define __TYPES

// Scheme Types

struct sval;
struct scons;

//   sexp (an sexp evaluates to an sval. we happen to use the same representation for both)
typedef struct sval sexp;
typedef int heap_pointer;

typedef struct __attribute((packed)) sval {
  union {
    // Used by PAIR, ENV, FUNCTION, and MACRO
    // PAIR is (<car> . <cdr>)
    // A list is (), or any pair where its <cdr> is a list.
    //
    // ENV is a list of frames.
    // FUNCTION is (<parameters> . (<body> . <env>))
    // MACRO is (<procedure> . ())
    struct scons {
        heap_pointer car;
        heap_pointer cdr;
    } list;
    // Used by NUMBER
    int smallnum;
    // Used by PRIMITIVE
    unsigned char primitive;
    // Used by ERROR, STRING, and SYMBOL
    char *symbol;
    // CONSTANT, SPECIAL_FORM don't use any data at all.
  } body;
  enum __attribute__((packed)) { UNALLOCATED, CONSTANT, ENV, ERROR, FUNCTION, MACRO, NUMBER, PAIR, PRIMITIVE, SPECIAL_FORM, STRING, SYMBOL } tag;
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
// PRIMITIVE
sval* (*_primitive_func(sval *primitive))(sval*);

#endif // __TYPES
