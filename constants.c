#include "errors.h"
#include "types.h"

struct sval TRUE_V = { CONSTANT };
struct sval FALSE_V = { CONSTANT };
struct sval EMPTY_LIST_V = { CONSTANT };
struct sval NIL_V = { CONSTANT };
sexp LAMBDA_V = { SPECIAL_FORM };
sexp COND_V = { SPECIAL_FORM };
sexp DEFINE_V = { SPECIAL_FORM };
sexp QUOTE_V = { SPECIAL_FORM };
sexp QUASIQUOTE_V = { SPECIAL_FORM };
sexp SET_V = { SPECIAL_FORM };
sexp UNQUOTE_V = { SPECIAL_FORM };
sexp UNQUOTE_SPLICING_V = { SPECIAL_FORM };
sexp DEFINE_MACRO_V = { SPECIAL_FORM  };
sval BUILTINS_ENV_V = { ENV, .body.env = { &EMPTY_LIST_V, 0} };
sval STANDARD_ENV_V = { ENV, .body.env = { &EMPTY_LIST_V, &BUILTINS_ENV_V} };
sval OUT_OF_MEMORY_V = { ERROR, .body.error = ERR_OUT_OF_MEMORY };
struct sval CHARS_V[128] = {
    {CONSTANT},
};

char *char_constant_names[] = {
    "#\\nul", "#\\soh", "#\\stx", "#\\etx", // 0-3
    "#\\eot", "#\\enq", "#\\ack", "#\\bel", // 4-7
    "#\\bs",  "#\\ht",  "#\\lf",  "#\\vt",  // 8-11
    "#\\ff",  "#\\cr",  "#\\so",  "#\\si",  // 12-15
    "#\\dle", "#\\dc1", "#\\dc2", "#\\dc3", // 16-19
    "#\\dc4", "#\\nak", "#\\syn", "#\\etb", // 20-23
    "#\\can", "#\\em",  "#\\sub", "#\\esc", // 24-27
    "#\\fs",  "#\\gs",  "#\\rs",  "#\\us",  // 28-31
    "#\\sp",
    "#\\!", "#\\\'", "#\\#", "#\\$", "#\\%", "#\\&", "\\'", // 33 - 39
    "#\\(", "#\\)", "#\\*", "#\\+", "#\\,", // 40-44
    "#\\-", "#\\.", "#\\/", "#\\0", "#\\1", // 45-49
    "#\\2", "#\\3", "#\\4", "#\\5", "#\\6", // 50-54
    "#\\7", "#\\8", "#\\9", "#\\:", "#\\;", // 55-59
    "#\\<", "#\\=", "#\\>", "#\\?", "#\\@", // 60-64
    "#\\A", "#\\B", "#\\C", "#\\D", "#\\E", // 65-69
    "#\\F", "#\\G", "#\\H", "#\\I", "#\\J", // 70-74
    "#\\K", "#\\L", "#\\M", "#\\N", "#\\O", // 75-79
    "#\\P", "#\\Q", "#\\R", "#\\S", "#\\T", // 80-84
    "#\\U", "#\\V", "#\\W", "#\\X", "#\\Y", // 85-89
    "#\\Z", "#\\[", "#\\\\", "#\\]", "#\\^", // 90-94
    "#\\_", "#\\`", "#\\a", "#\\b", "#\\c", // 95-99
    "#\\d", "#\\e", "#\\f", "#\\g", "#\\h", // 100-104
    "#\\i", "#\\j", "#\\k", "#\\l", "#\\m", // 105-109
    "#\\n", "#\\o", "#\\p", "#\\q", "#\\r", // 110-114
    "#\\s", "#\\t", "#\\u", "#\\v", "#\\w", // 115-119
    "#\\x", "#\\y", "#\\z", "#\\{", "#\\|", // 120-124
    "#\\}", "#\\~", "#\\del", // 125-127
    "#\\alarm", // 7
    "#\\backspace", // 8
    "#\\tab", // 9
    "#\\linefeed", // 10
    "#\\newline", // 10
    "#\\vtab", // 11
    "#\\page", // 12
    "#\\return", // 13
    "#\\escape", // 27
    "#\\space", // 32
    "#\\delete", // 127
};
const char char_constant_values[] = {
   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,
  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,
  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,
  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,
  90,  91,  92,  93,  94,  95,  96,  97,  98,  99,
 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
 120, 121, 122, 123, 124, 125, 126, 127,

 7,8,9,10,10,11,12,13,27,32,127
};

struct sval* TRUE=&TRUE_V;
struct sval* FALSE=&FALSE_V;
struct sval* EMPTY_LIST=&EMPTY_LIST_V;
struct sval* NIL=&NIL_V;
sexp *COND = &COND_V;
sexp *DEFINE = &DEFINE_V;
sexp *DEFINE_MACRO = &DEFINE_MACRO_V;
sexp *LAMBDA = &LAMBDA_V;
sexp *QUASIQUOTE = &QUASIQUOTE_V;
sexp *QUOTE = &QUOTE_V;
sexp *SET = &SET_V;
sexp *UNQUOTE = &UNQUOTE_V;
sexp *UNQUOTE_SPLICING = &UNQUOTE_SPLICING_V;
sval* BUILTINS_ENV = &BUILTINS_ENV_V;
sval* STANDARD_ENV = &STANDARD_ENV_V;
sval* OUT_OF_MEMORY = &OUT_OF_MEMORY_V;
