#ifndef __ERROR
#define __ERROR

// Parser
#define ERR_INVALID_CHAR        "parse: Invalid character"
#define ERR_QUOTE_NOT_IMPL      "Quote literals not yet implemented. Use the function 'quote' instead"
#define ERR_STRING_UNTERMINATED "String literal never terminated"
#define ERR_UNKNOWN_TOKEN       "parse: Unknown token type"
#define ERR_UNEXPECTED_CLOSE    "Unbalanced and unexpected close paren"
#define ERR_UNEXPECTED_DOT      "Encountered '.' symbol in unexpected place"

// Eval/Apply core loop
#define ERR_APPLY_NON_FUNCTION "Tried to apply a non-function"
#define ERR_DEFINE_NONSYMBOL   "Define can only be used to define a symbol (no lambda syntax yet)"
#define ERR_EVAL_CLOSURE       "eval: Tried to eval a closure"
#define ERR_EVAL_PRIMITIVE     "eval: Tried to eval a primitive"
#define ERR_EVAL_MACRO         "eval: Tried to eval a macro"
#define ERR_EVLIST_NON_LIST    "evlist: Tried to evlist a non-list"
#define ERR_EVAL_UNKNOWN       "eval: An unknown type was encountered"
#define ERR_BIND_UNKNOWN       "bind: An unknown type was encountered"
#define ERR_EXPECTED_ENV       "Expected an argument to be of type ENV"
#define ERR_FRAME_NON_LIST     "lookup_frame: A non-list was passed"
#define ERR_FRAME_NON_SYMBOL   "lookup_frame: A non-symbol was present in a parameter list"
#define ERR_SYMBOL_NOT_FOUND   "The symbol was unbound"
#define ERR_TOO_FEW_PARAM      "Too few parameters passed to function"
#define ERR_TOO_MANY_PARAM     "Too many parameters passed to function"

// Primitive execution
#define ERR_EMPTY_LIST  "primitive passed an empty list (car/cdr)"
#define ERR_NON_LIST    "prim_* passed a non-list"
#define ERR_WRONG_NUM   "primitive passed wrong number of arguments"
#define ERR_WRONG_TYPE  "primitive passed the wrong argument type(s)"
#define ERR_DIV_BY_ZERO "division by zero"

#endif // __ERROR
