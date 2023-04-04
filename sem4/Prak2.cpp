#include <iostream>
#include <cstring>

using namespace std;

// I will describe titan classification from the anime "Attack on titan"

class Titans {
protected:
    char* name;
public:
    // static
    static int titan_counter;
    // constructor
    Titans(char* str) {
        int name_len = strlen(str);
        name = new char[name_len + 1];
        strcpy(name, str);
        titan_counter++;
    }
    // destructor
    virtual ~Titans() {
        delete[] name;
    }
    // virtual method
    virtual int Titan_speed() const = 0;
    // const method
    char* Get_name() const {
        return name;
    }
};

class Wall_Titans: public Titans {
    int height;
public:
    // constructor
    Wall_Titans(char* str, int h): Titans(str) {
        height = h;
    }
    // copy constructor
    Wall_Titans(const Wall_Titans& T): Titans(T.name) {
        height = T.height;
    }
    // speed of titan method
    virtual int Titan_speed() const {
        int speed = 0;
        if (height <= 3) {
            speed = 10;
        }
        else if (height <= 15) {
            speed = 15;
        }
        else {
            speed = 30;
        }
        return speed;
    }
    // assignment operator overload
    const Wall_Titans& operator=(const Wall_Titans& T) {
        int len_name = strlen(T.name);
        name = new char[len_name + 1];
        strcpy(name, T.name);
        height = T.height;
        return *this;
    }
    // reloading output operation to standard stream
    // use "friend" cos we should describe it inside
    // the class std::ostream like this:
    // ostream& operator<<(const Wall_Titans& T)
    // but the class std::stream already exists
    // so we say the operator is friendly
    friend std::ostream& operator<<(std::ostream& os, const Wall_Titans& T) {
        os << T.Get_name() << ", heigth: " << T.height << ", speed: " << T.Titan_speed() << std::endl;
        return os;
    }
};

class Human_Titans: public Titans {
    int power, number_of_legs;
public:
    // constructor
    Human_Titans(char* str, int p, int n): Titans(str) {
        power = p;
        number_of_legs = n;
    }
    // copy constructor
    Human_Titans (const Human_Titans& T): Titans(T.name) {
        power = T.power;
        number_of_legs = T.number_of_legs;
    }
    // speed of titan method
    virtual int Titan_speed() const {
        return 2 * power * number_of_legs;
    }
    // assignment operator overload
    const Human_Titans& operator=(const Human_Titans& T) {
        int len_name = strlen(T.name);
        name = new char[len_name + 1];
        strcpy(name, T.name);
        power = T.power;
        number_of_legs = T.number_of_legs;
        return *this;
    }
    // reloading output operation to standard stream
    friend std::ostream& operator<<(std::ostream& os, const Human_Titans& T) {
        os << T.Get_name() << ", power: " << T.power << ", number of legs: " << T.number_of_legs << ", speed: " << T.Titan_speed() << std::endl;
        return os;
    }
};

int Titans::titan_counter = 0;

int main() {
    Wall_Titans Noname("Noname", 100);
    Wall_Titans Another_Noname = Noname;
    Human_Titans Attack_Titan("Eren", 10, 2);
    Human_Titans Jaw_Titan("Falko", 6, 4);
    Human_Titans Jaw_Titan_Copy(Jaw_Titan);

    std::cout << "Oh no, there're " << Titans::titan_counter << " titans here!"<< std::endl;

    std::cout << "Titans:" << std::endl;
    std::cout << Noname;
    std::cout << Another_Noname;
    std::cout << Attack_Titan;
    std::cout << Jaw_Titan;
    std::cout << Jaw_Titan_Copy;

    Titans* All[3];
    All[0] = &Noname;
    All[1] = &Attack_Titan;
    All[2] = &Jaw_Titan;
    for (int i = 0; i < 3; i++) {
        std::cout << (*All[i]).Get_name() << "'s speed is " << All[i]->Titan_speed() << std::endl;
    }

    return 0;
}