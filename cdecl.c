#include <stdio.h>
#include <stdlib.h>

#define MAX_TOKEN_LEN 64
#define MAX_TOKENS 256

typedef struct {
	char type[16];
	char name[MAX_TOKEN_LEN];
} Token;

typedef struct {
    Token stack[MAX_TOKENS];
    int index; 
} TokenStack;

TokenStack token_stack;
int current_index;

char* classify_string(const char* string) {
    // type, qualifier, identifier
	return "";
}

void make_single_char_token(Token *token, char c) {
    token->type[0] = c;
    token->type[1] = 0;
    strcpy(token->name, token->type);
}

void print_token(Token *token) {
    printf("%s\t|%s\n", token->type, token->name);
}

void print_token_stack() {
    for (int i = 0; i < token_stack.index; i++) 
    {
        print_token(&token_stack.stack[i]);
    }
}

Token get_token(const char* declaration) {
    Token token;
    char c = declaration[current_index++];
    switch (c) {
        case '(':
        case ')':
        case '[':
        case ']':
        case '{':
        case '}':
        case ';':
        case '*': 
            {
                make_single_char_token(
                        &token, c);
                break;
            }
        case ' ':
        case '\n':
        case '\r':
        case '\t':

        default:
            break;
    }
    return token;
}

void read_to_first_identifier(const char* declaration) {
    current_index = 0;
    int decl_len = strlen(declaration);
    while (current_index < decl_len)
    {
        Token this = get_token(declaration);
        if (strcmp(this.type, "identifier") == 0)
            break;
        token_stack.stack[token_stack.index++] = this;
    }
    return;
}

void deal_with_function_args() {
    return;
}

void deal_with_arrays() {
    return;
}

void deal_with_pointers() {
    return;
}

void deal_with_declarator() {
    return;
}

int main(int argc, char** argv) {
    for (;;) {
        token_stack.index = 0;
        char declaration[256];
        printf("Declaration: ");
        scanf("%s", declaration);
        read_to_first_identifier(declaration);

        print_token_stack();

        deal_with_declarator();
        //printf("%s\n", declaration);
    }
	return 0;
}
