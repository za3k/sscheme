#include "config.h"
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
sexp* parse_int(char **s, int base);
sexp* parse_string(char **s);
sexp* parse_symbol(char **s);
sexp* parse_character_constant(char**s);
sexp* parse_constant(char**s);
sexp* parse_quote(char **s);
void parse_comment(char **s);
void parse_dot(char **s);
void parse_close_paren(char **s);
void parse_open_paren(char **s);
void parse_whitespace(char **s);

enum token_type parse_token_type(char **s); // Returns next "real" token--skips whitespace and comments
sexp* parse_pair(char **s); // Or any other pair
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

char STRING_BUF[MAX_STRING_SIZE + 1];
sexp* parse_string(char **s) {
    int len=0;
    (*s)++; // Initial quote
    while (1) {
        char p=**s;
        switch(p) {
            case 0: return error(ERR_INVALID_CHAR);
            case '\"':
                (*s)++;
                STRING_BUF[len]=0;
                return make_string(STRING_BUF);
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
                if (len >= MAX_STRING_SIZE) return error(ERR_STRING_TOO_BIG);
        }
        (*s)++;
    }
    return error(ERR_INVALID_CHAR);
}

sexp* parse_int(char **s, int base) {
    int out=0, sign=1, digit;
    if ((*s)[0] == '#') {
        if (((*s)[1]=='d' && base==10) ||
            ((*s)[1]=='o' && base==8) ||
            ((*s)[1]=='x' && base==16)) (*s)+=2;
        else return error(ERR_INVALID_CHAR);
    }
    if (**s == '-') { (*s)++; sign=-1; }
    for (;;(*s)++) {
        switch (**s) {
            case '0' ... '9':
                digit = **s-'0';
                break;
            case 'a' ... 'z':
                digit = **s-'a'+10;
                break;
            case 'A' ... 'Z':
                digit = **s-'A'+10;
                break;
            case ' ': case '\t': case '\n': case '\r':
            case ')': case '(': case '\0':
                return make_int(out*sign);
            default: return error(ERR_INVALID_CHAR);
        }
        if (digit >= base) return error(ERR_INVALID_CHAR);
        out = out * base + digit;
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
    // Assume (*s)=='#'
    switch((*s)[1]) {
        case 't': // #t
            (*s)+=2;
            return TRUE;
        case 'f': // #f
            (*s)+=2;
            return FALSE;
        case '\\': return parse_character_constant(s);
        default: return error(ERR_INVALID_CHAR);
    }
}

char SYMBOL_BUF[MAX_SYMBOL_SIZE + 1];
sexp* parse_symbol(char **s) {
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
                if (length > MAX_SYMBOL_SIZE) return error(ERR_SYMBOL_TOO_BIG);
                memcpy(SYMBOL_BUF, *s, length);
                SYMBOL_BUF[length]=0;
                *s += length;
                return make_symbol(SYMBOL_BUF);
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

sexp* parse_pair_right(char **s, int atleastone) {
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
        if (!last) return error(ERR_UNEXPECTED_DOT);
        if (last->tag == ERROR) return last;
        sexp *rest = parse_pair_right(s, 1);
        if (isempty(rest)) {
            return last;
        } else { // We will leak the rest of the list, but it only happens on error
            return error(ERR_UNEXPECTED_DOT);
        }
    } else { // <initial> ...)
        sexp *first = parse_sexp(s);
        sexp *rest = parse_pair_right(s, 1);
        return make_cons(first, rest);
    }
}

sexp* parse_quote(char **s) {
    (*s)++;
    sexp *rest = parse_sexp(s);
    if (!rest) return error(ERR_QUOTE_UNTERMINATED);
    else if (rest->tag==ERROR) return rest;
    else return make_cons(QUOTE, make_cons(rest, EMPTY_LIST));
}

sexp* parse_pair(char **s) {
    parse_open_paren(s);
    return parse_pair_right(s, 0);
}

sexp* parse_sexp(char **s) {
    enum token_type next_type = parse_token_type(s);
    // Stream of (non-whitespace, non-comment) tokens here
    switch(next_type) {
        case tok_close_paren:
            parse_close_paren(s);
            return error(ERR_UNEXPECTED_CLOSE);
        case tok_comment:
            parse_comment(s);
            return error(ERR_LOGIC);
        case tok_constant: return parse_constant(s);
        case tok_dot:
            parse_dot(s);
            return error(ERR_UNEXPECTED_DOT);
        case tok_eof: return 0;
        case tok_invalid: return error(ERR_INVALID_CHAR);
        case tok_number_decimal: return parse_int(s,10);
        case tok_number_hex: return parse_int(s,16);
        case tok_number_octal: return parse_int(s,8);
        case tok_open_paren: return parse_pair(s);
        case tok_quote: return parse_quote(s);
        case tok_string: return parse_string(s);
        case tok_symbol: return parse_symbol(s);
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
