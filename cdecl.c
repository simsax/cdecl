#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TOKEN_LEN 64
#define MAX_TOKENS 256
#define ERROR(msg) fprintf(stderr, "ERROR: %s\n", (msg))

typedef enum {
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_SQUARE,
    TOKEN_RIGHT_SQUARE,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_SEMICOLON,
    TOKEN_ASTERISK,
    TOKEN_TYPE,
    TOKEN_QUALIFIER,
    TOKEN_STORAGE,
    TOKEN_IDENTIFIER,
    TOKEN_UNKNOWN,
    TOKEN_END
} TokenType;

typedef struct {
	TokenType type;
	char name[MAX_TOKEN_LEN];
} Token;

typedef struct {
    Token stack[MAX_TOKENS];
    int index; 
} TokenStack;

TokenStack token_stack;
Token this_token;
int current_index;

void push(Token token) {
    if (token_stack.index < MAX_TOKENS) {
        token_stack.stack[token_stack.index++] = token;
    } else {
        ERROR("Token stack is full.");
        exit(EXIT_FAILURE);
    }
}

Token pop() {
    if (token_stack.index > 0) {
        token_stack.index--;
        Token token = token_stack.stack[token_stack.index];
        return token;
    } else {
        ERROR("Token stack is empty.");
        exit(EXIT_FAILURE);
    }
}

Token* peek() {
    if (token_stack.index > 0) {
        Token* token = &token_stack.stack[token_stack.index - 1];
        return token;
    } else {
        ERROR("Token stack is empty.");
        exit(EXIT_FAILURE);
    }
}

void make_single_char_token(Token* token, char c, TokenType type) {
    token->name[0] = c;
    token->name[1] = 0;
    token->type = type;
}

static bool is_alphanumeric(char c) {
    return ((c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') || 
             c == '_');
}

static bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

void make_identifier_token(Token* token, const char* declaration, int start, int end) {
    memcpy(token->name, declaration + start, end - start);
    token->name[end - start] = 0;
    token->type = TOKEN_IDENTIFIER;
}

void check_keyword(Token* token, const char* declaration, int start, int end, const char* name, TokenType type) {
    if (memcmp(declaration + start, name, end - start) == 0) {
        strcpy(token->name, name);
        token->type = type;
    } else {
        make_identifier_token(token, declaration, start, end);
    }
}

void classify_string(Token* token, const char* declaration, int decl_len) {
    int start_index = current_index;
    while (current_index < decl_len && is_alphanumeric(declaration[current_index])) {
        current_index++;
    }
    switch (declaration[start_index]) {
        case 'v':
            if (current_index >= start_index + 1) {
                switch (declaration[start_index + 1]) {
                    case 'o':
                        if (current_index >= start_index + 2) {
                            switch (declaration[start_index + 2]) {
                                case 'i':
                                    check_keyword(token, declaration, start_index, current_index, "void", TOKEN_TYPE);
                                    return;
                                case 'l':
                                    check_keyword(token, declaration, start_index, current_index, "volatile", TOKEN_QUALIFIER);
                                    return;
                            }
                        }
                        break;
                }
            }
            break;
        case 'c':
            if (current_index >= start_index + 1) {
                switch (declaration[start_index + 1]) {
                    case 'h':
                        check_keyword(token, declaration, start_index, current_index, "char", TOKEN_TYPE);
                        return;
                    case 'o':
                        check_keyword(token, declaration, start_index, current_index, "const", TOKEN_QUALIFIER);
                        return;
                }
            }
            break;
        case 's':
            if (current_index >= start_index + 1) {
                switch (declaration[start_index + 1]) {
                    case 'h':
                        check_keyword(token, declaration, start_index, current_index, "short", TOKEN_TYPE);
                        return;
                    case 'i':
                        check_keyword(token, declaration, start_index, current_index, "signed", TOKEN_TYPE);
                        return;
                    case 't':
                        if (current_index >= start_index + 2) {
                            switch (declaration[start_index + 2]) {
                                case 'r':
                                    check_keyword(token, declaration, start_index, current_index, "struct", TOKEN_TYPE);
                                    return;
                                case 'a':
                                    check_keyword(token, declaration, start_index, current_index, "static", TOKEN_STORAGE);
                                    return;
                            }
                        }
                        break;
                }
            }
            break;
        case 'i':
            check_keyword(token, declaration, start_index, current_index, "int", TOKEN_TYPE);
            return;
        case 'l':
            check_keyword(token, declaration, start_index, current_index, "long", TOKEN_TYPE);
            return;
        case 'u':
            if (current_index >= start_index + 1) {
                switch (declaration[start_index + 1]) {
                    case 'n':
                        if (current_index >= start_index + 2) {
                            switch (declaration[start_index + 2]) {
                                case 's':
                                    check_keyword(token, declaration, start_index, current_index, "unsigned", TOKEN_TYPE);
                                    return;
                                case 'i':
                                    check_keyword(token, declaration, start_index, current_index, "union", TOKEN_TYPE);
                                    return;
                            }
                        }
                        break;
                }
            }
            break;
        case 'f':
            check_keyword(token, declaration, start_index, current_index, "float", TOKEN_TYPE);
            return;
        case 'd':
            check_keyword(token, declaration, start_index, current_index, "double", TOKEN_TYPE);
            return;
        case 'e':
            if (current_index >= start_index + 1) {
                switch (declaration[start_index + 1]) {
                    case 'n':
                        check_keyword(token, declaration, start_index, current_index, "enum", TOKEN_TYPE);
                        return;
                    case 'x':
                        check_keyword(token, declaration, start_index, current_index, "extern", TOKEN_STORAGE);
                        return;
                }
            }
            break;
    }
    make_identifier_token(token, declaration, start_index, current_index);
}

Token get_token(const char* declaration, int decl_len) {
    Token token;
    char current_char;

    // skip whitespace
    for (;;) {
        if (current_index >= decl_len) {
            token.type = TOKEN_END;
            strcpy(token.name, "end");
            return token;
        }
        current_char = declaration[current_index];
        bool is_whitespace = (
                current_char == ' '  ||
                current_char == '\n' ||
                current_char == '\r' ||
                current_char == '\t');
        if (!is_whitespace) {
            break;
        }
        current_index++;
    }

    if (is_alphanumeric(current_char)) {
        classify_string(&token, declaration, decl_len);
    } else {
        switch (current_char) {
            case '(':
                make_single_char_token(&token, current_char, TOKEN_LEFT_PAREN);
                break;
            case ')':
                make_single_char_token(&token, current_char, TOKEN_RIGHT_PAREN);
                break;
            case '[':
                make_single_char_token(&token, current_char, TOKEN_LEFT_SQUARE);
                break;
            case ']':
                make_single_char_token(&token, current_char, TOKEN_RIGHT_SQUARE);
                break;
            case '{':
                make_single_char_token(&token, current_char, TOKEN_LEFT_BRACE);
                break;
            case '}':
                make_single_char_token(&token, current_char, TOKEN_RIGHT_BRACE);
                break;
            case ';':
                make_single_char_token(&token, current_char, TOKEN_SEMICOLON);
                break;
            case '*': 
                make_single_char_token(&token, current_char, TOKEN_ASTERISK);
                break;
            default:
                {
                    token.type = TOKEN_UNKNOWN;
                    strcpy(token.name, "unknown");
                }
                break;
        }
        current_index++;
    }
    return token;
}

void read_to_first_identifier(const char* declaration, int decl_len) {
    current_index = 0;
    while (true)
    {
        this_token = get_token(declaration, decl_len);
        if (this_token.type == TOKEN_IDENTIFIER) {
            printf("%s is ", this_token.name);
            break;
        }
        push(this_token);
    }
    this_token = get_token(declaration, decl_len);
    return;
}

void deal_with_function_args(const char* declaration, int decl_len) {
    // expect no args for now
    while (this_token.type != TOKEN_RIGHT_PAREN) {
        this_token = get_token(declaration, decl_len);
    }
    printf("function returning ");
    this_token = get_token(declaration, decl_len);
    return;
}

void deal_with_arrays(const char* declaration, int decl_len) {
    printf("array of ");
    while (true) {
        this_token = get_token(declaration, decl_len);
        if (this_token.type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "ERROR: Expected array length, but found '%s'.\n", this_token.name);
            exit(EXIT_FAILURE);
        }
        printf("[%s]", this_token.name);
        this_token = get_token(declaration, decl_len);
        if (this_token.type != TOKEN_RIGHT_SQUARE) {
            fprintf(stderr, "ERROR: Expected ']', but found '%s'.\n", this_token.name);
            exit(EXIT_FAILURE);
        }
        this_token = get_token(declaration, decl_len);
        if (this_token.type != TOKEN_LEFT_SQUARE) {
            printf(" ");
            return;
        }
    }
}

void deal_with_any_pointers() {
    while (peek()->type == TOKEN_ASTERISK) {
        printf("pointer to ");
        pop();
    }
}

void deal_with_declarator(const char* declaration, int decl_len) {
    if (this_token.type == TOKEN_LEFT_SQUARE) {
        deal_with_arrays(declaration, decl_len);
    } else if (this_token.type == TOKEN_LEFT_PAREN) {
        deal_with_function_args(declaration, decl_len);
    }
    deal_with_any_pointers();
    while (token_stack.index > 0) {
        if (peek()->type == TOKEN_LEFT_PAREN) {
            pop();
            if (this_token.type != TOKEN_RIGHT_PAREN) {
                fprintf(stderr, "ERROR: Expected ')' but found '%s'.\n", this_token.name);
                exit(EXIT_FAILURE);
            }
            this_token = get_token(declaration, decl_len);
            deal_with_declarator(declaration, decl_len);
        } else {
            Token top_of_stack = pop();
            if (top_of_stack.type == TOKEN_ASTERISK)
                printf("pointer to ");
            else if (strcmp(top_of_stack.name, "const") == 0)
                printf("read-only ");
            else if (strcmp(top_of_stack.name, "volatile") == 0)
                printf("volatile ");
            else {
                // read remaning tokens
                for (int i = 0; i <= token_stack.index; i++)
                    printf("%s ", token_stack.stack[i].name);
                return;
            }
        }
    }
    return;
}

int main(int argc, char** argv) {
    for (;;) {
        token_stack.index = 0;
        printf("Declaration: ");
        char* declaration = NULL;
        size_t decl_len;
        getline(&declaration, &decl_len, stdin);
        decl_len = strlen(declaration);
        read_to_first_identifier(declaration, decl_len);
        deal_with_declarator(declaration, decl_len);
        printf("\n");
        free(declaration);
    }
	return 0;
}

// char* const *(*next)();
// char *(*c[10])();
