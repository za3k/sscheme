#include "parser.h"
#include "errors.h"
#include "prims.h"
#include "constants.h"
#include <stdlib.h>
#include <string.h>

// TODO: String parsing: "abc\n"
// TODO: Simple and advanced cons-notation: (1 . 2) or (3 4 5 . 6)
// TODO: Intern symbols to save space and allow == comparison

sexp* parse_list_right(char **s);
sexp* parse_sexp(char **s);

struct token {
    enum token_type { tok_close_paren, tok_constant, tok_eof, tok_form, tok_number, tok_open_paren, tok_quote, tok_symbol, } tag;
    sexp* atom;
};

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

char* parse_symbol(char **s) {
    int length;
    char *out;
    for (char* p=*s;; p++) {
        switch (*p) {
            case 'a'...'z':
            case 'A'...'Z':
            case '0'...'9':
            case '!': case '@': case '$': case '%': case '^':
            case '&': case '*': case '_': case '=': case '+':
            case '?': case '>': case '<': case '.': case '/':
            case ':': case '~':
            case '-':
                break;
            default:
                length = p-*s;
                out = malloc(length+1);
                memcpy(out, *s, length);
                out[length]=0;
                *s += length;
                return out;
        }
    }

}

struct token* parse_token(char **s) {
    struct token *res = malloc(sizeof(struct token));
    res->atom = 0;
    int parsed_int;
    char *parsed_symbol;
    start:
    switch(**s) {
        case '-':
            switch((*s)[1]) {
                case '0'...'9':
                    if(!parse_int(s, &parsed_int)) {
                        // Not a legal number!
                        free(res);
                        return 0;
                    }
                    res->tag = tok_number;
                    res->atom = make_int(parsed_int);
                    break;
                default:
                    parsed_symbol = parse_symbol(s);
                    res->tag = tok_symbol;
                    res->atom = make_symbol(parsed_symbol);
                    free(parsed_symbol);
                    break;
            }
            break;
        case '0' ... '9': 
            if(!parse_int(s, &parsed_int)) {
                // Not a legal number!
                free(res);
                return 0;
            }
            res->tag = tok_number;
            res->atom = make_int(parsed_int);
            break;
        case ';': // Ignore comments, don't output a token.
            (*s)++;
            while (**s != '\n') (*s)++;
            (*s)++;
            goto start;
        case '(':
            (*s)++;
            res->tag = tok_open_paren;
            break;
        case ')':
            (*s)++;
            res->tag = tok_close_paren;
            break;
        case ' ': case '\t': case '\n': case '\r':
            // Ignore whitespace, don't output a token
            (*s)++;
            goto start;
        case '\'':
            res->tag = tok_quote;
            break;
        case '#':
            switch((*s)[1]) {
                case 't':
                case 'f':
                case '\\':
                    res->tag = tok_constant;
                    res->atom = parse_constant(s);
                    break;
                case 'o':
                    (*s)+=2;
                    if(!parse_int_octal(s, &parsed_int)) { free(res); return 0; }
                    res->tag = tok_number;
                    res->atom = make_int(parsed_int);
                    break;
                case 'd':
                    (*s)+=2;
                    if(!parse_int(s, &parsed_int)) { free(res); return 0; }
                    res->tag = tok_number;
                    res->atom = make_int(parsed_int);
                    break;
                case 'x':
                    (*s)+=2;
                    if(!parse_int_hex(s, &parsed_int)) { free(res); return 0; }
                    res->tag = tok_number;
                    res->atom = make_int(parsed_int);
                    break;
                default: free(res); return 0;
            }
            break;
        case '\0':
            res->tag = tok_eof; // Done parsing!
            break;
        case 'a'...'z':
        case 'A'...'Z':
        case '!': case '@': case '$': case '%': case '^':
        case '&': case '*': case '_': case '=': case '+':
        case '?': case '>': case '<': case '.': case '/':
        case ':': case '~':
            parsed_symbol = parse_symbol(s);
            res->tag = tok_symbol;
            res->atom = make_symbol(parsed_symbol);
            free(parsed_symbol);
            break;
        case '"':
        default:
            free(res);
            return 0; // Nope! Not allowed
    }

    return res;
}

void free_tok(struct token *t) {
    if (t->atom && !(t->tag == tok_form || t->tag == tok_constant)) free(t->atom);
    free(t);
}

int peek_token(char **s) {
    char *old = *s;
    struct token *tok = parse_token(s);
    if (tok == 0) return 0;
    int tag = tok->tag;
    free_tok(tok);
    *s = old; // Reset, thus 'peek' instead of parse
    return tag;
}

sexp* parse_list_right(char **s) {
    // We just read "(". Parse the rest of the list, then return
    if (peek_token(s) == tok_close_paren) {
        free(parse_token(s));
        return make_empty();
    }
    sexp *first = parse_sexp(s);
    sexp *rest = parse_list_right(s);
    return make_cons(first, rest);
}

sexp* parse_sexp(char **s) {
    struct token *next_token;
    sexp *ret;
    next_token = parse_token(s);
    // Stream of (non-whitespace) tokens here
    if (next_token==0) return error(ERR_INVALID_CHAR);
    switch(next_token->tag) {
        case tok_open_paren:
            free_tok(next_token);
            ret = parse_list_right(s);
            break;
        case tok_close_paren:
            ret = error(ERR_UNEXPECTED_CLOSE);
            free_tok(next_token);
            break;
        case tok_form:
        case tok_symbol:
        case tok_number:
        case tok_constant:
            ret = next_token->atom;
            free(next_token);
            break;
        case tok_quote:
            ret = error(ERR_QUOTE_NOT_IMPL);
            free_tok(next_token);
            break;
        case tok_eof:
            free(next_token);
            return 0;
            break;
        default:
            ret = error(ERR_UNKNOWN_TOKEN);
            free_tok(next_token);
            break;
    }
    return ret;
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

