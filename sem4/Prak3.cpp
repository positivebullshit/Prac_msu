#include <iostream>
#include <fstream>
#include <ctype.h>
#include <cstring>
#include <vector>

using namespace std;

const char * TW [] = { "program", "int", "string", "boolean", "false", "true", 
    "and", "or", "not", "if", "else", "while", "break", "read", "write", NULL };

const char * TD [] = { "{", "}", ";", ",", ":", "=", "(", ")", "==", "<", ">", 
    "<=", "!=", ">=", "+",  "-", "*", "/", "%", NULL };

enum type_of_lex {
    LEX_PROGRAM,                                                                                            // 0
    LEX_INT, LEX_STRING, LEX_BOOLEAN,                                                                       // 1 - 3
    LEX_FALSE, LEX_TRUE,                                                                                    // 4 - 5
    LEX_AND, LEX_OR, LEX_NOT, LEX_IF, LEX_ELSE,                                                             // 6 - 10
    LEX_WHILE, LEX_BREAK, LEX_READ, LEX_WRITE,                                                              // 11 - 14
    LEX_LCURLY, LEX_RCURLY, LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN,        // 15 - 22
    LEX_EQ, LEX_LESS, LEX_GREATER, LEX_LEQ, LEX_NEQ, LEX_GEQ,                                               // 23 - 28
    LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_MOD,                                                     // 29 - 33         
    LEX_NUM, LEX_ID, LEX_STRLITERAL,                                                                        // 34 - 36
    LEX_NULL,                                                                                               // 37
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
    friend ostream& operator<< (ostream &s, Lex l); 
};

vector <string> TID; 
vector <string> TConst;

ostream& operator<< (ostream &s, Lex l) {
    string t;
    if (l.t_lex <= LEX_WRITE) {
        t = TW[l.t_lex];
    }
    else if (l.t_lex <= LEX_MOD) {
        t = TD[l.t_lex - LEX_WRITE - 1];  
    }
    else if (l.t_lex == LEX_ID)
    {
        t = "IDENTIFICATOR with number";
    }
    else if (l.t_lex == LEX_STRLITERAL)
    {
        t = "STRLITERAL with number";
        // t = TConst[l.v_lex];  //  This way if u want to print the string itself
    }
    else if (l.t_lex == LEX_NUM)
    {
        t = "NUMBER =";
    }
    else 
        throw l;
    s << t << " " << l.v_lex << endl;
    return s;
}

class Scanner {
    FILE* fp; 
    char c, c_prev;
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
    int n, i1, i2;
    string buf, s1, s2;
    state CS = H;
    do { 
        gc();
        switch (CS) {

            case H:
                if (feof(fp)) {
                    return Lex();  // Default constructor makes it LEX_NULL and stops "while (true)"
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
                else if (c == '+' || c == '-' || c == '*' || c == '{' || c == '}' || c == ';' 
                      || c == ',' || c == ':' || c == '(' || c == ')' ||  c == '%') {
                    CS = SYMB;
                    ungetc(c, fp);
                }
                else if (c == '/') {
                    c_prev = c;  // is necessary because we may ungetc twice
                    if (fpeek(fp) == '*') {
                        gc();
                        CS = COM;
                    }
                    else {
                        ungetc(c_prev, fp);
                        CS = SYMB;
                    }
                }
                else if (c == '=' || c == '>' || c == '<' || c == '!') {
                    c_prev = c;  // is necessary because we may ungetc twice
                    if (fpeek(fp) == '=') {
                        CS = COMP;
                    }
                    else {
                        CS = SYMB;
                    }
                    ungetc(c_prev, fp);
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

            case COMP:   // { } are necessary
                s1.push_back(c);
                gc();
                s1.push_back(c);
                i1 = lex_num(s1, TD);
                return Lex((type_of_lex) (i1 + 15), i1 + 15);
                break;
            

            case SYMB:
                s2.push_back(c);
                i2 = lex_num(s2, TD);
                return Lex((type_of_lex)(i2 + 15), i2 + 15);
                break;
        }
    } while (true);
}

class Parser {
    Lex curr_lex;
    type_of_lex c_type;
    int c_val;
    Scanner scan;

    void gl() {  // get_lex full
        curr_lex = scan.get_lex();
        c_type = curr_lex.get_type();
        c_val = curr_lex.get_value();
    }
    void P();  // program
    void TN();  // type_name
    void D();  // declaration
    void E();  // Elem
    void Expr();  // Expression
public:
    Parser(const char* program): scan(program) { }
    void analyze();
};

void Parser::analyze() {
    gl();
    P();
    if (c_type != LEX_NULL) {
        throw curr_lex;
    }
    cout << "The program execution is succesful!" << endl;
}

void Parser::P() {  //program
    if (c_type == LEX_PROGRAM) {
        gl();
    }
    else {
        throw curr_lex;
    }
    if (c_type == LEX_LCURLY) {
        gl();
    }
    else {
        throw curr_lex;
    }
    
    // Вызовы следующих функций, спуск как таковой
    TN();  // Type_name, which calls declaration D()

    if (c_type == LEX_RCURLY) {
        gl();
    }
    else {
        throw curr_lex;
    }
}

void Parser::TN() {  // type_name
    while ((c_type == LEX_BOOLEAN) || (c_type == LEX_INT) || (c_type == LEX_STRING)) {
        gl();
        D();
    }
}

void Parser::D() {  // declaration
    if (c_type == LEX_ID) {
        gl();
        if (c_type == LEX_ASSIGN) {
            gl();
            if ((c_type == LEX_NUM) || (c_type == LEX_STRLITERAL)) {
                gl();
                if (c_type == LEX_COMMA) {
                    D();
                }
                else if (c_type == LEX_SEMICOLON) {
                    gl();
                }
                else {
                    throw curr_lex;
                }
            }
            else {
                throw curr_lex;
            }
        }
        else if (c_type == LEX_COMMA) {
            D();
        }
        else if (c_type == LEX_SEMICOLON) {
            gl();
        }
        else {
            throw curr_lex;
        }
    }
    else {
        throw curr_lex;
    }
}

/* на сегодня все, берегите себя и своих близких
void Parser::E() {  // ELem
    if ((c_type == LEX_ID) || (c_type == LEX_NUM) || (c_type == LEX_STRLITERAL)) {
        gl();
    }
    else if (c_type == LEX_NOT) {
        gl();
        Expr();
    }
    
}
*/ 

int main(int argc, char** argv) {
    try {
        Scanner p("program.txt");
        Lex temp;
        while (true) {
            temp = p.get_lex();
            if (temp.get_type() == LEX_NULL) {
                break;
            }
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
}