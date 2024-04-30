#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

// Enumeration to represent the types of tokens in arithmetic expressions
typedef enum {
    T_NUMBER,   // Represents a numeric value
    T_PLUS,     // Represents the '+' operator
    T_MINUS,    // Represents the '-' operator
    T_MUL,      // Represents the '*' operator
    T_DIV,      // Represents the '/' operator
    T_LPAREN,   // Represents the '(' operator
    T_RPAREN,   // Represents the ')' operator
    T_EOF       // End of File token used to signify the end of input
} TokenType;

// Structure to hold the token data
typedef struct Token {
    TokenType type;   // Type of token
    double value;     // Holds the numeric value if it's a number
} Token;

// Function prototypes
Token *get_next_token(const char **input);
void eat(TokenType type, const char **input);
double expr(const char **input);

Token *cur_token = NULL; // Global variable to hold the current token

// Error handling function using variadic arguments
void error(const char *fmt, ...) {
    va_list args;
    fprintf(stderr, "Error: ");
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    exit(1);
}

// Retrieves the next token from the input
Token *get_next_token(const char **input) {
    const char *s = *input;
    while (isspace(*s) && *s != '\n') s++; // Skip whitespace but not newline

    Token *t = calloc(1, sizeof(Token));
    if (isdigit(*s) || (*s == '.' && isdigit(*(s + 1)))) { // Check if it's a number
        t->type = T_NUMBER;
        t->value = strtod(s, (char **)&s);
        printf("Token: Number (%f)\n", t->value);
    } else if (*s == '\n' || *s == '\0') { // Newline or NULL character
        t->type = T_EOF;
    } else {
        switch (*s) {
            case '+': t->type = T_PLUS; break;
            case '-': t->type = T_MINUS; break;
            case '*': t->type = T_MUL; break;
            case '/': t->type = T_DIV; break;
            case '(': t->type = T_LPAREN; break;
            case ')': t->type = T_RPAREN; break;
            default: error("Unexpected character: %c", *s);
        }
        printf("Token: %c\n", *s);
        s++;
    }
    *input = s;
    return t;
}

// Advances the current token if it matches the expected type
void eat(TokenType type, const char **input) {
    if (cur_token->type == type) {
        free(cur_token);
        cur_token = get_next_token(input);
    } else {
        error("Unexpected token: %d, expected: %d", cur_token->type, type);
    }
}

// Parses a factor from the expression
double factor(const char **input) {
    printf("Parse factor\n");
    if (cur_token->type == T_NUMBER) {
        double value = cur_token->value;
        eat(T_NUMBER, input);
        return value;
    } else if (cur_token->type == T_LPAREN) {
        eat(T_LPAREN, input);
        double value = expr(input);
        eat(T_RPAREN, input);
        return value;
    } else {
        error("Invalid token for factor: %d", cur_token->type);
    }
}

// Parses a term from the expression
double term(const char **input) {
    printf("Parse term\n");
    double value = factor(input);
    while (cur_token->type == T_MUL || cur_token->type == T_DIV) {
        if (cur_token->type == T_MUL) {
            eat(T_MUL, input);
            value *= factor(input);
        } else if (cur_token->type == T_DIV) {
            eat(T_DIV, input);
            value /= factor(input);
        }
    }
    return value;
}

// Parses an expression from the input
double expr(const char **input) {
    printf("Parse expression\n");
    double value = term(input);
    while (cur_token->type == T_PLUS || cur_token->type == T_MINUS) {
        if (cur_token->type == T_PLUS) {
            eat(T_PLUS, input);
            value += term(input);
        } else if (cur_token->type == T_MINUS) {
            eat(T_MINUS, input);
            value -= term(input);
        }
    }
    return value;
}

// Main function
int main() {
    printf("Enter an expression and press enter: ");
    fflush(stdout); // Make sure prompt is visible immediately
    char input[1024];
    if (fgets(input, sizeof(input), stdin) != NULL) {
        const char *input_ptr = input;
        cur_token = get_next_token(&input_ptr);
        double result = expr(&input_ptr);
        printf("Result: %f\n", result);
        free(cur_token);
    }
    return 0;
}
