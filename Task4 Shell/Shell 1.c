#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FILE *in, *out;
int c;

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

void readword(int *flag_eof, int *flag_eol, int *len_word, Node *last_elem) {
    char *word;
    int i = 0, len = *len_word;
    int flag_quote = 0, flag_and = 0, flag_or = 0, flag_arrow = 0;
    word = calloc(len + 1, sizeof (char));
    while ((c = fgetc(in) != EOF) && (c != '\n')) {
        if (c == '\"') {
            flag_quote++;
        }
        else if (flag_quote == 1) {
            word[i++] = c;
        }
        else if (flag_quote == 2) {
            flag_quote = 0;
        }
        // Everything lower is when text not in quotes:
        else if (c == ' ') {
            if (word != NULL) {
                word[i] = '\0';
                // add to list
                free(word);
            }
            break;
        }
        // if it's a special symbol let's add the word
        else if ((c == '&') || (c == '|') || (c == ';') || (c == '>')
            || (c == '<') || (c == ')') || (c == '(')) {
            if (((flag_and) && (c == '&')) || ((flag_or) && (c == '|')) ||
                    ((flag_arrow) && (c == '>'))) {
                word[i++] = c;
                word[i] = '\0';
                // add to list
                free(word);
                break;
            }
            else {
                if (word != NULL) {
                    word[i] = '\0';
                    // add to list
                    free(word);
                    len = 50, i = 0;
                    word = calloc(len + 1, sizeof(char));
                }
                word[i++] = c;
                if (c == '&') {
                    flag_and++;
                } else if (c == '|') {
                    flag_or++;
                } else if (c == '>') {
                    flag_arrow++;
                } else {
                    word[i] = '\0';
                    // add to list
                    free(word);
                    break;
                }
            }
        }
        else {
            if ((flag_and) || (flag_or) || (flag_arrow)) {
                word[i] = '\0';
                // add to list
                free(word);
                i = 0;
                word = calloc(len + 1, sizeof (char));
            }
            word[i++] = c;
        }
        if (i >= len - 3) {
            len *= 2;
            word = realloc(word, (len + 1) * sizeof (char));
        }
    }

    if ((c == '\n') || (c == EOF)){
        *flag_eol = (c == '\n');
        *flag_eof = (c == EOF);
        if (word != NULL) {
            word[i] = '\0';
            // add to list
            free(word);
        }
    }
    *len_word = len;
}













int main(int argc, char *argv[]) {

    int flag_eof = 0, flag_eol = 0, len_word = 50;
    char *word;
    Node *root = NULL;
    Node *end = root;

    if (argc == 5) {
        in = fopen("r", argv[2]);
        out = fopen("r", argv[4]);
    }
    else if (argc == 3) {
        if (strcmp(argv[1], "-i") == 0) {
            in = fopen("r", argv[2]);
            out = stdout;
        }
        else if (strcmp(argv[1], "-o") == 0) {
            in = stdin;
            out = fopen("r", argv[2]);
        }
    }
    else if (argc == 1) {
        in = stdin;
        out = stdout;
    }
    else {
        return EXIT_FAILURE;
    };

    /*
    while (!flag_eof) {
        while ((!flag_eof) && (!flag_eol)) {

        }
    }
     */

}

/*
 * Идея алгоритма:
 * Пока не конец файла:
 *      Создали список
 *      Пока не конец строки и не конец файла:
 *          Считываем слово и сразу добавляем его в список
 *      Выводим список слов
 *      Очищаем список
 */