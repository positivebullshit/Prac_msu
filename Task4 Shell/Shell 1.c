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
#include <fcntl.h>


#define STDIN 0
#define STDOUT 1
#define STDERR 2

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

char *spec_symbols[] = {"", "&", "&&", "|", "||", ";", ">", ">>", "<", "(", ")"};
/*
 * NOTSPECSYMB - read the name of flag and don't ask any questions
 * BG - & - for background mode
 * AND - &&
 * PIPE - | - for conveyor realisation
 * OR - ||
 * SEMICOLON - ;
 * OUT - > - output redirection, overwriting file
 * ADD - >> - output redirection, adding to file
 * IN - < - input redirection
 * LEFTBRACKET - (
 * RIGHTBRACKET - )
 */
enum flag {NOTSPECSYMB, BG, AND, PIPE, OR, SEMICOLON, OUT, ADD, IN, LEFTBRACKET, RIGHTBRACKET};

enum flag check_the_elem(char *elem) {
    for (enum flag i = NOTSPECSYMB + 1; i <= RIGHTBRACKET; i++) {
        if (!(strcmp(elem, spec_symbols[i]))) {
            return i;
        }
    }
    return NOTSPECSYMB;
}

typedef struct command {
    char **command_array;
    int command_word_count;
    int bg_fl, and_fl, or_fl, semicolon_fl, pipe_fl;  // flags
    char *in_fd, *out_fd, *add_fd;
    struct command *command_next;
} command;

void struct_initialization(command *cmd_struct) {
    cmd_struct->command_array = NULL;
    cmd_struct->command_word_count = 0;
    cmd_struct->bg_fl = cmd_struct->and_fl = cmd_struct->or_fl = cmd_struct->semicolon_fl = cmd_struct->pipe_fl =  0;
    cmd_struct->in_fd = cmd_struct->out_fd = cmd_struct->add_fd = NULL;
}

command *parcing_string_by_commands(char **cmd_array) {
    command *cmd_struct = (command*)malloc(sizeof(command)), *root = cmd_struct;
    struct_initialization(cmd_struct);
    enum flag res;
    while (*cmd_array != NULL) {
        res = check_the_elem(*cmd_array);
        switch (res) {
            // cmd_array++ - переход к следующему слову нашего массива слова
            case NOTSPECSYMB:
                cmd_struct->command_word_count++;
                cmd_struct->command_array = (char**)realloc(cmd_struct->command_array, sizeof(char*) * (cmd_struct->command_word_count + 1));
                cmd_struct->command_array[cmd_struct->command_word_count - 1] = *cmd_array;
                cmd_struct->command_array[cmd_struct->command_word_count] = NULL;
                cmd_array++;
                break;
            case BG:
                cmd_array++;
                cmd_struct->bg_fl = 1;
                if (*cmd_array == NULL) {
                    perror("There is nothing after &\n");
                    return root;
                }
                else {
                    cmd_struct->command_next = calloc(1, sizeof(command));
                    cmd_struct = cmd_struct->command_next;
                    struct_initialization(cmd_struct);
                }
                break;
            case AND:
                cmd_struct->command_next = calloc(1, sizeof(command));
                cmd_struct = cmd_struct->command_next;
                struct_initialization(cmd_struct);
                cmd_struct->and_fl = 1;
                cmd_array++;
                break;
            case PIPE:
                cmd_struct->command_next = calloc(1, sizeof(command));
                cmd_struct->pipe_fl = 1;
                cmd_struct = cmd_struct->command_next;
                struct_initialization(cmd_struct);
                cmd_array++;
                break;
            case OR:
                cmd_struct->command_next = calloc(1, sizeof(command));
                cmd_struct = cmd_struct->command_next;
                struct_initialization(cmd_struct);
                cmd_struct->or_fl = 1;
                cmd_array++;
                break;
            case SEMICOLON:
                cmd_struct->command_next = calloc(1, sizeof(command));
                cmd_struct = cmd_struct->command_next;
                struct_initialization(cmd_struct);
                cmd_struct->semicolon_fl = 1;
                cmd_array++;
                break;
            case OUT:
                cmd_array++;
                cmd_struct->out_fd = *cmd_array;
                cmd_array++;
                break;
            case ADD:
                cmd_array++;
                cmd_struct->add_fd = *cmd_array;
                cmd_array++;
                break;
            case IN:
                cmd_array++;
                cmd_struct->in_fd = *cmd_array;
                cmd_array++;
                break;
            default:
                break;
        }
    }
    return root;
}

int executing_a_command(command *cmd_struct) {
    int command_number = 1;
    int fd[2], file;
    while (cmd_struct != NULL) {
        int savestdin = dup(STDIN), savestdout = dup(STDOUT);
        if ((cmd_struct->command_array != NULL) && (!(strcmp(cmd_struct->command_array[0], "cd")))) {
            if (cmd_struct->command_array[1] == NULL) {
                if (chdir(getenv("HOME")) == -1) {
                    perror("Error with home directory\n");
                    exit(1);
                }
            }
            else if (cmd_struct->command_array[2]) {
                perror("Too many arguments\n");
                exit(1);
            }
            else {
                if (chdir(cmd_struct->command_array[1]) == -1) {
                    perror("Wrong path\n");
                }
            }

        }
        else if (cmd_struct->command_array != NULL && (!(strcmp(cmd_struct->command_array[0], "exit")))) {
            exit(0);
        }
        else {
            // Input/Output redirection
            if (command_number > 1) {
                dup2(fd[0], STDIN);
                close(fd[0]);
            }
            if (cmd_struct->pipe_fl) {  // Each command with | after it
                pipe(fd);
                dup2(fd[1], STDOUT);
                close(fd[1]);
            }
            int pid = fork();
            if (!pid) {  // if True - child, else - father
                if (cmd_struct->in_fd) {  // now input has to be cmd_struct->in_fd
                    file = open(cmd_struct->in_fd, O_RDONLY);
                    if (file == -1) {
                        perror( cmd_struct->in_fd);
                        exit(1);
                    }
                    dup2(file, STDIN);
                    close(file);
                }
                else if (cmd_struct->out_fd) {  // now output has to be cmd_struct->out_fd
                    file = open(cmd_struct->out_fd, O_WRONLY | O_CREAT | O_TRUNC, 0666);  // 0666 to make O_CREAT work correct
                    if (file == -1) {
                        perror( cmd_struct->out_fd);
                        exit(1);
                    }
                    dup2(file, STDOUT);
                    close(file);
                }
                else if (cmd_struct->add_fd) {  // now output should be added to existing file
                    file = open(cmd_struct->add_fd, O_WRONLY | O_APPEND);
                    if (file == -1) {
                        perror( cmd_struct->add_fd);
                        exit(1);
                    }
                    dup2(file, STDOUT);
                    close(file);
                }
                //printf("%d\n", command_number);
                execvp(cmd_struct->command_array[0], cmd_struct->command_array);
                perror(cmd_struct->command_array[0]); // incorrect execvp so return 1
                return 1;
            }
            else {
                wait(NULL);
            }
        }
        cmd_struct = cmd_struct->command_next;
        command_number++;
        dup2(savestdin, STDIN);
        close(savestdin);
        dup2(savestdout, STDOUT);
        close(savestdout);
        // stdin and stdout are default again
    }
    return 0;
}

int main(int argc, char *argv[]) {

    int flag_eof = 1, flag_eol = 1, len_word = 50, number_of_words = 0;
    int exit_fl;
    Node *root = (Node*)malloc(sizeof(Node));  // root-pointer
    root->info = "";
    root->next = NULL;
    Node *end_of_list = root;

    command *one_line;

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
        list_to_array(root, array, number_of_words);  // array of separate words and symbols

        if (array[0] != NULL) {
            if (strcmp(array[0], "")) {
                one_line = parcing_string_by_commands(array);  // one_line - struct of commands
                exit_fl = executing_a_command(one_line);
            }
        }

        while (one_line != NULL) {
            command *tmp = one_line;
            one_line = one_line->command_next;
            free(tmp->command_array);
            free(tmp);
        }


        free(array);
        clear_list(root, &end_of_list);
        if (exit_fl) {
            break;
        }
    }
    free(root);
    fclose(in);
    fclose(out);
}