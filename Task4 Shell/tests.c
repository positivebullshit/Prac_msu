#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Node{
    char *info;
    struct Node *next;
} Node;

void push(Node **end_of_list, char *word, int len_word) {
    Node *tmp = (Node*)malloc(sizeof(Node));
    tmp->info = calloc(len_word + 1, sizeof (char));
    strcpy(tmp->info, word);
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


int main() {
    int lenword = 100;
    char *word1 = "Hello, world";
    char *word2 = "Hello, I ...";
    char *word3 = "I'm sorry, it was a joke, don't kill me";

    Node *root = (Node*)malloc(sizeof(Node));
    root->info = "\0";
    root->next = NULL;
    Node *end = root;

    push(&end, word1, lenword);
    push(&end, word2, lenword);
    push(&end, word3, lenword);

    print_list(&root);
}