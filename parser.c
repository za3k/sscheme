#include "parser.h"
#include "errors.h"
#include <stdlib.h>
#include <string.h>

// TODO: String parsing: "abc\n"
// TODO: Character parsing: #\a
// TODO: Hex numbers: 0xFE and 0xfe
// TODO: Simple and advanced cons-notation: (1 . 2) or (3 4 5 . 6)
// TODO: Intern symbols to save space and allow == comparison

sexp* parse_list_right(char **s);
sexp* parse_sexp(char **s);

struct token {
    enum token_type { tok_open_paren, tok_close_paren, tok_symbol, tok_number, tok_quote, tok_form } tag;
    sexp* atom;
};

int parse_int(char **s, int *result) {
    int out = 0;
    int digit;
    for (char* p=*s;; p++) {
        switch (*p) {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                digit = *p-'0';
                out = out * 10 + digit;
                break;
            case ' ': case '\t': case '\n': case '\r':
            case ')': case '(': case '\0':
                *s = p;
                *result = out;
                return 1;
                break;
            default:
                return 0;
        }
    }
}

char* parse_symbol(char **s) {
    int length;
    char *out;
    for (char* p=*s;; p++) {
        switch (*p) {
            case 'a': case 'b': case 'c': case 'd': case 'e':
            case 'f': case 'g': case 'h': case 'i': case 'j':
            case 'k': case 'l': case 'm': case 'n': case 'o':
            case 'p': case 'q': case 'r': case 's': case 't':
            case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
            case 'A': case 'B': case 'C': case 'D': case 'E':
            case 'F': case 'G': case 'H': case 'I': case 'J':
            case 'K': case 'L': case 'M': case 'N': case 'O':
            case 'P': case 'Q': case 'R': case 'S': case 'T':
            case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
            case '!': case '@': case '#': case '$': case '%':
            case '^': case '&': case '*': case '-': case '_':
            case '=': case '+': case '?': case '>': case '<':
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
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
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            if(!parse_int(s, &parsed_int)) {
                // Not a legal number!
                free(res);
                return 0;
            }
            res->tag = tok_number;
            res->atom = make_int(parsed_int);
            break;
        case '(':
            (*s)++;
            res->tag = tok_open_paren;
            break;
        case ')':
            (*s)++;
            res->tag = tok_close_paren;
            break;
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            (*s)++;
            goto start;
            break;
        case '\'':
            res->tag = tok_quote;
            break;
        case '\0':
            free(res);
            return 0; // Done parsing!
            break;
        case 'a': case 'b': case 'c': case 'd': case 'e':
        case 'f': case 'g': case 'h': case 'i': case 'j':
        case 'k': case 'l': case 'm': case 'n': case 'o':
        case 'p': case 'q': case 'r': case 's': case 't':
        case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
        case 'A': case 'B': case 'C': case 'D': case 'E':
        case 'F': case 'G': case 'H': case 'I': case 'J':
        case 'K': case 'L': case 'M': case 'N': case 'O':
        case 'P': case 'Q': case 'R': case 'S': case 'T':
        case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
        case '!': case '@': case '#': case '$': case '%':
        case '^': case '&': case '*': case '-': case '_':
        case '=': case '+': case '?': case '>': case '<':
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
    if (t->atom && !(t->tag == tok_form)) free(t->atom);
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
            ret = next_token->atom;
            free(next_token);
            break;
        case tok_quote:
            ret = error(ERR_QUOTE_NOT_IMPL);
            free_tok(next_token);
            break;
        default:
            ret = error(ERR_UNKNOWN_TOKEN);
            free_tok(next_token);
            break;
    }
    return ret;
}

sexp* parse(char *s) {
    return parse_sexp(&s); // Ignore anything after it for simplicity.
}

