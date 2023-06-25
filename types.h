// TODO: String

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
  enum stype { CONS, CONSTANT, ERROR, FUNCTION, MACRO, NUMBER, PRIMITIVE, SPECIAL_FORM, SYMBOL } tag;
  union {
    struct scons list; // NIL is also considered the empty list
    char *symbol; // Symbols are parsed to strings instead of ints for convenience
    int smallnum; // Small integer. No bignum support.
    enum {
       // TODO: We never look at this. Use only memory position?
       FALSE, TRUE, EMPTY_LIST, NIL,
       C000, C001, C002, C003, C004, C005, C006, C007, C008, C009,
       C010, C011, C012, C013, C014, C015, C016, C017, C018, C019,
       C020, C021, C022, C023, C024, C025, C026, C027, C028, C029,
       C030, C031, C032, C033, C034, C035, C036, C037, C038, C039,
       C040, C041, C042, C043, C044, C045, C046, C047, C048, C049,
       C050, C051, C052, C053, C054, C055, C056, C057, C058, C059,
       C060, C061, C062, C063, C064, C065, C066, C067, C068, C069,
       C070, C071, C072, C073, C074, C075, C076, C077, C078, C079,
       C080, C081, C082, C083, C084, C085, C086, C087, C088, C089,
       C090, C091, C092, C093, C094, C095, C096, C097, C098, C099,
       C100, C101, C102, C103, C104, C105, C106, C107, C108, C109,
       C110, C111, C112, C113, C114, C115, C116, C117, C118, C119,
       C120, C121, C122, C123, C124, C125, C126, C127,
    } constant;
    enum { 
       // TODO: We rarely look at this. Use only memory position?
        form_cond, form_define, form_define_macro, form_lambda, form_quote // Special forms
    } form;
    struct sval *macro_procedure;
    struct sval* (*primitive)(struct sval*);
    struct sclosure closure;
    char *error;
  } body;
} sval;

// The helpers
sval* error(char *msg);
sval* make_cell();
sval* make_cons(sval *car, sval *cdr);
sval* make_int(int i);
sval* make_symbol(char* name);

sval* make_empty();
sval* make_prim(sval* primitive(sval*));
sexp* make_function(sexp *parameters, sexp *body, struct senv *env);
sexp* make_macro(sexp *function);
sval* make_character_constant (char c);

#endif // __TYPES
