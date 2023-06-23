#ifndef __ERROR
#define __ERROR

// Parser
#define ERR_INVALID_CHAR       "parse: Invalid character"
#define ERR_UNKNOWN_TOKEN      "parse: Unknown token type"
#define ERR_UNEXPECTED_CLOSE   "Unbalanced and unexpected close paren"
#define ERR_QUOTE_NOT_IMPL     "Quote literals not yet implemented. Use the function 'quote' instead"

// Eval/Apply core loop
#define ERR_EVAL_CLOSURE       "eval: Tried to eval a closure"
#define ERR_APPLY_NON_FUNCTION "Tried to apply a non-function"
#define ERR_EVLIST_NON_LIST    "evlist: Tried to evlist a non-list"
#define ERR_TOO_MANY_PARAM     "Too many parameters passed to function"
#define ERR_TOO_FEW_PARAM      "Too few parameters passed to function"
#define ERR_FRAME_NON_LIST     "lookup_frame: A non-list was passed"
#define ERR_FRAME_NON_SYMBOL   "lookup_frame: A non-symbol was present in a parameter list"
#define ERR_EVAL_UNKNOWN       "eval: An unknown type was encountered"
#define ERR_SYMBOL_NOT_FOUND   "The symbol was unbound"

// Primitive execution
#define ERR_WRONG_NUM  "primitive passed wrong number of arguments"
#define ERR_WRONG_TYPE "primitive passed the wrong argument type(s)"
#define ERR_NON_LIST   "prim_* passed a non-list"
#define ERR_EMPTY_LIST "primitive passed an empty list (car/cdr)"

#endif // __ERROR
