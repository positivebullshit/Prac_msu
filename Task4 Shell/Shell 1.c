/*
 * Идея алгоритма
 * Создали список
 * Пока не конец файла:
 *      Пока не конец строки и не конец файла:
 *          Считываем слово и сразу добавляем его в список
 *      Выводим список слов
 *      Очищаем список
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

FILE *in, *out;

typedef struct Node{
    char *info;
    struct Node *next;
} Node;

int number_of_nodes(Node *root) {
    int counter = 0;
    while (root != NULL) {
        counter++;
        root = root->next;
    }
    return counter;
}

void print_list(Node *root) {
    while (root != NULL) {
        if ((strcmp(root->info, ""))) {
            fprintf(out, "%s\n", root->info);
        }
        root = root->next;
    }
}

// adding node in the end of singly linked list
void push(Node **end_of_list_address, char **word_address) {
    Node *tmp = calloc(1, sizeof (Node));
    tmp->info = *word_address;
    tmp->next = NULL;
    (*end_of_list_address)->next = tmp;
    *end_of_list_address = tmp;
}

void clear_list(Node *root, Node **end_of_list_address) {
    *end_of_list_address = root;
    Node *tmp;
    while (root != NULL) {
        tmp = root;
        root = root->next;
        if ((strcmp(tmp->info, ""))) {
            free(tmp->info);
            free(tmp);
        }
    }
    (*end_of_list_address)->next = NULL;
}

void list_to_array(Node *root, char **array, int number_of_words) {
    Node *tmp;
    int i;
    tmp = root->next;
    for (i = 0; i < number_of_words - 1; i++) {
        array[i] = tmp->info;
        tmp = tmp->next;
    }
    array[i] = NULL;
}

void read_word(int *flag_eof, int *flag_eol, int *len_word, Node **end_of_list) {

    int c, i = 0, len = *len_word, flag_quote = 0, flag_and = 0, flag_or = 0, flag_arrow = 0;
    char *word;
    word = calloc(len + 1, sizeof (char));

    while (((c = fgetc(in)) != EOF) && (c != '\n')) {
        if (c == '\"') {
            flag_quote++;

            // reset the flags for the case when Spec1"Spec1"
            // (for example &"&") or it will show:
            // Spec1Spec1       &&
            if ((flag_and) || (flag_or) || (flag_arrow)) {
                word[i] = '\0';
                push(end_of_list, &word);
                flag_and = flag_or = flag_arrow = 0;
                len = 50, i = 0;
                word = calloc(len, sizeof(char));
            }

            if (flag_quote == 2) {
                flag_quote = 0;
            }

        }
        else if (flag_quote == 1) {
            word[i++] = c;
        }
        // Everything lower for the case when text is not in quotes:
        else if (c == ' ') {
            if (strcmp(word, "")) {
                word[i] = '\0';
                push(end_of_list, &word);
            }
            break;
        }
        // if it's a special symbol let's add the previous word
        else if ((c == '&') || (c == '|') || (c == ';') || (c == '>')
            || (c == '<') || (c == ')') || (c == '(')) {
            // if it's paired special symbol:
            if (((flag_and) && (c == '&')) || ((flag_or) && (c == '|')) ||
            ((flag_arrow) && (c == '>'))) {
                word[i++] = c;
                word[i] = '\0';
                push(end_of_list, &word);
                break;
            }
            else {
                // add the previous word
                if (strcmp(word, "")) {
                    word[i] = '\0';
                    push(end_of_list, &word);
                    len = 50, i = 0;
                    word = calloc(len + 1, sizeof(char));

                    // reset the flags for the case when Spec1Spec2Spec1
                    // (for example &|&) or it will show:
                    // Spec1            &
                    // Spec2Spec1       |&
                    flag_and = flag_or = flag_arrow = 0;
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
                    push(end_of_list, &word);
                    break;
                }
            }
        }
        else {
            if ((flag_and) || (flag_or) || (flag_arrow)) {
                word[i] = '\0';
                push(end_of_list, &word);
                len = 50, i = 0;
                word = calloc(len + 1, sizeof(char));
                flag_and = flag_or = flag_arrow = 0;
            }
            word[i++] = c;
        }
        if (i >= len - 3) {
            len *= 2;
            word = realloc(word, (len + 1) * sizeof (char));
        }
    }

    if ((c == '\n') || (c == EOF)) {
        *flag_eol = (c == '\n') ? 0 : 1;
        *flag_eof = (c == EOF) ? 0 : 1;

        // add the last word of the line
        if (!(strcmp(word, ""))) {
            free(word);
        } else {
            word[i] = '\0';
            push(end_of_list, &word);
        }
    }
    else if (!(strcmp(word, ""))) {
        free(word);
    }
    *len_word = len;
}

int main(int argc, char *argv[]) {

    int flag_eof = 1, flag_eol = 1, len_word = 50, number_of_words = 0;

    Node *root = (Node*)malloc(sizeof(Node));  // root-pointer
    root->info = "";
    root->next = NULL;
    Node *end_of_list = root;

    if (argc == 5) {
        in = fopen(argv[2], "r");
        out = fopen(argv[4], "w");
    }
    else if (argc == 3) {
        if (strcmp(argv[1], "-i") == 0) {
            in = fopen(argv[2], "r");
            out = stdout;
        }
        else if (strcmp(argv[1], "-o") == 0) {
            in = stdin;
            out = fopen(argv[2], "w");
        }
    }
    else if (argc == 1) {
        in = stdin;
        out = stdout;
    }
    else {
        return EXIT_FAILURE;
    }


    while (flag_eof) {
        while ((flag_eof) && (flag_eol)) {
            read_word(&flag_eof, &flag_eol, &len_word, &end_of_list);
        }
        flag_eol = 1;

        number_of_words = number_of_nodes(root);
        char **array = calloc(number_of_words + 1, sizeof (char*));
        list_to_array(root, array, number_of_words);

        if (array[0] != NULL) {
            if (!(strcmp(array[0], "cd"))) {
                if (!array[1]) {
                    chdir(getenv("HOME"));
                }
                else if (array[2]) {
                    perror("Too many arguments");
                }
                else {
                    if (chdir(array[1]) == -1) {
                        perror("Wrong path");
                    }
                    else {
                        chdir(array[1]);
                    }
                }
            }
            else if (!(strcmp(array[0], "exit"))) {
                clear_list(root, &end_of_list);
                free(array);
                free(root);
                exit(1);
            }
            else {
                int pid = fork();
                if (pid == -1) {
                    perror("fork() failed");
                }
                else if (!pid) {
                    execvp(array[0], array);
                }
                else {
                    wait(NULL);
                }
            }
        }
        free(array);
        clear_list(root, &end_of_list);
    };

    free(root);
    fclose(in);
    fclose(out);
}