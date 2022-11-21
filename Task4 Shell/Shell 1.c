#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FILE *in, *out;

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

void print_list(Node *root) {
    while (root != NULL) {
        if ((strcmp(root->info, ""))) {
            printf("%s\n", root->info);
        }
        root = root->next;
    }
}

/*
void clear_list(Node *root, Node **end_of_list_address){
    Node *tmp;
    *end_of_list_address = root;
    while (root != NULL) {
        tmp = root;
        if ((strcmp(tmp->info, ""))) {
            free(tmp->info);
            free(tmp);
        }
        root = root->next;
    }
}
*/


void clear_list(Node **root_address, Node **end_of_list_address){
    Node *tmp_list = *root_address;
    Node *tmp_node;
    while (tmp_list != NULL) {
        tmp_node = tmp_list;
        tmp_list = tmp_list->next;
        if (strcmp(tmp_node->info, "")) {
            free(tmp_node->info);
            free(tmp_node);
        }
    }
    *end_of_list_address = *root_address;
}


/*
void clear_list(Node **root, Node **end_of_list) {
    Node *tmp;
    while ((*root) != NULL) {
        tmp = *root;
        if ((strcmp(tmp->info, ""))) {
            free(tmp->info);
        }
        *root = (*root)->next;
        free(tmp);
    }
}
*/

void read_word(int *flag_eof, int *flag_eol, int *len_word, Node **end_of_list) {
    char *word;
    int c;
    int i = 0, len = *len_word;
    int flag_quote = 0, flag_and = 0, flag_or = 0, flag_arrow = 0;
    word = calloc(len + 1, sizeof (char));
    while (((c = fgetc(in)) != EOF) && (c != '\n')) {
        // printf("%c", c);
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
                push(end_of_list, word, len);  //add to list
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
                push(end_of_list, word, len);  //add to list
                free(word);
                break;
            }
            else {
                if (word != NULL) {
                    word[i] = '\0';
                    push(end_of_list, word, len);  //add to list
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
                    push(end_of_list, word, len);  //add to list
                    free(word);
                    break;
                }
            }
        }
        else {
            if ((flag_and) || (flag_or) || (flag_arrow)) {
                word[i] = '\0';
                push(end_of_list, word, len);  //add to list
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
        *flag_eol = (c == '\n') ? 0 : 1;
        *flag_eof = (c == EOF) ? 0 : 1;
        if (word != NULL) {
            word[i] = '\0';
            push(end_of_list, word, len);  //add to list
            free(word);
        }
    }
    *len_word = len;
}

int main(int argc, char *argv[]) {

    int flag_eof = 1, flag_eol = 1, len_word = 50;
    Node *root = (Node*)malloc(sizeof(Node));
    root->info = "";
    root->next = NULL;
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
    read_word(&flag_eof, &flag_eol, &len_word, &end);
    print_list(&root);
    */

    while (flag_eof) {
        while ((flag_eof) && (flag_eol)) {
            read_word(&flag_eof, &flag_eol, &len_word, &end);
        }
        flag_eol = 1;
        print_list(root);
        clear_list(&root, &end);
    }
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