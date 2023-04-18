#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>
#include <cstring>
#include <vector>

using namespace std;

enum type_of_lex {
    LEX_PROGRAM, 
    LEX_INT, LEX_STRING, LEX_BOOLEAN, 
    LEX_FALSE, LEX_TRUE,                                                                           
    LEX_AND, LEX_OR, LEX_NOT, LEX_IF, LEX_ELSE, 
    LEX_WHILE, LEX_BREAK, LEX_READ, LEX_WRITE, 
    LEX_LCURLY, LEX_RCURLY, LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, 
    LEX_EQ, LEX_LESS, LEX_GREATER, LEX_LEQ, LEX_NEQ, LEX_GEQ, 
    LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH,             
    LEX_NUM, LEX_ID, LEX_STRLITERAL,
    LEX_NULL,
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
};

const char * TW [] = {"program", "int", "string", "boolean", "false", "true", 
    "and", "or", "not", "if", "else", "while", "break", "read", "write", NULL};

const char * TD [] = {"{", "}", ";", ",", ":", "=", "(", ")", "==", "<", ">", 
    "<=", "!=", ">=", "+",  "-", "*", "/", NULL};

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

vector <string> TID; 
vector <string> TConst;

Lex get_lex() {
    enum state {H, IDENT, NUMB, STRING, COM, COMP, SYMB};
    char c;
    int n;
    string buf;
    state CS = H;
    do {
        cin.get(c);
        switch (CS) {
            case H:
                if (cin.eof()) {
                    return Lex();
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
                else if (c == '+'|| c == '-' || c == '*' || c == '{' || c == '}' 
                     || c == ';' || c == ',' || c == ':' || c == '(' || c == ')') {
                    CS = SYMB;
                    cin.unget();
                }
                else if (c == '/') {
                    if (cin.peek() == '*') {
                        cin.get();
                        CS = COM;
                    }
                    else {
                        cin.unget();
                        CS = SYMB;
                    }
                }
                else if (c == '=' || c == '>' || c == '<' || c == '!') {
                    if (cin.peek() == '=') {
                        CS = COMP;
                    }
                    else {
                        CS = SYMB;
                    }
                    cin.unget();
                }
                else if (c == '"') {
                    CS = STRING;
                }
                else {
                    throw -11111;
                }
                break;
            case IDENT:
                if (isdigit(c) || isalpha(c)) {
                    buf.push_back(c);
                }
                else {
                    cin.unget();
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
                    cin.unget();
                    return Lex(LEX_NUM, n);
                }
                break;
            case COM:
                if (cin.eof()) {
                    throw -2;
                }
                if (c == '*') {
                    if (cin.peek() == '/') {
                        cin.get(c);
                        CS = H;
                    }
                    else {
                        cin.unget();
                    }
                }
                break;
            case STRING:
                if (cin.eof()) {
                    throw -3;
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
                s1.push_back(cin.get());
                int i1 = lex_num(s1, TD);
                return Lex((type_of_lex) (i1 + 15), i1);
                break;
                } 
            case SYMB:
                string ss = "";
                ss.push_back(c);
                int ii = lex_num(ss, TD);
                return Lex((type_of_lex)(ii + 15), ii);
                break;
        }
    } while (true);
}

int main(int argc, char** argv) {
    Lex lex;
    while (true) {
        lex = get_lex();
        if (lex.get_type() == LEX_NULL) {
            break;
        }
        else {
            cout << lex.get_type() << " " << lex.get_value()<< endl;
        }
    }
    return 0;
}