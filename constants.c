#include "types.h"


struct sval TRUE_V = { CONSTANT, .body.constant = constant_true };
struct sval FALSE_V = { CONSTANT, .body.constant = constant_false };
struct sval EMPTY_LIST_V = { CONSTANT, .body.constant = constant_empty_list };
struct sval NIL_V = { CONSTANT, .body.constant = constant_nil };
sexp LAMBDA_V = { SPECIAL_FORM, .body.form = form_lambda };
sexp COND_V = { SPECIAL_FORM, .body.form = form_cond };
sexp QUOTE_V = { SPECIAL_FORM, .body.form = form_quote };
sexp DEFINE_V = { SPECIAL_FORM, .body.form = form_define };
sexp DEFINE_MACRO_V = { SPECIAL_FORM, .body.form = form_define_macro };
struct senv BUILTINS_ENV_V = { { &EMPTY_LIST_V, &EMPTY_LIST_V }, 0 };
struct senv STANDARD_ENV_V = { { &EMPTY_LIST_V, &EMPTY_LIST_V}, &BUILTINS_ENV_V };
struct sval CHARS_V[128] = {
    {CONSTANT, .body.constant = C000},
    {CONSTANT, .body.constant = C001},
    {CONSTANT, .body.constant = C002},
    {CONSTANT, .body.constant = C003},
    {CONSTANT, .body.constant = C004},
    {CONSTANT, .body.constant = C005},
    {CONSTANT, .body.constant = C006},
    {CONSTANT, .body.constant = C007},
    {CONSTANT, .body.constant = C008},
    {CONSTANT, .body.constant = C009},

    {CONSTANT, .body.constant = C010},
    {CONSTANT, .body.constant = C011},
    {CONSTANT, .body.constant = C012},
    {CONSTANT, .body.constant = C013},
    {CONSTANT, .body.constant = C014},
    {CONSTANT, .body.constant = C015},
    {CONSTANT, .body.constant = C016},
    {CONSTANT, .body.constant = C017},
    {CONSTANT, .body.constant = C018},
    {CONSTANT, .body.constant = C019},

    {CONSTANT, .body.constant = C020},
    {CONSTANT, .body.constant = C021},
    {CONSTANT, .body.constant = C022},
    {CONSTANT, .body.constant = C023},
    {CONSTANT, .body.constant = C024},
    {CONSTANT, .body.constant = C025},
    {CONSTANT, .body.constant = C026},
    {CONSTANT, .body.constant = C027},
    {CONSTANT, .body.constant = C028},
    {CONSTANT, .body.constant = C029},

    {CONSTANT, .body.constant = C030},
    {CONSTANT, .body.constant = C031},
    {CONSTANT, .body.constant = C032},
    {CONSTANT, .body.constant = C033},
    {CONSTANT, .body.constant = C034},
    {CONSTANT, .body.constant = C035},
    {CONSTANT, .body.constant = C036},
    {CONSTANT, .body.constant = C037},
    {CONSTANT, .body.constant = C038},
    {CONSTANT, .body.constant = C039},

    {CONSTANT, .body.constant = C040},
    {CONSTANT, .body.constant = C041},
    {CONSTANT, .body.constant = C042},
    {CONSTANT, .body.constant = C043},
    {CONSTANT, .body.constant = C044},
    {CONSTANT, .body.constant = C045},
    {CONSTANT, .body.constant = C046},
    {CONSTANT, .body.constant = C047},
    {CONSTANT, .body.constant = C048},
    {CONSTANT, .body.constant = C049},

    {CONSTANT, .body.constant = C050},
    {CONSTANT, .body.constant = C051},
    {CONSTANT, .body.constant = C052},
    {CONSTANT, .body.constant = C053},
    {CONSTANT, .body.constant = C054},
    {CONSTANT, .body.constant = C055},
    {CONSTANT, .body.constant = C056},
    {CONSTANT, .body.constant = C057},
    {CONSTANT, .body.constant = C058},
    {CONSTANT, .body.constant = C059},

    {CONSTANT, .body.constant = C060},
    {CONSTANT, .body.constant = C061},
    {CONSTANT, .body.constant = C062},
    {CONSTANT, .body.constant = C063},
    {CONSTANT, .body.constant = C064},
    {CONSTANT, .body.constant = C065},
    {CONSTANT, .body.constant = C066},
    {CONSTANT, .body.constant = C067},
    {CONSTANT, .body.constant = C068},
    {CONSTANT, .body.constant = C069},

    {CONSTANT, .body.constant = C070},
    {CONSTANT, .body.constant = C071},
    {CONSTANT, .body.constant = C072},
    {CONSTANT, .body.constant = C073},
    {CONSTANT, .body.constant = C074},
    {CONSTANT, .body.constant = C075},
    {CONSTANT, .body.constant = C076},
    {CONSTANT, .body.constant = C077},
    {CONSTANT, .body.constant = C078},
    {CONSTANT, .body.constant = C079},

    {CONSTANT, .body.constant = C080},
    {CONSTANT, .body.constant = C081},
    {CONSTANT, .body.constant = C082},
    {CONSTANT, .body.constant = C083},
    {CONSTANT, .body.constant = C084},
    {CONSTANT, .body.constant = C085},
    {CONSTANT, .body.constant = C086},
    {CONSTANT, .body.constant = C087},
    {CONSTANT, .body.constant = C088},
    {CONSTANT, .body.constant = C089},

    {CONSTANT, .body.constant = C090},
    {CONSTANT, .body.constant = C091},
    {CONSTANT, .body.constant = C092},
    {CONSTANT, .body.constant = C093},
    {CONSTANT, .body.constant = C094},
    {CONSTANT, .body.constant = C095},
    {CONSTANT, .body.constant = C096},
    {CONSTANT, .body.constant = C097},
    {CONSTANT, .body.constant = C098},
    {CONSTANT, .body.constant = C099},

    {CONSTANT, .body.constant = C100},
    {CONSTANT, .body.constant = C101},
    {CONSTANT, .body.constant = C102},
    {CONSTANT, .body.constant = C103},
    {CONSTANT, .body.constant = C104},
    {CONSTANT, .body.constant = C105},
    {CONSTANT, .body.constant = C106},
    {CONSTANT, .body.constant = C107},
    {CONSTANT, .body.constant = C108},
    {CONSTANT, .body.constant = C109},

    {CONSTANT, .body.constant = C110},
    {CONSTANT, .body.constant = C111},
    {CONSTANT, .body.constant = C112},
    {CONSTANT, .body.constant = C113},
    {CONSTANT, .body.constant = C114},
    {CONSTANT, .body.constant = C115},
    {CONSTANT, .body.constant = C116},
    {CONSTANT, .body.constant = C117},
    {CONSTANT, .body.constant = C118},
    {CONSTANT, .body.constant = C119},

    {CONSTANT, .body.constant = C120},
    {CONSTANT, .body.constant = C121},
    {CONSTANT, .body.constant = C122},
    {CONSTANT, .body.constant = C123},
    {CONSTANT, .body.constant = C124},
    {CONSTANT, .body.constant = C125},
    {CONSTANT, .body.constant = C126},
    {CONSTANT, .body.constant = C127},
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
sexp *LAMBDA = &LAMBDA_V;
sexp *COND = &COND_V;
sexp *QUOTE = &QUOTE_V;
sexp *DEFINE = &DEFINE_V;
sexp *DEFINE_MACRO = &DEFINE_MACRO_V;
struct senv* BUILTINS_ENV = &BUILTINS_ENV_V;
struct senv* STANDARD_ENV = &STANDARD_ENV_V;
