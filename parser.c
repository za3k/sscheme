#include "constants.h"
#include "errors.h"
#include "helpers.h"
#include "parser.h"
#include "prims.h"
#include <stdlib.h>
#include <string.h>

// TODO: Intern symbols to save space and allow == comparison
// TODO: Point to where an error is--save the source location of each sexp and token

enum token_type {
    tok_close_paren,
    tok_comment,
    tok_constant,
    tok_dot,
    tok_eof,
    tok_invalid,
    tok_number_decimal,
    tok_number_hex,
    tok_number_octal,
    tok_open_paren,
    tok_quote,
    tok_string,
    tok_symbol,
    tok_whitespace,
};


enum token_type detect_token_type(char **s);

// All parse_* function "consume" some of *s by mutating it to point later unless otherwise noted.
int parse_int_decimal(char **s, int *result);
int parse_int_octal(char **s, int *result);
int parse_int_hex(char **s, int *result);
char* parse_string(char **s);
char* parse_symbol(char **s);
sexp* parse_character_constant(char**s);
sexp* parse_constant(char**s);
void parse_comment(char **s);
void parse_dot(char **s);
void parse_close_paren(char **s);
void parse_open_paren(char **s);
void parse_whitespace(char **s);

enum token_type parse_token_type(char **s); // Returns next "real" token--skips whitespace and comments
sexp* parse_list_right(char **s, int atleastone); // Parse a list.
sexp* parse_sexp(char **s); // Returns the next parsed s-expression
sexp* parse(char *s); // Return a (sexp) list of parsed s-expresions.


enum token_type detect_token_type(char **s) {
    // Look 2 characters ahead and decide the next token type.
    switch(**s) {
        case '-':
            switch((*s)[1]) {
                case '0'...'9': return tok_number_decimal;
                default: return tok_symbol;
            }
            break;
        case '0' ... '9': return tok_number_decimal;
        case ';': return tok_comment;
        case '(': return tok_open_paren;
        case ')': return tok_close_paren;
        case ' ': case '\t': case '\n': case '\r': return tok_whitespace;
        case '\'': return tok_quote;
        case '#':
            switch((*s)[1]) {
                case 't': return tok_constant;
                case 'f': return tok_constant;
                case '\\': return tok_constant;
                case 'o': return tok_number_octal;
                case 'd': return tok_number_decimal;
                case 'x': return tok_number_hex;
                default: return tok_invalid;
            }
        case '\0': return tok_eof;
        case '.':
            switch ((*s)[1]) {
                case ' ': case '\t': case '\n': case '\r': case '\0':
                case ')':
                    return tok_dot;
                default: return tok_symbol;
            }
        case 'a'...'z':
        case 'A'...'Z':
        case '!': case '@': case '$': case '%': case '^':
        case '&': case '*': case '_': case '=': case '+':
        case '?': case '>': case '<': case '/': case ':': case '~':
            return tok_symbol;
        case '"': return tok_string;
        default: return tok_invalid;
    }
}

char STRING_BUF[1000];
char* parse_string(char **s) {
    // TODO: Warn about too-long strings or use malloc/salloc
    int len=0;
    (*s)++; // Initial quote
    while (1) {
        char p=**s;
        switch(p) {
            case 0: return 0;
            case '\"':
                (*s)++;
                STRING_BUF[len]=0;
                return strdup(STRING_BUF);
            case '\\':
                (*s)++;
                p=**s;
                switch(p) {
                    case 'a': p='\a'; break;
                    case 'b': p='\b'; break;
                    case 'f': p='\f'; break;
                    case 'n': p='\n'; break;
                    case 'r': p='\r'; break;
                    case 't': p='\t'; break;
                    case 'v': p='\v'; break;
                    case '0': p='\0'; break; // Will break the string, whatever.
                    case 0: return 0;
                    // default: p=p
                }
            default:
                STRING_BUF[len++]=p;
        }
        (*s)++;
    }
    return 0;
}

int parse_int_decimal(char **s, int *result) {
    int out = 0;
    int sign=1;
    if ((*s)[0] == '#' && (*s)[1]=='d') (*s)+=2;
    if (**s == '-') { (*s)++; sign=-1; }
    for (char* p=*s;; p++) {
        switch (*p) {
            case '0' ... '9':
                out = out * 10 + (*p-'0');
                break;
            case ' ': case '\t': case '\n': case '\r':
            case ')': case '(': case '\0':
                *s = p;
                *result = out*sign;
                return 1;
            default:
                return 0;
        }
    }
}
int parse_int_octal(char **s, int *result) {
    int out = 0;
    int sign=1;
    if ((*s)[0] == '#' && (*s)[1]=='o') (*s)+=2;
    if (**s == '-') { (*s)++; sign=-1; }
    for (char* p=*s;; p++) {
        switch (*p) {
            case '0' ... '7':
                out = out * 8 + (*p-'0');
                break;
            case ' ': case '\t': case '\n': case '\r':
            case ')': case '(': case '\0':
                *s = p;
                *result = out*sign;
                return 1;
            default:
                return 0;
        }
    }
}
int parse_int_hex(char **s, int *result) {
    int out = 0;
    int sign=1;
    if ((*s)[0] == '#' && (*s)[1]=='x') (*s)+=2;
    if (**s == '-') { (*s)++; sign=-1; }
    for (char* p=*s;; p++) {
        switch (*p) {
            case '0' ... '9':
                out = out * 16 + (*p-'0'); break;
            case 'a' ... 'f':
                out = out * 16 + (*p-'a'+10); break;
            case 'A' ... 'F':
                out = out * 16 + (*p-'A'+10); break;
            case ' ': case '\t': case '\n': case '\r':
            case ')': case '(': case '\0':
                *s = p;
                *result = out*sign;
                return 1;
            default:
                return 0;
        }
    }
}

char CONSTANT_BUF[200];
sexp* parse_character_constant(char**s) {
    int length=0, i;
    char *constant = *s;

    // Step 0, test for #\) or #\(
    if (constant[2]=='(' || constant[2]==')') {
        (*s)+=3;
        return make_character_constant(constant[2]);
    }

    // Step 1, read into a string.
    while (1) {
        switch((*s)[length]) {
            case ' ': case '\t': case '\n': case '\r': case '\0':
            case '(': case ')':
                CONSTANT_BUF[length] = 0;
                goto done;
            default:
                CONSTANT_BUF[length] = (*s)[length];
                length++;
                if (length >= 100) return 0;
        }
    }
    done:
    (*s)+=length;

    // Step 2, check against the hardcoded list of character constants.
    for (i=0; i<sizeof(char_constant_names)/sizeof(char*); i++) {
        if (strcmp(char_constant_names[i], CONSTANT_BUF)==0) {
            return make_character_constant(char_constant_values[i]);
        }
    }
    return 0;
}

sexp* parse_constant(char **s) {
    switch((*s)[1]) {
        case 't': // #t
            (*s)+=2;
            return TRUE;
        case 'f': // #f
            (*s)+=2;
            return FALSE;
        case '\\': // Character constant
            return parse_character_constant(s);
        default:
            return 0;
    }
}

char SYMBOL_BUF[1000];
char* parse_symbol(char **s) {
    // TODO: Warn about too-long symbols
    int length;
    for (char* p=*s;; p++) {
        switch (*p) {
            case 'a'...'z':
            case 'A'...'Z':
            case '0'...'9':
            case '!': case '@': case '$': case '%': case '^':
            case '&': case '*': case '_': case '=': case '+':
            case '?': case '>': case '<': case '/': case ':': case '~':
            case '.':
            case '-':
                break;
            default:
                length = p-*s;
                memcpy(SYMBOL_BUF, *s, length);
                SYMBOL_BUF[length]=0;
                *s += length;
                return SYMBOL_BUF;
        }
    }

}


inline void parse_comment(char **s) {
    while (*(++*s) != '\n');
    (*s)++;
}

inline void parse_dot(char **s) { (*s)++; }
inline void parse_close_paren(char **s) { (*s)++; }
inline void parse_open_paren(char **s) { (*s)++; }
inline void parse_whitespace(char **s) { 
    while(1) {
        switch (**s) {
            case ' ': case '\t': case '\n': case '\r': (*s)++;
            default: return;
        }
    }
}

inline enum token_type parse_token_type(char **s) { // Returns next "real" token--skips whitespace and comments
    enum token_type next_type;
    start:
    next_type = detect_token_type(s);
    switch(next_type) {
        case tok_comment: parse_comment(s); goto start;
        case tok_whitespace: parse_whitespace(s); goto start;
        default: return next_type;
    }
}

sexp* parse_list_right(char **s, int atleastone) {
    // We just read "(<init1> <init2> ...".
    // Parse the rest of the list, then return
    enum token_type next_type = parse_token_type(s);
    if (next_type == tok_close_paren) { // )
        parse_close_paren(s);
        return make_empty();
    } else if (next_type == tok_dot) { // . <final>)
        parse_dot(s);
        if (!atleastone) return error(ERR_UNEXPECTED_DOT);
        sexp *last = parse_sexp(s);
        if (!last) return 0;
        if (last->tag == ERROR) return last;
        sexp *rest = parse_list_right(s, 1);
        if (isempty(rest)) {
            return last;
        } else { // We will leak the rest of the list, but it only happens on error
            return error(ERR_UNEXPECTED_DOT);
        }
    } else { // <initial> ...)
        sexp *first = parse_sexp(s);
        sexp *rest = parse_list_right(s, 1);
        return make_cons(first, rest);
    }
}

sexp* parse_sexp(char **s) {
    enum token_type next_type = parse_token_type(s);
    int parsed_int;
    char *parsed_symbol;
    sexp *parsed_constant;
    // Stream of (non-whitespace, non-comment) tokens here
    switch(next_type) {
        case tok_close_paren:
            parse_close_paren(s);
            return error(ERR_UNEXPECTED_CLOSE);
        case tok_comment:
            parse_comment(s);
            return error(ERR_LOGIC);
        case tok_constant:
            parsed_constant = parse_constant(s);
            if (parsed_constant) return parsed_constant;
            else return error(ERR_INVALID_CHAR);
        case tok_dot:
            parse_dot(s);
            return error(ERR_UNEXPECTED_DOT);
        case tok_eof: return 0;
        case tok_invalid: return error(ERR_INVALID_CHAR);
        case tok_number_decimal:
            if(parse_int_decimal(s, &parsed_int)) return make_int(parsed_int);
            else return error(ERR_INVALID_CHAR);
        case tok_number_hex:
            if(parse_int_hex(s, &parsed_int)) return make_int(parsed_int);
            else return error(ERR_INVALID_CHAR);
        case tok_number_octal:
            if(parse_int_octal(s, &parsed_int)) return make_int(parsed_int);
            else return error(ERR_INVALID_CHAR);
        case tok_open_paren:
            parse_open_paren(s);
            return parse_list_right(s, 0);
        case tok_quote: return error(ERR_QUOTE_NOT_IMPL);
        case tok_string:
            parsed_symbol = parse_string(s);
            if (!parsed_symbol) return error(ERR_INVALID_CHAR);
            return make_string(parsed_symbol);
        case tok_symbol:
            parsed_symbol = parse_symbol(s);
            return make_symbol(parsed_symbol);
        case tok_whitespace:
            parse_whitespace(s);
            return error(ERR_LOGIC);
        default: return error(ERR_UNKNOWN_TOKEN);
    }
}

sexp* parse(char *s) {
    sexp *first = parse_sexp(&s);
    if (!first) return make_empty();
    else if (first->tag == ERROR) return first;
    else {
        sexp *rest = parse(s);
        if (rest->tag == ERROR) return rest;
        else return cons(first, rest);
    }
}
