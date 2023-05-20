#include <iostream>
#include <fstream>
#include <ctype.h>
#include <cstring>
#include <vector>
#include <algorithm>
#include <stack>

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
    POLIZ_LABEL,                                                                                            // 37
    POLIZ_ADDRESS,                                                                                          // 38
    POLIZ_GO,                                                                                               // 39
    POLIZ_FGO,                                                                                              // 40
    LEX_NULL,                                                                                               // 41
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

class Ident {
    string name;
    type_of_lex type;
    bool declare;
    int value;
public:
    Ident (const string n): name(n), type(LEX_NULL), declare(false), value(0) { }
    string get_name() const { return name; }
    type_of_lex get_type() { return type; }
    void put_type(type_of_lex t) { type = t; }
    bool get_declare() const { return declare; }
    void put_declare() { declare = true; }
    int get_value() { return value; }
    void put_value(int v) { value = v; }
    bool operator== (const string& s) const { return name == s; }
};

vector <Ident> TID; 
vector <string> TConst;

int put (const string& buf) {
    vector<Ident>::iterator k;
    if ((k = find(TID.begin(), TID.end(), buf)) != TID.end()) {
        return k - TID.begin();
    }
    TID.push_back(Ident(buf));
    return TID.size() - 1;
}

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
    else if (l.t_lex == POLIZ_LABEL) {
        t = "LABEL";
    }
    else if (l.t_lex == POLIZ_ADDRESS) {
        t = "ADDRESS";
    }
    else if (l.t_lex == POLIZ_GO) {
        t = "!";
    }
    else if (l.t_lex == POLIZ_FGO) { 
        t = "!F";
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
                    ungetc(c, fp);
                    int i = lex_num(buf, TW);
                    if (i == -1) {
                        int ind = put(buf);
                        return Lex(LEX_ID, ind);
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

    int idtype;
    stack <int> st_int;
    stack <type_of_lex> st_lex;

    void gl() {  // get_lex full
        curr_lex = scan.get_lex();
        c_type = curr_lex.get_type();
        c_val = curr_lex.get_value();
    }
    void P();  // program
    void TN();  // type_name
    void D();  // declaration
    void Expr();  // Expression
    void E1();  // 1 expression
    void O();  // Operations
    bool Op();  // 1 operation
    void A();  // Assign
    void T();  
    void F();
    void dec(type_of_lex type);
    void check_id();
    void check_op();
    void check_not();
    void eq_type();
    void eq_bool();
    void check_id_in_read();
    void set_idtype();
    void change_break();
public:
    Parser(const char* program): scan(program) { }
    void analyze();
    void idtype_res() {
        idtype = 0;
    }
    void idtype_boolean() {
        idtype = 1;
    }
    void idtype_int() {
        idtype = 2;
    }
    void idtype_string() {
        idtype = 3;
    }
    vector <Lex> poliz;
    vector <int> st_while;
    vector <int> fin_while;
};

void Parser::set_idtype() {
    idtype_res();
    if (c_type == LEX_BOOLEAN) {
        idtype_boolean();
    } 
    else if (c_type == LEX_INT) {
        idtype_int();
    }
    else if (c_type == LEX_STRING) {
        idtype_string();
    }
}

void Parser::analyze() {
    gl();
    P();
    if (c_type != LEX_NULL) {
        throw curr_lex;
    }
    change_break();
    for (Lex l: poliz) {
        cout << l;
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
    O();

    if (c_type == LEX_RCURLY) {
        gl();
    }
    else {
        throw curr_lex;
    }
}

void Parser::TN() {  // type_name
    while ((c_type == LEX_BOOLEAN) || (c_type == LEX_INT) || (c_type == LEX_STRING)) {
        set_idtype();
        gl();
        D();
    }
}

void Parser::D() {  // declaration
    if (c_type == LEX_ID) {
        st_int.push(c_val);
        if (idtype == 1) {
            dec(LEX_BOOLEAN);
            st_lex.push(LEX_BOOLEAN);
        }
        else if (idtype == 2) {
            dec(LEX_INT);
            st_lex.push(LEX_INT);
        } 
        else if (idtype == 3) {
            dec(LEX_STRING);
            st_lex.push(LEX_STRING);
        }
        
        gl();
        if (c_type == LEX_ASSIGN) {
            gl();
            if (c_type == LEX_NUM || c_type == LEX_STRLITERAL || 
                 c_type == LEX_TRUE || c_type == LEX_FALSE) {
                gl();
                if (c_type == LEX_COMMA) {
                    gl();
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
            gl();
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

bool Parser::Op() {  // 1 operation
    int p0, p1, p2, p3;
    bool flag = false;
    if (c_type == LEX_IF) {
        gl();
        if (c_type == LEX_LPAREN) {
            gl();
        } else {
            throw curr_lex;
        }
        Expr();
        eq_bool();
        p2 = poliz.size();
        poliz.push_back(Lex());
        poliz.push_back(Lex(POLIZ_FGO));
        if (c_type == LEX_RPAREN) {
            gl();
        } else {
            throw curr_lex;
        }
        Op();
        if (c_type == LEX_ELSE) {
            p3 = poliz.size();
            poliz.push_back(Lex());
            poliz.push_back(Lex(POLIZ_GO));
            poliz[p2] = Lex(POLIZ_LABEL, poliz.size());
            gl();
            Op();
            poliz[p3] = Lex(POLIZ_LABEL, poliz.size());
        } else {
            poliz[p2] = Lex(POLIZ_LABEL, poliz.size());
        }
        flag = true;
    }
    else if (c_type == LEX_WHILE) {
        gl();
        if (c_type == LEX_LPAREN) {
            gl();
        } else {
            throw curr_lex;
        }
        p0 = poliz.size();
        Expr();
        eq_bool();
        p1 = poliz.size();
        st_while.push_back(p0);
        poliz.push_back(Lex());
        poliz.push_back(Lex(POLIZ_FGO));
        if (c_type == LEX_RPAREN) {
            gl();
        } else {
            throw curr_lex;
        }
        Op();
        poliz.push_back(Lex(POLIZ_LABEL, p0));
        poliz.push_back(Lex(POLIZ_GO));
        fin_while.push_back(poliz.size());
        poliz[p1] = Lex(POLIZ_LABEL, poliz.size());
        flag = true;
    }
    else if (c_type == LEX_READ) {
        gl();
        if (c_type == LEX_LPAREN) {
            gl();
            if (c_type == LEX_ID) {
                check_id_in_read();
                poliz.push_back(Lex(POLIZ_ADDRESS, c_val));
                gl();
                if (c_type == LEX_RPAREN) {
                    poliz.push_back(LEX_READ);
                    gl();
                    flag = true;
                    if (c_type == LEX_SEMICOLON) {
                        gl();
                    } else {
                        throw(curr_lex);
                    }
                } else {
                    throw(curr_lex);
                }
            } else {
                throw(curr_lex);
            }            
        } else {
            throw(curr_lex);
        }
    }
    else if (c_type == LEX_WRITE) {
        gl();
        if (c_type == LEX_LPAREN) {
            gl();
            Expr();
            poliz.push_back(Lex(LEX_WRITE));
            if (c_type == LEX_COMMA) {
                while (c_type != LEX_RPAREN) {
                    gl();
                    Expr();
                    poliz.push_back(Lex(LEX_WRITE));
                }
            }
            if (c_type == LEX_RPAREN) {
                gl();
                if (c_type == LEX_SEMICOLON) {
                    gl();
                    flag = true;
                } else {
                    throw curr_lex;
                }
            } else {
                throw curr_lex;
            }
        } else {
            throw curr_lex;
        }
    }
    else if (c_type == LEX_ID) {
        check_id();
        poliz.push_back(Lex(POLIZ_ADDRESS, c_val));
        gl();
        A();
        if (c_type == LEX_SEMICOLON) {
            gl();
            flag = true;
        } else {
            throw(curr_lex);
        }
    }
    else if (c_type == LEX_LCURLY) {
        gl();
        O();
        if (c_type == LEX_RCURLY) {
            gl();
            flag = true;
        } else {
            throw curr_lex;
        }
    }
    else if (c_type == LEX_BREAK) {
        gl();
        if (c_type == LEX_SEMICOLON) {
            poliz.push_back(Lex(LEX_BREAK));
            gl();
            flag = true;
        } else {
            throw curr_lex;
        }
    }
    return flag;    
}

void Parser::O() {
    while (Op());
}

void Parser::A() {
    if (c_type == LEX_ASSIGN) {
        gl();
        E1();
        eq_type();
        poliz.push_back(Lex(LEX_ASSIGN));
    } else {
        throw(curr_lex);
    }
}

void Parser::Expr() 
    {
        E1();
        if (c_type == LEX_LEQ || c_type == LEX_GEQ || c_type == LEX_LESS || 
            c_type == LEX_GREATER|| c_type == LEX_NEQ || c_type == LEX_EQ)
        {
            st_lex.push(c_type);
            gl();
            E1();
            check_op();
        }
    }

void Parser::E1() {
    T();
    while (c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_OR) {
            st_lex.push(c_type);
            gl();
            T();
            check_op();
    }
}

void Parser::T() {
    F();
    while (c_type == LEX_TIMES || c_type == LEX_SLASH || c_type == LEX_AND || c_type == LEX_MOD) {
        st_lex.push(c_type);
        gl();
        F();
        check_op();
    }
}

void Parser::F() {
    if (c_type == LEX_ID) {
        check_id();
        poliz.push_back(Lex(LEX_ID, c_val));
        gl();
    }
    else if (c_type == LEX_NUM) {
        st_lex.push(LEX_INT);
        poliz.push_back(Lex(LEX_NUM, c_val));
        gl();
    }
    else if (c_type == LEX_STRLITERAL) {
        st_lex.push(LEX_STRING);
        poliz.push_back(Lex(LEX_STRING, c_val));
        gl();
    } else if (c_type == LEX_TRUE) {
        poliz.push_back(Lex(LEX_TRUE, 1));
        st_lex.push(LEX_BOOLEAN);
        gl();
    } 
    else if (c_type == LEX_FALSE) {
        poliz.push_back(Lex(LEX_FALSE, 0));
        st_lex.push(LEX_BOOLEAN);
        gl();
    }
    else if (c_type == LEX_NOT) {
        gl();
        F();
        check_not();
    } 
    else if (c_type == LEX_LPAREN) {
        gl();
        Expr();
        if (c_type == LEX_RPAREN) {
            gl();
        } else {
            throw curr_lex;
        }
    } else {
        throw curr_lex;
    }
}

void Parser::dec(type_of_lex type) {
    int i;
    while (!st_int.empty()) {
        i = st_int.top();
        st_int.pop();
        if (TID[i].get_declare()) {
            throw "twice";
        } else {
            TID[i].put_declare();
            TID[i].put_type(type);
        }
    }
}

void Parser::check_op() {
    type_of_lex t1, t2, op, r = LEX_NULL;
    t2 = st_lex.top();
    st_lex.pop();
    op = st_lex.top();
    st_lex.pop();
    t1 = st_lex.top();
    st_lex.pop();
    if (t1 != t2) {
        throw "wrong types are in operation";
    }
    if (op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH || op == LEX_MOD) {
        r = LEX_INT;
        if (t1 != r) {
            throw "wrong types are in operation";
        }
    }
    else if (op == LEX_AND || op == LEX_OR) {
        r = LEX_BOOLEAN;
        if (t1 != r) {
            throw "wrong types are in operation";
        }
    }
    else if (op == LEX_LEQ || op == LEX_GEQ || op == LEX_LESS ||
             op == LEX_GREATER|| op == LEX_NEQ || op == LEX_EQ) {
        r = LEX_BOOLEAN;
        if (t1 == LEX_BOOLEAN) {
            throw "wrong types are in operation";
        }
    }
    else if (op == LEX_PLUS) {
        if (t1 == LEX_BOOLEAN) {
            throw "wrong types are in operation";
        }
        r = t1;
    }
    if (r != LEX_NULL) {
        st_lex.push(r); 
    } else {
        throw "wrong types are in operation";
    }
    poliz.push_back(Lex(op));
}

void Parser::check_not() {
    if (st_lex.top() != LEX_BOOLEAN) {
        throw "wrong type is in not";
    }
    poliz.push_back(Lex(LEX_NOT));
}

void Parser::eq_type() {
    type_of_lex t;
    t = st_lex.top();
    st_lex.pop();
    if (t != st_lex.top())
        throw "wrong types are in assign";
    st_lex.pop();
}

void Parser::eq_bool() {
    if (st_lex.top() != LEX_BOOLEAN) {
        throw "expression is not boolean";
    st_lex.pop ();
    }
}

void Parser::check_id() {
    if (TID[c_val].get_declare()) {
        st_lex.push(TID[c_val].get_type());
    } else {
        throw "not declared";
    }
}

void Parser::check_id_in_read() {
    if (!TID[c_val].get_declare()) {
        throw "not declared";
    }
    if (TID[c_val].get_type() == LEX_BOOLEAN) {
        throw "Identificator can't be boolean";
    }
}

void Parser::change_break() {
    int k = 0;
    for (Lex l: poliz) {
        if (l.get_type() == LEX_BREAK) {
            int num_while = st_while.size();
                for (int i = num_while - 1; i >= 0; i--) {
                    if (st_while[i] < k) {
                        if (fin_while[i] > k) {
                            poliz[k] = Lex(POLIZ_LABEL, fin_while[i]);
                            break;
                        }
                    }
                }
        }
    k++;
    }
}

int main(int argc, char** argv) {
    try {
        Parser p("program.txt");
        p.analyze();
        return 0;
    }

    catch (const char* str) {
        cout << "Code terminated: " << str << endl;
    }

    catch (Lex l) {
        cout << "Unexpected Lexeme " << l << endl;
    }
}