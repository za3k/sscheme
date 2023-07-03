#ifndef __ERROR
#define __ERROR

// Parser
#define ERR_INVALID_CHAR        "parse: Invalid character"
#define ERR_STRING_UNTERMINATED "String literal never terminated"
#define ERR_QUOTE_UNTERMINATED  "Quoted literal never terminated"
#define ERR_LIST_UNTERMINATED   "List/pair never terminated"
#define ERR_QUASIQUOTE_UNTERMINATED   "Quasiquote (`) not followed by anything"
#define ERR_UNQUOTE_UNTERMINATED   "Unquote (, or ,@) not followed by anything"
#define ERR_UNKNOWN_TOKEN       "parse: Unknown token type"
#define ERR_UNEXPECTED_CLOSE    "Unbalanced and unexpected close paren"
#define ERR_UNEXPECTED_DOT      "Encountered '.' symbol in unexpected place"
#define ERR_LOGIC               "parse/lookup_frame: Logic error, unexpected token type encountered"

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
#define ERR_SYMBOL_NOT_FOUND   "The symbol '%s' was unbound"
#define ERR_TOO_FEW_PARAM      "Too few parameters passed to function"
#define ERR_TOO_MANY_PARAM     "Too many parameters passed to function"
#define ERR_WRONG_NUM_FORM     "special form '%s' passed wrong number of arguments"
#define ERR_UNQUOTE_NOWHERE    "Unquote (, or ,@) not inside a quasiquote (`)"
#define ERR_QUASIQUOTE         "Quasiquote (`) not yet implemented"
#define ERR_SET                "set!: Tried to set! a nonexistent variable %s"

// Primitive execution
#define ERR_NULL_PTR    "primitive '%s' passed a null pointer"
#define ERR_EMPTY_LIST  "primitive '%s' passed an empty list"
#define ERR_NON_LIST    "prim_* '%s' passed a non-list"
#define ERR_WRONG_NUM   "primitive '%s' passed wrong number of arguments"
#define ERR_WRONG_TYPE  "primitive '%s' passed the wrong argument type(s)"
#define ERR_DIV_BY_ZERO "division by zero"
#define ERR_CHAR_OUT_OF_RANGE  "integer->char out of range"
#define ERR_STRING_TOO_BIG     "Tried to make a string bigger than allowed"
#define ERR_SYMBOL_TOO_BIG     "Tried to make a symbol bigger than allowed"

#endif // __ERROR
