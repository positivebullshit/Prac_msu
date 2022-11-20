#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FILE *in, *out;
int c, lenword;
int flag_eof = 0, flag_eol = 0;

int readword() {
    char *word = NULL;
    int i = 0;
    lenword = 50;
    int flag_quote = 0;
    int flag_and = 0, flag_or = 0, flag_arrow = 0;
    word = calloc(lenword + 1, sizeof (char));
    while ((c = fgetc(in) != EOF) && (c != '\n')) {
        if (c == '\"') {
            flag_quote++;
        }
        else if (flag_quote == 1) {
            word[i++] = c;
        }
        else if (c == ' ') {
            break;
        }
        else if ((c == '&') && (!flag_quote)) {
            if (flag_and == 1) {
                word[i++] = c;
                flag_and = 0;
                break;
            }
            else {
                word[i++] = c;
                flag_and++;
            }
        }
        else if ((c == '|') && (!flag_quote)) {
            if (flag_or == 1) {
                word[i++] = c;
                flag_or = 0;
                break;
            }
            else {
                flag_or++;
                word[i++] = c;
            }
        }
        else if ((c == '>') && (!flag_quote)) {
            if (flag_arrow == 1) {
                word[i++] = c;
                flag_arrow = 0;
                break;
            }
            else {
                flag_arrow++;
                word[i++] = c;
            }
        }
        else if ((c == ';') && (!flag_quote)) {
            word[i++] = c;
            break;
        }
        else if ((c == '<') && (!flag_quote)) {
            word[i++] = c;
            break;
        }
        else if ((c == '(') && (!flag_quote)) {
            word[i++] = c;
            break;
        }
        else if ((c == ')') && (!flag_quote)) {
            word[i++] = c;
            break;
        }
        else if (flag_quote == 2) {
            flag_quote = 0;
        }
        else {  // Если не спецсимвол
            if ((flag_and) || (flag_or) || (flag_arrow)){
                add_to_list(word);
                free(word);
                lenword = 50;
                word = calloc(lenword + 1, sizeof (char));
                i = 0;
                word[i++] = c;
            }
            else {
                word[i++] = c;
            }
        }
        if (word != NULL) {
            add_to_list(word);
        }
        if ((i == lenword - 2) || (i == lenword - 3)) {
            lenword *= 2;
            word = realloc(word, (lenword + 1) * sizeof (char));
        }
        if (c == '\n') {
            flag_eol = 1;
        }
        if (c == EOF) {
            flag_eof = 1;
        }
    }
}

int main(int argc, char *argv[]) {

    char *word;

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
