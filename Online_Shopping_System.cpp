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

//  ABSTRACTION - Abstract Payment Class


class Payment {
protected:
    double amount;
    string status;

public:
    Payment(double amt) : amount(amt), status("Pending") {}

    virtual bool   processPayment() = 0;
    virtual string getMethodName()  const = 0;
    virtual void   displayReceipt() const = 0;

    string getStatus() const { return status; }

    virtual ~Payment() {}
};

//  INHERITANCE + POLYMORPHISM - Payment Subclasses

class CashOnDelivery : public Payment {
public:
    CashOnDelivery(double amt) : Payment(amt) {}

    bool processPayment() {
        cout << "  [COD] Payment will be collected on delivery." << endl;
        status = "Confirmed";
        return true;
    }

    string getMethodName() const { return "Cash on Delivery"; }

    void displayReceipt() const {
        cout << "  Receipt: COD - Rs. "
             << fixed << setprecision(2) << amount << endl;
        cout << "  Status : " << status << endl;
    }
};

class ESewa : public Payment {
private:
    string mobileNumber;
public:
    ESewa(double amt) : Payment(amt) {}

    bool processPayment() {
        mobileNumber = getString("  eSewa mobile number : ");
        string otp   = getString("  Enter OTP           : ");
        cout << "  [eSewa] Payment of Rs. "
             << fixed << setprecision(2) << amount << " confirmed!" << endl;
        status = "Paid via eSewa";
        return true;
    }

    string getMethodName() const { return "eSewa"; }

    void displayReceipt() const {
        cout << "  Receipt : eSewa - Rs. "
             << fixed << setprecision(2) << amount << endl;
        cout << "  Mobile  : " << mobileNumber << endl;
        cout << "  Status  : " << status        << endl;
    }
};

class Khalti : public Payment {
private:
    string mobileNumber;
public:
    Khalti(double amt) : Payment(amt) {}

    bool processPayment() {
        mobileNumber = getString("  Khalti mobile number: ");
        string pin   = getString("  Enter Khalti PIN    : ");
        cout << "  [Khalti] Payment of Rs. "
             << fixed << setprecision(2) << amount << " confirmed!" << endl;
        status = "Paid via Khalti";
        return true;
    }

    string getMethodName() const { return "Khalti"; }

    void displayReceipt() const {
        cout << "  Receipt : Khalti - Rs. "
             << fixed << setprecision(2) << amount << endl;
        cout << "  Mobile  : " << mobileNumber << endl;
        cout << "  Status  : " << status        << endl;
    }
};

class IMEPay : public Payment {
private:
    string mobileNumber;
public:
    IMEPay(double amt) : Payment(amt) {}

    bool processPayment() {
        mobileNumber = getString("  IME Pay mobile      : ");
        string otp   = getString("  Enter OTP           : ");
        cout << "  [IME Pay] Payment of Rs. "
             << fixed << setprecision(2) << amount << " confirmed!" << endl;
        status = "Paid via IME Pay";
        return true;
    }

    string getMethodName() const { return "IME Pay"; }

    void displayReceipt() const {
        cout << "  Receipt : IME Pay - Rs. "
             << fixed << setprecision(2) << amount << endl;
        cout << "  Mobile  : " << mobileNumber << endl;
        cout << "  Status  : " << status        << endl;
    }
};

class ConnectIPS : public Payment {
private:
    string username;
public:
    ConnectIPS(double amt) : Payment(amt) {}

    bool processPayment() {
        username        = getString("  ConnectIPS Username : ");
        string password = getString("  ConnectIPS Password : ");
        cout << "  [ConnectIPS] Payment of Rs. "
             << fixed << setprecision(2) << amount << " confirmed!" << endl;
        status = "Paid via ConnectIPS";
        return true;
    }

    string getMethodName() const { return "ConnectIPS"; }

    void displayReceipt() const {
        cout << "  Receipt : ConnectIPS - Rs. "
             << fixed << setprecision(2) << amount << endl;
        cout << "  User    : " << username << endl;
        cout << "  Status  : " << status   << endl;
    }
};

class BankTransfer : public Payment {
private:
    string bankName;
    string accountNumber;
public:
    BankTransfer(double amt) : Payment(amt) {}

    bool processPayment() {
        cout << "  Available Banks: Nabil, NIC Asia, Everest," << endl;
        cout << "                   Himalayan, NMB, Siddhartha" << endl;
        bankName      = getString("  Bank Name           : ");
        accountNumber = getString("  Account Number      : ");
        cout << "  [Bank Transfer] Transfer of Rs. "
             << fixed << setprecision(2) << amount << " initiated." << endl;
        cout << "  [Bank Transfer] Will be verified in 1 hour." << endl;
        status = "Transfer Initiated";
        return true;
    }

    string getMethodName() const {
        return "Bank Transfer (" + bankName + ")";
    }

    void displayReceipt() const {
        cout << "  Receipt : Bank Transfer - Rs. "
             << fixed << setprecision(2) << amount << endl;
        cout << "  Bank    : " << bankName      << endl;
        cout << "  Account : " << accountNumber << endl;
        cout << "  Status  : " << status        << endl;
    }
};

//  ABSTRACTION - Abstract Shipping Strategy

class ShippingStrategy {
public:
    virtual double calculateFee(const string &province)  const = 0;
    virtual string getCarrierName()                      const = 0;
    virtual int    getEstimatedDays(const string &prov)  const = 0;
    virtual ~ShippingStrategy() {}
};

class StandardShipping : public ShippingStrategy {
public:
    double calculateFee(const string &province) const {
        if (province == "Bagmati Pradesh")       return 100.0;
        if (province == "Karnali Pradesh" ||
            province == "Sudurpashchim Pradesh") return 350.0;
        return 200.0;
    }

    string getCarrierName() const { return "Nepal Post Standard"; }

    int getEstimatedDays(const string &prov) const {
        if (prov == "Bagmati Pradesh")       return 2;
        if (prov == "Karnali Pradesh" ||
            prov == "Sudurpashchim Pradesh") return 7;
        return 5;
    }
};

class ExpressShipping : public ShippingStrategy {
public:
    double calculateFee(const string &province) const {
        if (province == "Bagmati Pradesh")       return 250.0;
        if (province == "Karnali Pradesh" ||
            province == "Sudurpashchim Pradesh") return 600.0;
        return 400.0;
    }

    string getCarrierName() const { return "Sajha Express Delivery"; }

    int getEstimatedDays(const string &prov) const {
        if (prov == "Bagmati Pradesh")       return 1;
        if (prov == "Karnali Pradesh" ||
            prov == "Sudurpashchim Pradesh") return 3;
        return 2;
    }
};