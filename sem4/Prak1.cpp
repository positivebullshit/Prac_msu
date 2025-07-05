#include <iostream>
#include <string>

using namespace std;

class StringArray {

private:
    int LenArray;
    std::string** Array;
public:
    // Конструкторы
    StringArray() {  // пустой
        Array = nullptr;
        LenArray = 0;
    }
    StringArray(int len) {  // с известным числом строк
        Array = new std::string*[len];
        LenArray = len;
        for (int i = 0; i < LenArray; i++) {
            Array[i] = new std::string();
        }
    }

    // Деструктор
    ~StringArray() {
        for (int i = 0; i < LenArray; i++) {
            delete Array[i];
        }
        delete[] Array;
    }

    // Добавление строки в массив
    void add(const std::string& str) {
        std::string** NewArray = new std::string*[LenArray + 1];
        for (int i = 0; i < LenArray; i++) {
            NewArray[i] = Array[i];
        }
        NewArray[LenArray] = new std::string(str);
        delete[] Array;
        Array = NewArray;
        LenArray++;
    }

    // Получение длины массива
    int GetLenArray() const {
        return LenArray;
    }

    // Получение длины строки по ее индексу в массиве
    int GetLenString(int index) const {
        if (index < 0 || index >= LenArray) {
            throw std::out_of_range("Index out of range");
        }
        return Array[index]->size();
    }

    // Перегрузка оператора индексирования и обращения к строке
    const std::string& operator[] (int index) const {  // для константых объектов
        if (index < 0 || index >= LenArray) {
            throw std::out_of_range("Index out of range");
        }
        return *Array[index];
    }

    std::string& operator[] (int index) {  // для неконстантных объектов
        if (index < 0 || index >= LenArray) {
            throw std::out_of_range("Index out of range");
        }
        return *Array[index];
    }

    // Вывод элемента массива по индексу
    void PrintStr (int index) const {
        if (index < 0 || index >= LenArray) {
            throw std::out_of_range("Index out of range");
        }
        else {
            std::cout << "String with index " << index << ": " << *Array[index] << std::endl;
        }
    }

    // Вывод всего массива
    void PrintArray() const {
        for (int i = 0; i < LenArray; i++) {
            std::cout << *Array[i] << "; ";
        }
        std::cout << std::endl;
    }

    // Поэлементное сцепление двух массивов с образованием третьего
    StringArray plus(const StringArray& other) const {
        if (GetLenArray() != other.GetLenArray()) {
            std::cout << "Arrays have different lengths " << endl;
            std::abort();
        }
        StringArray NewArray(GetLenArray());
        for (int i = 0; i < GetLenArray(); i++) {
            NewArray[i] = (*this)[i] + other[i];
        }
        return NewArray;
    }

    // Слияние элементов двух массивов с исключением повторов и образованием третьего массива
    StringArray merge(const StringArray& other) const {
        int NewLenArray = (*this).GetLenArray();
        StringArray NewArray(NewLenArray);

        // Элементы первого массива
        for (int i = 0; i < (*this).GetLenArray(); i++) {
            NewArray[i] = (*this)[i];
        }

        // Элементы второго массива, убирая повторения
        for (int i = 0; i < other.GetLenArray(); i++) {
            const std::string& CurStr = other[i];
            bool found = false;
            for (int j = 0; j < (*this).GetLenArray(); j++) {
                if ((*this)[j] == CurStr) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                NewArray.add(CurStr);
                NewLenArray++;
            }
        }
        return NewArray;

    }

};

int main() {

    /*
    StringArray arr1(3);
    StringArray arr2;
    arr1[0] = "Hello";
    arr1[1] = "world";
    arr1[2] = "I'm Tim";
    arr1.PrintArray();
    std::cout << "Length of arr1 is " << arr1.GetLenArray() << std::endl;
    arr1.PrintStr(0);
    std::cout <<"Length of the string is " << arr1.GetLenString(1) << std::endl;
    std::string s = "Added string 1";
    std::string s1 = "Added string 2";
    arr2.add(s);
    arr2.add(s1);
    arr2.PrintArray();
    std::cout << "Length of arr2 is " << arr2.GetLenArray() << std::endl;
    std::string s2 = "Added string 3";
    arr2.add(s2);
    StringArray arr3 = arr1.plus(arr2);
    arr3.PrintArray();
    arr1.add(s2);
    std::cout << arr3.GetLenArray() << std::endl;
    StringArray arr4 = arr1.merge(arr2);
    arr4.PrintArray();
    std::cout << arr4.GetLenArray() << std::endl;
    std::string str = arr1[0];
    str[1] = 'j';
    std::cout << str << std::endl;
    arr1.PrintArray();
    arr1.PrintArray();
    */

    int len;
    std::cout << "Put length of arr2: ";
    std::cin >> len;
    StringArray arr1, arr2(len);
    std::cout << "Input " << len << " strings for arr2:" << std::endl;
    string s;
    std::getline(cin, s);
    for (int i = 0; i < len; i++) {
        std::getline(cin, s);
        arr2[i] = s;
    }

    int option;
    do {
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "1. Add string to arr1" << std::endl;
        std::cout << "2. Add string to arr2" << std::endl;
        std::cout << "3. Print lengths of arr1 and arr2" << std::endl;
        std::cout << "4. Print arr1" << std::endl;
        std::cout << "5. Print arr2" << std::endl;
        std::cout << "6. Elem-by-elem concatenation with saving result to arr3" << std::endl;
        std::cout << "7. Merging with saving result to arr4 (without duplicates)" << std::endl;
        std::cout << "8. Stop execution" << std::endl;
        std::cout << "Write the number of the option: " << std::endl;
        std::cin >> option;
        std::cout << std::endl;

        switch (option) {
            case 1: {
                std::cout << "Input string" << std::endl;
                string s;
                std::getline(cin, s);
                std::getline(cin, s);
                arr1.add(s);
                break;
            }
            case 2: {  // будем добавлять в arr1
                std::cout << "Input string" << std::endl;
                string s;
                std::getline(cin, s);
                std::getline(cin, s);
                arr2.add(s);
                break;
            }
            case 3: {
                std::cout << "arr1 size is " <<  arr1.GetLenArray() << std::endl;
                std::cout << "arr2 size is " <<  arr2.GetLenArray() << std::endl;
                break;
            }
            case 4: {
                std::cout << "arr1:" << std::endl;
                arr1.PrintArray();
                break;
            }
            case 5: {
                std::cout << "arr2:" << std::endl;
                arr2.PrintArray();
                break;
            }
            case 6: {
                StringArray arr3 = arr1.plus(arr2);
                std::cout << "Elem-by-elem concatenated arr3:" << std::endl;
                arr3.PrintArray();
                break;
            }
            case 7: {
                StringArray arr4 = arr1.merge(arr2);
                std::cout << "Merged arr4:" << std::endl;
                arr4.PrintArray();
                break;
            }
            case 8: {
                break;
            }
            default: {
                std::cout << "Incorrect option. Try again" << std::endl;
                break;
            }
        }
    } while (option != 8);

    return 0;
}