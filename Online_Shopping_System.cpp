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

//  ENCAPSULATION - Product Class (private data, public methods)

class Product : public Entity, public IDisplayable {
private:
    string category;
    double price;
    int    stock;
    string description;

public:
    Product(int i, string n, string c,
            double p, int s, string d)
        : Entity(i, n),
          category(c), price(p), stock(s), description(d) {}

    // Getters
    string getCategory()    const { return category;    }
    double getPrice()       const { return price;       }
    int    getStock()       const { return stock;       }
    string getDescription() const { return description; }

    // Setters
    void setPrice(double p)    { price    = p; }
    void setStock(int s)       { stock    = s; }
    void setCategory(string c) { category = c; }

    bool reduceStock(int qty) {
        if (qty <= 0 || qty > stock) return false;
        stock -= qty;
        return true;
    }

    void restoreStock(int qty) {
        if (qty > 0) stock += qty;
    }

    bool isAvailable() const { return stock > 0; }

    void display() const {
        cout << "  | " << left
             << setw(6)  << id
             << setw(22) << name
             << setw(18) << category
             << "Rs." << right << setw(10)
             << fixed << setprecision(2) << price
             << "  " << left << setw(5) << stock
             << " |" << endl;
    }

    void displayDetail() const {
        cout << endl;
        cout << "  ID          : " << id          << endl;
        cout << "  Name        : " << name        << endl;
        cout << "  Category    : " << category    << endl;
        cout << "  Price       : Rs. " << fixed
             << setprecision(2)   << price          << endl;
        cout << "  Stock       : " << stock       << endl;
        cout << "  Description : " << description << endl;
    }
};

//  CartItem and Order Structures

struct CartItem {
    int    productID;
    string productName;
    double price;
    int    quantity;

    CartItem(int pid, string pn, double pr, int q)
        : productID(pid), productName(pn),
          price(pr), quantity(q) {}

    double subtotal() const { return price * quantity; }

    void display() const {
        cout << "  " << left
             << setw(6)  << productID
             << setw(22) << productName
             << "Rs." << right << setw(9)
             << fixed << setprecision(2) << price
             << "  x" << left << setw(4) << quantity
             << "  Rs." << right << setw(10)
             << fixed << setprecision(2) << subtotal()
             << endl;
    }
};

struct Order {
    int             orderID;
    string          customerName;
    vector<CartItem>items;
    double          subtotal;
    double          vat;
    double          shipping;
    double          total;
    string          paymentMethod;
    string          status;
    string          date;
    string          address;
    string          province;

    void display() const {
        cout << endl;
        cout << "  ORDER #" << orderID << endl;
        cout << "  Customer : " << customerName << endl;
        cout << "  Date     : " << date         << endl;
        cout << "  Address  : " << address      << endl;
        cout << "  Province : " << province     << endl;
        cout << "  Payment  : " << paymentMethod<< endl;
        cout << "  Status   : " << status       << endl;
        cout << endl;
        cout << "  " << left
             << setw(6)  << "ID"
             << setw(22) << "Product"
             << setw(12) << "Price"
             << setw(6)  << "Qty"
             << "Subtotal" << endl;
        cout << "                                                  " << endl;
        for (int i = 0; i < (int)items.size(); i++)
            items[i].display();
        cout << "                                                  " << endl;
        cout << "  Subtotal  : Rs. " << fixed << setprecision(2) << subtotal  << endl;
        cout << "  VAT (13%) : Rs. " << fixed << setprecision(2) << vat       << endl;
        cout << "  Shipping  : Rs. " << fixed << setprecision(2) << shipping  << endl;
        cout << "  TOTAL     : Rs. " << fixed << setprecision(2) << total     << endl;
    }
};
