#include <iostream>
using namespace std;

// Class Declaration

class person2 {
    //Access - Specifier
public:
    int getCurrNum() {
      return number;
    }
    //Variable Declaration
    string name;
    int number;
};


class person {
    //Access - Specifier
public:
    int getCurrNum(const class person2 &obj) {
      obj.number++;
      return obj.number + number;
    }
    //Variable Declaration
    string name;
    int number;
};

class person3 {
    //Access - Specifier
public:
    int getCurrNum(class person2 obj) {
      return obj.number + number;
    }
    //Variable Declaration
    string name;
    int number;
};


static int help(class person obj, int i) {
  return obj.number + 1 - i;
}

static int help2(int *i) {
  return *i+2;
}

//Main Function

int main() {
    // Object Creation For Class
    person obj;

    //Get Input Values For Object Varibales
    cout << "Enter the Name :";
    cin >> obj.name;

    cout << "Enter the Number :";
    cin >> obj.number;

    //Show the Output
    cout << obj.name << ": " << obj.number << endl;

    return 0;
}
