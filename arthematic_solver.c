#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Forward declaration of parse_expr
ASTNode* parse_expr(const char *input, int *pos);

// Token Types
typedef enum {
    TOKEN_INT, TOKEN_PLUS, TOKEN_MINUS, TOKEN_MUL, TOKEN_DIV,
    TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_EOF
} TokenType;

// Token Structure
typedef struct {
    TokenType type;
    char* value;
} Token;

// AST Node Types
typedef enum {
    AST_NUMBER, AST_BINARY_OP
} ASTNodeType;

// AST Node Structure
typedef struct ASTNode {
    ASTNodeType type;
    union {
        struct { struct ASTNode* left; char op; struct ASTNode* right; } binop;
        int val;
    } data;
} ASTNode;

Token* lexer(const char *input, int *pos) {
    while (isspace(input[*pos])) (*pos)++;
    char current_char = input[*pos];

    if (current_char == '\0') {
        Token* token = (Token*)malloc(sizeof(Token));
        token->type = TOKEN_EOF;
        token->value = NULL;
        return token;
    }

    if (isdigit(current_char)) {
        int start = *pos;
        while (isdigit(input[*pos])) (*pos)++;
        char* value = (char*)malloc((*pos - start + 1) * sizeof(char));
        strncpy(value, &input[start], *pos - start);
        value[*pos - start] = '\0';

        Token* token = (Token*)malloc(sizeof(Token));
        token->type = TOKEN_INT;
        token->value = value;
        printf("Lexer: Found NUMBER token with value %s\n", value); // Debug statement
        return token;
    }

    Token* token = (Token*)malloc(sizeof(Token));
    token->value = NULL;
    switch (current_char) {
        case '+': token->type = TOKEN_PLUS; break;
        case '-': token->type = TOKEN_MINUS; break;
        case '*': token->type = TOKEN_MUL; break;
        case '/': token->type = TOKEN_DIV; break;
        case '(': token->type = TOKEN_LPAREN; break;
        case ')': token->type = TOKEN_RPAREN; break;
        default:
            printf("Lexer Error: Unknown character %c\n", current_char);
            exit(1);
    }
    printf("Lexer: Found token %c\n", current_char); // Debug statement
    (*pos)++;
    return token;
}

ASTNode* parse_factor(const char *input, int *pos) {
    Token* token = lexer(input, pos);
    if (token->type == TOKEN_INT) {
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_NUMBER;
        node->data.val = atoi(token->value);
        printf("Parser: Constructed AST node for number %d\n", node->data.val); // Debug statement
        free(token->value);
        free(token);
        return node;
    } else if (token->type == TOKEN_LPAREN) {
        ASTNode* node = parse_expr(input, pos);
        Token* next_token = lexer(input, pos);
        if (next_token->type!= TOKEN_RPAREN) {
            fprintf(stderr, "Error: expected ')'\n");
            exit(1);
        }
        free(next_token);
        return node;
    }
    fprintf(stderr, "Error: expected number or '('\n");
    exit(1);
}

ASTNode* parse_term(const char *input, int *pos) {
    ASTNode* node = parse_factor(input, pos);
    Token* token = lexer(input, pos);

    while (token->type == TOKEN_MUL || token->type == TOKEN_DIV) {
        char op = token->type == TOKEN_MUL? '*' : '/';
        ASTNode* new_node = (ASTNode*)malloc(sizeof(ASTNode));
        new_node->type = AST_BINARY_OP;
        new_node->data.binop.left = node;
        new_node->data.binop.op = op;
        new_node->data.binop.right = parse_factor(input, pos);
        node = new_node;
        printf("Parser: Constructed AST node for operation '%c'\n", op); // Debug statement
        free(token);
        token = lexer(input, pos);
    }
    free(token);
    return node;
}

ASTNode* parse_expr(const char *input, int *pos) {
    ASTNode* node = parse_term(input, pos);
    Token* token = lexer(input, pos);

    while (token->type == TOKEN_PLUS || token->type == TOKEN_MINUS) {
        char op = token->type == TOKEN_PLUS? '+' : '-';
        ASTNode* new_node = (ASTNode*)malloc(sizeof(ASTNode));
        new_node->type = AST_BINARY_OP;
        new_node->data.binop.left = node;
        new_node->data.binop.op = op;
        node = new_node;
        printf("Parser: Constructed AST node for operation '%c'\n", op); // Debug statement
        free(token);
        token = lexer(input, pos);
        node->data.binop.right = parse_expr(input, pos); // Recursively call parse_expr
    }
    free(token);
    return node;
}

void generate_code(ASTNode* node) {
    if (node == NULL) return;
    switch (node->type) {
        case AST_NUMBER:
            printf("Generate Code: Push number %d\n", node->data.val);
            break;
        case AST_BINARY_OP:
            generate_code(node->data.binop.left);
            generate_code(node->data.binop.right);
            printf("Generate Code: Perform operation '%c'\n", node->data.binop.op);
            break;
    }
}

// Free AST
void free_ast(ASTNode* node) {
    if (node == NULL) return;
    switch (node->type) {
        case AST_NUMBER:
            // No additional memory to free if it's a number
            break;
        case AST_BINARY_OP:
            // Recursively free left and right children
            free_ast(node->data.binop.left);
            free_ast(node->data.binop.right);
            break;
    }
    free(node); // Free the node itself
}

int main() {
    char input[100];
    printf("Enter an arithmetic expression: ");
    fgets(input, sizeof(input), stdin);
    int pos = 0;
    ASTNode* root = parse_expr(input, &pos);

    Token* token = lexer(input, &pos);
    // Additional check after parsing to make sure no unprocessed input remains.
    while (isspace(input[pos])) pos++;  // Skip any trailing whitespace after the expression
    if (input[pos] != '\0' && input[pos] != '\n') { // Checking for unprocessed characters
        fprintf(stderr, "Unexpected tokens after expression.\n");
        exit(EXIT_FAILURE);
    }

    generate_code(root);
free_ast(root);
    free(token);

    // Calculate the final result
    int result = 0;
    ASTNode* current_node = root;
    while (current_node->type == AST_BINARY_OP) {
        ASTNode* left_child = current_node->data.binop.left;
        ASTNode* right_child = current_node->data.binop.right;
        switch (current_node->data.binop.op) {
            case '+':
                result += left_child->data.val + right_child->data.val;
                break;
            case '-':
                result += left_child->data.val - right_child->data.val;
                break;
            case '*':
                result += left_child->data.val * right_child->data.val;
                break;
            case '/':
                result += left_child->data.val / right_child->data.val;
                break;
        }
        current_node = left_child;
    }

    // Print the final result
    printf("Result: %d\n", result);

    return 0;
}