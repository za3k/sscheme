#include "constants.h"
#include "errors.h"
#include "helpers.h"
#include "parser.h"
#include "prims.h"
#include <stdlib.h>
#include <string.h>

// TODO: Intern symbols to save space and allow == comparison
// TODO: Point to where an error is--save the source location of each sexp and token

sexp* parse_list_right(char **s, int atleastone);
sexp* parse_sexp(char **s);

struct token {
    enum token_type { tok_close_paren, tok_constant, tok_dot, tok_eof, tok_form, tok_invalid, tok_number, tok_open_paren, tok_quote, tok_string, tok_symbol } tag;
    sexp* atom;
};

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

int parse_int(char **s, int *result) {
    int out = 0;
    int sign=1;
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

struct token parse_token(char **s) {
    struct token res;
    res.tag = tok_invalid;
    res.atom = 0;
    int parsed_int;
    char *parsed_symbol;
    start:
    switch(**s) {
        case '-':
            switch((*s)[1]) {
                case 0: break;
                case '0'...'9': goto decimal;
                default: goto symbol;
            }
            break;
        case '0' ... '9': 
            decimal:
            if(!parse_int(s, &parsed_int)) break;
            res.tag = tok_number;
            res.atom = make_int(parsed_int);
            break;
        case ';': // Ignore comments, don't output a token.
            while (*(++*s) != '\n');
            (*s)++;
            goto start;
        case '(':
            (*s)++;
            res.tag = tok_open_paren;
            break;
        case ')':
            (*s)++;
            res.tag = tok_close_paren;
            break;
        case ' ': case '\t': case '\n': case '\r':
            // Ignore whitespace, don't output a token
            (*s)++;
            goto start;
        case '\'':
            res.tag = tok_quote;
            break;
        case '#':
            switch((*s)[1]) {
                case 't':
                case 'f':
                case '\\':
                    res.tag = tok_constant;
                    res.atom = parse_constant(s);
                    break;
                case 'o':
                    (*s)+=2;
                    if(!parse_int_octal(s, &parsed_int)) break;
                    res.tag = tok_number;
                    res.atom = make_int(parsed_int);
                    break;
                case 'd': (*s)+=2; goto decimal;
                case 'x':
                    (*s)+=2;
                    if(!parse_int_hex(s, &parsed_int)) break;
                    res.tag = tok_number;
                    res.atom = make_int(parsed_int);
                    break;
            }
            break;
        case '\0':
            res.tag = tok_eof; // Done parsing!
            break;
        case '.':
            switch ((*s)[1]) {
                case ' ': case '\t': case '\n': case '\r': case '\0':
                    (*s)++;
                    res.tag = tok_dot;
                    return res;
            }
            // Fall through and parse a symbol like .foo
        case 'a'...'z':
        case 'A'...'Z':
        case '!': case '@': case '$': case '%': case '^':
        case '&': case '*': case '_': case '=': case '+':
        case '?': case '>': case '<': case '/': case ':': case '~':
            symbol:
            parsed_symbol = parse_symbol(s);
            res.tag = tok_symbol;
            res.atom = make_symbol(parsed_symbol);
            // No need to free because we have only one buffer
            break;
        case '"':
            parsed_symbol = parse_string(s);
            if (!parsed_symbol) break;
            res.tag = tok_string;
            res.atom = make_string(parsed_symbol);
            // No need to free because we have only one buffer
            break;
    }

    return res;
}

void free_tok(struct token *t) {
    if (t->atom && !(t->tag == tok_form || t->tag == tok_constant)) free(t->atom);
    free(t);
}

int peek_token(char **s) {
    char *old = *s;
    struct token tok = parse_token(s);
    *s = old; // Reset, thus 'peek' instead of parse
    return tok.tag;
}

sexp* parse_list_right(char **s, int atleastone) {
    // We just read "(<init1> <init2> ...".
    // Parse the rest of the list, then return
    int peeked = peek_token(s);
    if (peeked == tok_close_paren) { // )
        parse_token(s); // Consume the close-paren
        return make_empty();
    } else if (peeked == tok_dot) { // . <final>)
        parse_token(s); // Consume the dot
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
    struct token next_token = parse_token(s);
    // Stream of (non-whitespace, non-comment) tokens here
    switch(next_token.tag) {
        case tok_constant:
        case tok_form:
        case tok_number:
        case tok_string:
        case tok_symbol:
            return next_token.atom;
        case tok_invalid: return error(ERR_INVALID_CHAR);
        case tok_open_paren: return parse_list_right(s, 0);
        case tok_close_paren: return  error(ERR_UNEXPECTED_CLOSE);
        case tok_quote: return error(ERR_QUOTE_NOT_IMPL);
        case tok_dot: return error(ERR_UNEXPECTED_DOT);
        case tok_eof: return 0;
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
