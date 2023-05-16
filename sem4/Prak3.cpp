#include <iostream>
#include <fstream>
#include <ctype.h>
#include <cstring>
#include <vector>

using namespace std;

const char * TW [] = { "program", "int", "string", "boolean", "false", "true", 
    "and", "or", "not", "if", "else", "while", "break", "read", "write", NULL };

const char * TD [] = { "{", "}", ";", ",", ":", "=", "(", ")", "==", "<", ">", 
    "<=", "!=", ">=", "+",  "-", "*", "/", NULL };

enum type_of_lex {
    LEX_PROGRAM,                                                                                            // 0
    LEX_INT, LEX_STRING, LEX_BOOLEAN,                                                                       // 1 - 3
    LEX_FALSE, LEX_TRUE,                                                                                    // 4 - 5
    LEX_AND, LEX_OR, LEX_NOT, LEX_IF, LEX_ELSE,                                                             // 6 - 10
    LEX_WHILE, LEX_BREAK, LEX_READ, LEX_WRITE,                                                              // 11 - 14
    LEX_LCURLY, LEX_RCURLY, LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN,        // 15 - 22
    LEX_EQ, LEX_LESS, LEX_GREATER, LEX_LEQ, LEX_NEQ, LEX_GEQ,                                               // 23 - 28
    LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH,                                                              // 29 - 32          
    LEX_NUM, LEX_ID, LEX_STRLITERAL,                                                                        // 33 - 35
    LEX_NULL,                                                                                               // 36
};

class Lex {
    type_of_lex t_lex;
    int v_lex;
public:
    Lex(type_of_lex t = LEX_NULL, int v = 0): t_lex(t), v_lex(v) { }
    type_of_lex get_type() const {
        return t_lex;
    }
    int get_value() const {
        return v_lex;
    }
    friend ostream& operator<< (ostream &s, Lex l);  // добавил только
};

vector <string> TID; 
vector <string> TConst;

ostream& operator<< (ostream &s, Lex l) {
    string t;
    if (l.t_lex <= LEX_WRITE) {
        t = TW[l.t_lex];
    }
    else if (l.t_lex <= LEX_SLASH) {
        t = TD[l.t_lex - LEX_WRITE - 1];
    }
    else if (l.t_lex == LEX_ID)
    {
        t = "IDENTIFICATOR";
    }
    else if (l.t_lex == LEX_STRLITERAL)
    {
        t = "STRLITERAL";
    }
    else if (l.t_lex == LEX_NUM)
    {
        t = "NUMBER";
    }
    else 
        throw l;
    s << t  << endl;
    return s;
}

class Scanner {
    FILE* fp; 
    char c;
    void gc() {
        c = fgetc(fp);
    }
    char fpeek(FILE* fp) {
        gc();
        ungetc(c, fp);
    }
    int lex_num(string str, const char** id) {
        int i = 0;
        while ((id[i] != NULL) && (string(id[i]) != str)) {
            i++;
        }
        if (id[i] == NULL) {
            return -1;
        }
        return i;
    }
public:
    Scanner(const char* program) {
        if ( !(fp = fopen ( program, "r" )) ) 
            throw "can't open file" ;
    }
    Lex get_lex ();
};


Lex Scanner::get_lex() {
    enum state {H, IDENT, NUMB, STRING, COM, COMP, SYMB};
    int n;
    string buf;
    state CS = H;
    do { 
        gc();
        if (c == '=') {
            cout << "OK";
        }
        switch (CS) {

            case H:
                if (feof(fp)) {
                    return Lex();  // нужен ли тут type_of_lex хммм подумать
                }
                if (c == ' ' || c == '\n' || c == '\t');
                else if (isalpha(c)) {
                    buf.push_back(c);
                    CS = IDENT;
                }
                else if (isdigit(c)) {
                    n = c - '0';
                    CS = NUMB;
                }
                else if (c == '+' || c == '-' || c == '*' || c == '{' || c == '}' 
                      || c == ';' || c == ',' || c == ':' || c == '(' || c == ')' ) {
                    CS = SYMB;
                    ungetc(c, fp);
                }
                else if (c == '/') {
                    if (fpeek(fp) == '*') {
                        gc();
                        CS = COM;
                    }
                    else {
                        ungetc(c, fp);
                        CS = SYMB;
                    }
                }
                else if (c == '=' || c == '>' || c == '<' || c == '!') {
                    if (fpeek(fp) == '=') {
                        CS = COMP;
                    }
                    else {
                        CS = SYMB;
                    }
                    ungetc(c, fp);
                }
                else if (c == '"') {
                    CS = STRING;
                }
                else {
                    throw "unexpected lex";
                }
                break;

            case IDENT:
                if (isdigit(c) || isalpha(c)) {
                    buf.push_back(c);
                }
                else {
                    int i = lex_num(buf, TW);
                    if (i == -1) {
                        TID.push_back(buf);
                        return Lex(LEX_ID, TID.size() - 1);
                    }
                    else {
                        return Lex((type_of_lex) i, i);
                    }
                }
                break;

            case NUMB:
                if (isdigit(c)) {
                    n = n * 10 + (c - '0');
                }
                else {
                    ungetc(c, fp);
                    return Lex(LEX_NUM, n);
                }
                break;

            case COM:
                if (feof(fp)) {
                    throw "EOF inside comment";
                }
                if (c == '*') {
                    if (fpeek(fp) == '/') {
                        gc();
                        CS = H;
                    }
                    else {
                        ungetc(c, fp);  // а зачем мне тут ungetc...
                    }
                }
                break;

            case STRING:
                if (feof(fp)) {
                    throw "EOF inside string";
                }
                if (c != '"') {
                    buf.push_back(c);
                } else {
                    TConst.push_back(buf);
                    return Lex(LEX_STRLITERAL, TConst.size() - 1);
                }
                break;

            case COMP: {  // { } are necessary
                string s1 = "";
                s1.push_back(c);
                gc();
                s1.push_back(c);
                int i1 = lex_num(s1, TD);
                return Lex((type_of_lex) (i1 + 15), i1 + 15);
                break;
                } 

            case SYMB:
                string ss = "";
                ss.push_back(c);
                // cout << c << endl;
                // cout << ss << endl;
                int ii = lex_num(ss, TD);
                // cout << ss << " " << ii << endl;
                return Lex((type_of_lex)(ii + 15), ii + 15);
                break;
        }
    } while (true);
}

int main(int argc, char** argv) {
    try {
        Scanner p("program.txt");
        Lex temp;
        for (int i = 0; i <= 10; i++) {
            temp = p.get_lex();
            cout << temp;
        }
        return 0;
    }

    catch (const char* str) {
        cout << "Code terminated: " << str << endl;
    }

    catch (Lex l) {
        cout << "Unexpected Lexeme " << l << endl;
    }
    // Lex lex;
    // while (true) {
    //     lex = get_lex();
    //     if (lex.get_type() == LEX_NULL) {
    //         break;
    //     }
    //     else {
    //         cout << lex.get_type() << " " << lex.get_value()<< endl;
    //     }
    // }
}