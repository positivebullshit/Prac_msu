#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>

using namespace std;

void Scanner() {
    enum state {H, IDENT, NUMB, COM, ASSIG, COMP, MATH};
    char c;
    int n;
    string buf;
    state CS = H;
    do {
        cin >> c;
        switch (CS) {
            case H:
                if (c == ' ' || c == '\n' || c == '\t');
                else if (isalpha(c)) {
                    buf.push_back(c);
                    CS = IDENT;
                }
                else if (isdigit(c)) {
                    n = c - '0';
                    CS = NUMB;
                }
                else if (c == '+'|| c == '-' || c == '*') {
                    CS = MATH;
                }
                else if (c == '/') {
                    cin >> c;
                    if (c == '*') {
                        CS = COM;
                    }
                    else {
                        cin.putback(c);
                        cout << "Division operation\n";
                        CS = MATH;
                    }
                }
                else if (c == '=') {
                    cin >> c;
                    if (c == '=') {
                        CS = COMP;
                    }
                    else {
                        cin.putback(c);
                        CS = ASSIG;
                    }
                }
                else if (c == '<' || c == '>') {
                    cin >> c;
                    if (c != '=') {
                        cin.putback(c);
                    }
                    CS = COMP;  
                }
                else if  (c == '!') {
                    cin >> c;
                    if (c == '=') {
                        CS = COMP;
                    }
                    else {
                        cout << "Error with NEQ\n";
                    }
                }
                else {
                    cin.putback(c);
                    cout << "Error with reading\n";
                }
                break;
            case IDENT:
                if (isdigit(c) || isalpha(c)) {
                    buf.push_back(c);
                }
                else {
                    cin.putback(c);
                    CS = H;
                }
                break;
            case NUMB:
                if (isdigit(c)) {
                    n = n * 10 + (c - '0');
                }
                else {
                    cin.putback(c);
                    CS = H;
                }
                break;
            case COM:
                if (c == '*') {
                    cin >> c;
                    if (c == '/') {
                        CS = H;
                    }
                    else {
                        cin.putback(c);
                    }
                }
                break;
            case ASSIG:
                cout << "Assigment operation\n";
                CS = H;
                break;
            case COMP:
                cout << "Comparison operation\n";
                CS = H;
                break; 
            case MATH:
                cout << "Arifmethical operations\n";
                CS = H;
                break;
        }
    } while (true);
}

int main(int argc, char** argv) {
    Scanner();
    return 0;
}