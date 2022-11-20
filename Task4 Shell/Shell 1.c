#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FILE *in, *out;
int c;
int flag_eof = 0, flag_eol = 0;

void readword() {
    char *word;
    int i = 0, lenword = 50;
    int flag_quote = 0, flag_and = 0, flag_or = 0, flag_arrow = 0;
    word = calloc(lenword + 1, sizeof (char));
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
                    lenword = 50, i = 0;
                    word = calloc(lenword + 1, sizeof(char));
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
                word = calloc(lenword + 1, sizeof (char));
            }
            word[i++] = c;
        }
        if (i >= lenword - 3) {
            lenword *= 2;
            word = realloc(word, (lenword + 1) * sizeof (char));
        }
    }


    if ((c == '\n') || (c == EOF)){
        flag_eol = (c == '\n');
        flag_eof = (c == EOF);
        if (word != NULL) {
            word[i] = '\0';
            // add to list
            free(word);
        }
    }
}













int main(int argc, char *argv[]) {

     //char *word;

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

    while (!flag_eof) {
        while ((!flag_eof) && (!flag_eol)) {

        }
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