#include <iostream>

using namespace std;

class List {

    struct Node {
        int elem;
        Node* next;
        Node () {  // конструктор звена без параметров
            this->next = nullptr;
            this->elem = 0;
        }
        Node (int elem, Node* next = nullptr) {  // конструктор звена с параметром
            this->elem = elem;
            this->next = next;
        }
    };

private:
    Node* root;
    Node* end;
public:
    List() {  // конструктор без параметров
        this->root = nullptr;
        // (*this).root = nullptr;
        this->end = root;
    }
    List(int elem) {  // конструктор с параметром-числом
        this->root = new Node(elem);
        this->end = root;
    }
    List(const List &other) {  // конструктор копирования
        plus(other);
    }

    bool is_empty() {
        return !root;
    }


    void add(int x) {
        Node* tmp = new Node(x);
        if (is_empty()) {
            root = tmp;
            end = root;
        } else {
            end->next = tmp;
            end = end->next;
        }
    }

    void output() {
        Node* tmp = root;
        while (tmp) {
            cout<< tmp->elem << " ";
            tmp = tmp->next;
        }
        cout << "\n";
    }

    void del(int x) {
        if (is_empty()) {  // Если пусто то просто вернем ничего
            return;
        }
        Node* tmp = root;  // Звено, которое будет потенциально удалять
        if (root->elem == x && end == root) {  // Если один элемент и это корень
            root = nullptr;
            end = nullptr;
            delete tmp;
        } else if (root->elem == x) { // Если это корень, но он не единственный
            root = root->next;
            delete tmp;
        } else {  // Это не корень
            Node* prev = tmp;  // текущий элемент листа
            tmp = tmp->next;  // следующий элемент
            while (tmp) {
                if (tmp->elem == x) {  // проверяем следующее за нашим tmp звено
                    if (tmp == end) {
                        end = prev;
                    }
                    prev->next = tmp->next;
                    delete tmp;
                    break;
                }
                prev = tmp;
                tmp = tmp->next;
            }
        }
    }

    void endPlus(List &other) {  // Через адрес, чтобы не создавать новую переменную
        end->next = other.root;
    }

    void plus(const List &other) {
        Node* tmp = other.root;
        while (tmp) {
            add(tmp->elem);
            tmp = tmp->next;
        }
    }

    /*
    void plus(const List &other) {
        Node* other_tmp = other.root;
        while (other_tmp) {
            Node* tmp = new Node(other_tmp->elem);
            cout << tmp->elem;
            end->next = tmp;
            end = tmp->next;
            other_tmp = other_tmp->next;
        }

    }
    */
    List newPlus(const List &other1, const List &other2) {
        List newList(other1);
        newList.plus(other2);
        return newList;
    }


    void clear() {
        Node* tmp;
        while (root) {
            tmp = root;
            root = root->next;
            delete tmp;
        }
    }

    void operator= (const List &other) {
        clear();
        plus(other);
    }

    List operator+ (const List &other) {
        List tmp = newPlus(*this, other);
        return tmp;
    }
};

int main() {
    List a;
    List b(5);
    a.add(1);
    a.add(2);
    a.add(3);
    a.output();
    b.output();
    a.del(1);
    a.del(3);
    a.del(2);
    b.add(4);
    b.add(4);
    a.output();
    b.output();
    cout << "\n";
    a.plus(b);
    a.plus(b);
    a.output();
    a.clear();
    b.clear();
    a.output();
    b.output();
    return 0;
}