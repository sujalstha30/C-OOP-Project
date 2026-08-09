#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>
#include <ctime>

using namespace std;

//  INPUT UTILITIES

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int getInt(const string &prompt) {
    int val;
    while (true) {
        cout << prompt;
        if (cin >> val) {
            clearInput();
            return val;
        }
        cout << "  [!] Invalid. Enter a number." << endl;
        clearInput();
    }
}

double getDouble(const string &prompt) {
    double val;
    while (true) {
        cout << prompt;
        if (cin >> val && val > 0) {
            clearInput();
            return val;
        }
        cout << "  [!] Enter a positive number." << endl;
        clearInput();
    }
}

string getString(const string &prompt) {
    string val;
    cout << prompt;
    getline(cin, val);
    return val;
}

string getDate() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    return to_string(1900 + ltm->tm_year) + "-" +
           to_string(1 + ltm->tm_mon)     + "-" +
           to_string(ltm->tm_mday);
}

//  ABSTRACTION - Abstract Base Classes (Interfaces)

// Abstract: anything that can be displayed
class IDisplayable {
public:
    virtual void display()       const = 0;
    virtual void displayDetail() const = 0;
    virtual ~IDisplayable() {}
};

// Abstract: anything that can be managed (CRUD)
class IManageable {
public:
    virtual void add()    = 0;
    virtual void remove() = 0;
    virtual void update() = 0;
    virtual ~IManageable() {}
};

// Abstract: Base Entity (every object has an ID and name)
class Entity {
protected:
    int    id;
    string name;

public:
    Entity(int i, string n) : id(i), name(n) {}

    virtual int    getID()   const { return id;   }
    virtual string getName() const { return name; }

    virtual ~Entity() {}
};
