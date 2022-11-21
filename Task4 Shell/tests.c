#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Node{
    char *info;
    struct Node *next;
} Node;

void push(Node **end_of_list, char **word, int len_word) {
    Node *tmp = (Node*)malloc(sizeof(Node));
    tmp->info = *word;
    tmp->next = NULL;
    (*end_of_list)->next = tmp;
    *end_of_list = tmp;
}

void print_list(Node **root) {
    while ((*root) != NULL) {
        if ((strcmp((*root)->info, ""))) {
            printf("%s\n", (*root)->info);
        }
        *root = (*root)->next;
    }
}

void new_read_word() {
    int c;
    while (((c = fgetc(stdin)) != EOF) && (c != '\n')) {
        printf("%c", c);
    }
}

void read_word(int *flag_eof, int *flag_eol, int *len_word, Node **end_of_list) {
    int c;
    int i = 0, len = *len_word;
    int flag_quote = 0, flag_and = 0, flag_or = 0, flag_arrow = 0;
    while (((c = fgetc(stdin)) != EOF) && (c != '\n')) {
        printf("%c", c);
    }
}

int main() {
    int lenword = 100;
    char *word1 = "Hello, world";
    char *word2 = "Hello, I ...";
    char *word3 = "I'm sorry, it was a joke, don't kill me";

    Node *root = (Node*)malloc(sizeof(Node));
    root->info = "";
    root->next = NULL;
    Node *end = root;

    push(&end, &word1, lenword);
    word1 = NULL;
    printf("%s\n", word1);

    print_list(&root);
}