#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Node{
    char *info;
    struct Node *next;
} Node;

void push(Node **end_of_list_address, char *word_address) {
    Node *tmp = calloc(1, sizeof (Node));
    tmp->info = word_address;
    tmp->next = NULL;
    (*end_of_list_address)->next = tmp;
    *end_of_list_address = tmp;
}

void print_list(Node *root) {
    while (root != NULL) {
        if ((strcmp(root->info, ""))) {
            fprintf(stdout, "%s\n", root->info);
        }
        root = root->next;
    }
}

void clear_list(Node **root, Node **end_of_list_address) {
    *end_of_list_address = *root;
    Node *tmp;
    while (*root != NULL) {
        tmp = *root;
        *root = (*root)->next;
        if ((strcmp(tmp->info, ""))) {
            free(tmp->info);
            free(tmp);
        }
    }
}

void new_read_word() {
    int c;
    while (((c = fgetc(stdin)) != EOF) && (c != '\n')) {
        printf("%c", c);
    }
}

/*
void read_word(int *flag_eof, int *flag_eol, int *len_word, Node **end_of_list) {
    int c;
    int i = 0, len = *len_word;
    int flag_quote = 0, flag_and = 0, flag_or = 0, flag_arrow = 0;
    while (((c = fgetc(stdin)) != EOF) && (c != '\n')) {
        printf("%c", c);
    }
}
*/

void print_word(char **word) {
    int len = strlen(*word);
    for(int i = 0; i < len; i++) {
        printf("%c\n", (*word)[i]);
    }
}
int main() {
    Node *root = calloc(1, sizeof (Node));
    root->info = "";
    root->next = NULL;
    Node *end = root;

    int lenword = 100;

    char *word = calloc(lenword + 1, sizeof (char));
    word[0] = 'd';
    word[1] = 'o';
    word[2] = 'm';
    word[3] = '\0';

    push(&end, word);

    word = calloc(lenword + 1, sizeof (char));
    word[0] = 'e';
    word[1] = 'd';
    word[2] = 'a';
    word[3] = '\0';

    push(&end, word);

    word = calloc(lenword + 1, sizeof (char));
    word[0] = 'm';
    word[1] = 'o';
    word[2] = 'm';
    word[3] = '\0';

    push(&end, word);

    print_list(root);

    clear_list(&root, &end);

    print_list(root);

    free(root);
    //free(end);

}