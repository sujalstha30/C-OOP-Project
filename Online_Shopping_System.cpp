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

//  ABSTRACTION
//  Abstract Base Classes (Interfaces)

//  Abstract: anything that can be displayed 
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

//  ENCAPSULATION
//  Product Class (private data, public methods)

class Product : public Entity, public IDisplayable {
private:
    // Private: hidden from outside 
    string category;
    double price;
    int    stock;
    string description;

public:
    //  Constructor 
    Product(int i, string n, string c,
            double p, int s, string d)
        : Entity(i, n),
          category(c), price(p), stock(s), description(d) {}

    // Getters (Controlled Access) 
    string getCategory()    const { return category;    }
    double getPrice()       const { return price;       }
    int    getStock()       const { return stock;       }
    string getDescription() const { return description; }

    // Setters (Controlled Modification)
    void setPrice(double p)    { price    = p; }
    void setStock(int s)       { stock    = s; }
    void setCategory(string c) { category = c; }

    // Stock Logic 
    bool reduceStock(int qty) {
        if (qty <= 0 || qty > stock) return false;
        stock -= qty;
        return true;
    }

    void restoreStock(int qty) {
        if (qty > 0) stock += qty;
    }

    bool isAvailable() const { return stock > 0; }

    //  IDisplayable Implementation 
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
        cout << "                                             " << endl;
        cout << "             PRODUCT DETAILS                 " << endl;
        cout << "                                             " << endl;
        cout << "   ID          : " << id          << endl;
        cout << "   Name        : " << name        << endl;
        cout << "   Category    : " << category    << endl;
        cout << "   Price       : Rs. " << fixed
             << setprecision(2)   << price          << endl;
        cout << "   Stock       : " << stock       << endl;
        cout << "   Description : " << description << endl;
        cout << "                                             " << endl;
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
        cout << "  ORDER #" << orderID << "     " << endl;
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
        cout << "                                                 " << endl;
        for (int i = 0; i < (int)items.size(); i++)
            items[i].display();
        cout << "                                                 " << endl;
        cout << "  Subtotal  : Rs. " << fixed << setprecision(2) << subtotal  << endl;
        cout << "  VAT (13%) : Rs. " << fixed << setprecision(2) << vat       << endl;
        cout << "  Shipping  : Rs. " << fixed << setprecision(2) << shipping  << endl;
        cout << "  TOTAL     : Rs. " << fixed << setprecision(2) << total     << endl;
    }
};


//  ABSTRACTION
//  Abstract Payment Class

class Payment {
protected:
    double amount;
    string status;

public:
    Payment(double amt) : amount(amt), status("Pending") {}

    // Pure Virtual: subclasses MUST implement 
    virtual bool   processPayment() = 0;
    virtual string getMethodName()  const = 0;
    virtual void   displayReceipt() const = 0;

    string getStatus() const { return status; }

    virtual ~Payment() {}
};


//  INHERITANCE + POLYMORPHISM

//  Cash on Delivery
class CashOnDelivery : public Payment {
public:
    CashOnDelivery(double amt) : Payment(amt) {}

    bool processPayment() {
        cout << "  [COD] Payment will be collected on delivery." << endl;
        status = "Confirmed";
        return true;
    }

    string getMethodName() const {
        return "Cash on Delivery";
    }

    void displayReceipt() const {
        cout << "  Receipt: COD - Rs. "
             << fixed << setprecision(2) << amount << endl;
        cout << "  Status : " << status << endl;
    }
};

// eSewa
class ESewa : public Payment {
private:
    string mobileNumber;

public:
    ESewa(double amt) : Payment(amt) {}

    bool processPayment() {
        mobileNumber = getString("  eSewa mobile number : ");
        string otp   = getString("  Enter OTP           : ");
        cout << "  [eSewa] Verifying..." << endl;
        cout << "  [eSewa] Payment of Rs. "
             << fixed << setprecision(2) << amount
             << " confirmed!" << endl;
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

//  Khalti 
class Khalti : public Payment {
private:
    string mobileNumber;

public:
    Khalti(double amt) : Payment(amt) {}

    bool processPayment() {
        mobileNumber = getString("  Khalti mobile number: ");
        string pin   = getString("  Enter Khalti PIN    : ");
        cout << "  [Khalti] Verifying..." << endl;
        cout << "  [Khalti] Payment of Rs. "
             << fixed << setprecision(2) << amount
             << " confirmed!" << endl;
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

// IME Pay 
class IMEPay : public Payment {
private:
    string mobileNumber;

public:
    IMEPay(double amt) : Payment(amt) {}

    bool processPayment() {
        mobileNumber = getString("  IME Pay mobile      : ");
        string otp   = getString("  Enter OTP           : ");
        cout << "  [IME Pay] Verifying..." << endl;
        cout << "  [IME Pay] Payment of Rs. "
             << fixed << setprecision(2) << amount
             << " confirmed!" << endl;
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

// ConnectIPS 
class ConnectIPS : public Payment {
private:
    string username;

public:
    ConnectIPS(double amt) : Payment(amt) {}

    bool processPayment() {
        username       = getString("  ConnectIPS Username : ");
        string password= getString("  ConnectIPS Password : ");
        cout << "  [ConnectIPS] Verifying..." << endl;
        cout << "  [ConnectIPS] Payment of Rs. "
             << fixed << setprecision(2) << amount
             << " confirmed!" << endl;
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

// Bank Transfer 
class BankTransfer : public Payment {
private:
    string bankName;
    string accountNumber;

public:
    BankTransfer(double amt) : Payment(amt) {}

    bool processPayment() {
        cout << "  Available Banks: Nabil, NIC Asia, Everest, " << endl;
        cout << "                   Himalayan, NMB, Siddhartha"  << endl;
        bankName      = getString("  Bank Name           : ");
        accountNumber = getString("  Account Number      : ");
        cout << "  [Bank Transfer] Transfer of Rs. "
             << fixed << setprecision(2) << amount
             << " initiated." << endl;
        cout << "  [Bank Transfer] Will be verified in 1 hour." << endl;
        status = "Transfer Initiated";
        return true;
    }

    string getMethodName() const { return "Bank Transfer (" + bankName + ")"; }

    void displayReceipt() const {
        cout << "  Receipt : Bank Transfer - Rs. "
             << fixed << setprecision(2) << amount << endl;
        cout << "  Bank    : " << bankName      << endl;
        cout << "  Account : " << accountNumber << endl;
        cout << "  Status  : " << status        << endl;
    }
};

//  ABSTRACTION
//  Abstract User Base Class

class BaseUser : public Entity {
protected:
    string email;
    string password;
    string phone;
    string address;
    string province;

public:
    BaseUser(int i, string n, string e,
             string pass, string ph,
             string addr, string prov)
        : Entity(i, n),
          email(e), password(pass), phone(ph),
          address(addr), province(prov) {}

    // Pure Virtual: subclasses define their own menu 
    virtual void showMenu()    = 0;
    virtual void displayInfo() const = 0;

    //  Common Methods (shared by all user types)
    bool checkPassword(const string &p) const {
        return password == p;
    }

    // Getters 
    string getEmail()    const { return email;    }
    string getPhone()    const { return phone;    }
    string getAddress()  const { return address;  }
    string getProvince() const { return province; }

    // Setters 
    void setEmail(string e)    { email    = e; }
    void setPhone(string p)    { phone    = p; }
    void setAddress(string a)  { address  = a; }
    void setProvince(string p) { province = p; }

    virtual ~BaseUser() {}
};

//  INHERITANCE
//  Customer Class (inherits BaseUser)
class Customer : public BaseUser {
private:
    // Private to Customer only 
    vector<CartItem> cart;
    vector<Order>    orders;

public:
    Customer(int i, string uname, string email,
             string pass, string phone,
             string addr, string prov)
        : BaseUser(i, uname, email, pass, phone, addr, prov) {}

    //  Polymorphic: overrides BaseUser's pure virtual 
    void showMenu() const {
        cout << endl;
        cout << "  Namaste, " << name << "! " << endl;
        cout << "  1. Browse Products"  << endl;
        cout << "  2. Search Product"   << endl;
        cout << "  3. View Cart"        << endl;
        cout << "  4. Checkout"         << endl;
        cout << "  5. Order History"    << endl;
        cout << "  6. Track Order"      << endl;
        cout << "  7. My Account"       << endl;
        cout << "  8. Logout"           << endl;
    }

    void showMenu() {}

    void displayInfo() const {
        cout << endl;
        cout << "  MY ACCOUNT: " << endl;
        cout << "  Username : " << name     << endl;
        cout << "  Email    : " << email    << endl;
        cout << "  Phone    : " << phone    << endl;
        cout << "  Address  : " << address  << endl;
        cout << "  Province : " << province << endl;
    }

    // Cart Operations 
    double cartTotal() const {
        double t = 0;
        for (int i = 0; i < (int)cart.size(); i++)
            t += cart[i].subtotal();
        return t;
    }

    void addToCart(int pid, string pn, double pr, int qty) {
        for (int i = 0; i < (int)cart.size(); i++) {
            if (cart[i].productID == pid) {
                cart[i].quantity += qty;
                cout << "  [+] Quantity updated in cart." << endl;
                return;
            }
        }
        cart.push_back(CartItem(pid, pn, pr, qty));
        cout << "  [+] " << pn << " added to cart!" << endl;
    }

    bool removeFromCart(int pid) {
        for (int i = 0; i < (int)cart.size(); i++) {
            if (cart[i].productID == pid) {
                cart.erase(cart.begin() + i);
                return true;
            }
        }
        return false;
    }

    void clearCart()            { cart.clear(); }
    bool cartIsEmpty()    const { return cart.empty(); }
    vector<CartItem>& getCart() { return cart; }

    //  Order Operations 
    void addOrder(Order o)      { orders.push_back(o); }
    bool hasOrders()      const { return !orders.empty(); }

    vector<Order>& getOrders()  { return orders; }

    void displayOrders() const {
        cout << endl;
        cout << " ORDER HISTORY: " << endl;
        if (orders.empty()) {
            cout << "  No orders yet." << endl;
            return;
        }
        for (int i = 0; i < (int)orders.size(); i++) {
            cout << "  Order #" << orders[i].orderID
                 << " | " << orders[i].date
                 << " | Rs. " << fixed << setprecision(2)
                 << orders[i].total
                 << " | " << orders[i].status << endl;
        }
    }

    void displayCartItems() const {
        cout << endl;
        cout << " YOUR CART: " << endl;

        if (cart.empty()) {
            cout << "  Cart is empty." << endl;
            return;
        }

        cout << "  " << left
             << setw(6)  << "ID"
             << setw(22) << "Product"
             << setw(12) << "Price"
             << setw(6)  << "Qty"
             << "Subtotal" << endl;
        cout << "                                                 " << endl;

        for (int i = 0; i < (int)cart.size(); i++)
            cart[i].display();

        cout << "                                                 " << endl;
        cout << "  TOTAL: Rs. "
             << fixed << setprecision(2) << cartTotal() << endl;
    }
};


//  INHERITANCE
//  Admin Class (inherits BaseUser)
class Admin : public BaseUser {
private:
    string adminCode;

public:
    Admin(int i, string uname, string email,
          string pass, string phone,
          string addr, string prov, string code)
        : BaseUser(i, uname, email, pass,
                   phone, addr, prov),
          adminCode(code) {}

    // Polymorphic: overrides BaseUser's pure virtual 
    void showMenu() {
        cout << endl;
        cout << "  ADMIN PANEL: " << endl;
        cout << "  1. View All Products"    << endl;
        cout << "  2. Add Product"          << endl;
        cout << "  3. Update Product Price" << endl;
        cout << "  4. Update Product Stock" << endl;
        cout << "  5. Remove Product"       << endl;
        cout << "  6. View All Customers"   << endl;
        cout << "  7. Logout"               << endl;
    }

    void displayInfo() const {
        cout << endl;
        cout << "  ADMIN INFO: " << endl;
        cout << "  Username : " << name     << endl;
        cout << "  Email    : " << email    << endl;
        cout << "  Phone    : " << phone    << endl;
    }

    bool verifyCode(const string &code) const {
        return adminCode == code;
    }
};


//  ABSTRACTION
//  Abstract Shipping Class
class ShippingStrategy {
public:
    // Pure Virtual
    virtual double   calculateFee(const string &province) const = 0;
    virtual string   getCarrierName()                     const = 0;
    virtual int      getEstimatedDays(const string &prov) const = 0;

    virtual ~ShippingStrategy() {}
};

//  INHERITANCE + POLYMORPHISM

class StandardShipping : public ShippingStrategy {
public:
    double calculateFee(const string &province) const {
        if (province == "Bagmati Pradesh")         return 100.0;
        if (province == "Karnali Pradesh" ||
            province == "Sudurpashchim Pradesh")   return 350.0;
        return 200.0;
    }

    string getCarrierName() const {
        return "Nepal Post Standard";
    }

    int getEstimatedDays(const string &prov) const {
        if (prov == "Bagmati Pradesh")         return 2;
        if (prov == "Karnali Pradesh" ||
            prov == "Sudurpashchim Pradesh")   return 7;
        return 5;
    }
};

// Express Shipping 
class ExpressShipping : public ShippingStrategy {
public:
    double calculateFee(const string &province) const {
        if (province == "Bagmati Pradesh")         return 250.0;
        if (province == "Karnali Pradesh" ||
            province == "Sudurpashchim Pradesh")   return 600.0;
        return 400.0;
    }

    string getCarrierName() const {
        return "Sajha Express Delivery";
    }

    int getEstimatedDays(const string &prov) const {
        if (prov == "Bagmati Pradesh")         return 1;
        if (prov == "Karnali Pradesh" ||
            prov == "Sudurpashchim Pradesh")   return 3;
        return 2;
    }
};

//  MAIN SHOP SYSTEM CLASS
class ShopSystem {
private:
    string           shopName;
    vector<Product>  products;
    vector<Customer> customers;
    Admin            admin;
    int              nextProductID;
    int              nextCustomerID;
    int              nextOrderID;
    int              loggedInCustomer; // -1 = not logged in
    bool             adminLoggedIn;

    string provinces[7] = {
        "Koshi Pradesh",
        "Madhesh Pradesh",
        "Bagmati Pradesh",
        "Gandaki Pradesh",
        "Lumbini Pradesh",
        "Karnali Pradesh",
        "Sudurpashchim Pradesh"
    };

    Product* findProduct(int id) {
        for (int i = 0; i < (int)products.size(); i++)
            if (products[i].getID() == id)
                return &products[i];
        return NULL;
    }

    int findCustomer(const string &uname) {
        for (int i = 0; i < (int)customers.size(); i++)
            if (customers[i].getName() == uname)
                return i;
        return -1;
    }

    string selectProvince() {
        cout << endl;
        cout << "  Select Province:" << endl;
        for (int i = 0; i < 7; i++)
            cout << "  " << i + 1 << ". " << provinces[i] << endl;
        int c = getInt("  Choice: ");
        if (c >= 1 && c <= 7) return provinces[c - 1];
        return "Bagmati Pradesh";
    }

    void printTableHeader() {
        cout << "                                                                   " << endl;
        cout << "  | " << left
             << setw(6)  << "ID"
             << setw(22) << "Name"
             << setw(18) << "Category"
             << setw(14) << "Price"
             << setw(5)  << "Stock"
             << " |" << endl;
        cout << "                                                                    " << endl;
    }

    void printTableFooter() {
        cout << "                                                                    " << endl;
    }

    // POLYMORPHISM: payment factory 
    Payment* createPayment(int choice, double amount) {
        switch (choice) {
            case 1: return new CashOnDelivery(amount);
            case 2: return new ESewa(amount);
            case 3: return new Khalti(amount);
            case 4: return new IMEPay(amount);
            case 5: return new ConnectIPS(amount);
            case 6: return new BankTransfer(amount);
            default:return new CashOnDelivery(amount);
        }
    }

    // POLYMORPHISM: shipping factory 
    ShippingStrategy* createShipping(int choice) {
        switch (choice) {
            case 1: return new StandardShipping();
            case 2: return new ExpressShipping();
            default:return new StandardShipping();
        }
    }

    void loadProducts() {
        int i = nextProductID;

        // Electronics
        products.push_back(Product(i++,
            "Samsung Galaxy A54","Electronics",
            49999.00, 20, "Latest Samsung smartphone"));
        products.push_back(Product(i++,
            "HP Laptop 15","Electronics",
            75000.00, 10, "Popular HP laptop"));
        products.push_back(Product(i++,
            "Sony Headphones","Electronics",
            5500.00, 35, "Noise-cancelling headphones"));
        products.push_back(Product(i++,
            "Inverter UPS","Electronics",
            8500.00, 15, "Load shedding backup power"));

        // Traditional Wear
        products.push_back(Product(i++,
            "Dhaka Topi","Traditional Wear",
            450.00, 100, "Nepali traditional topi"));
        products.push_back(Product(i++,
            "Daura Suruwal","Traditional Wear",
            2500.00, 60, "National dress of Nepal"));
        products.push_back(Product(i++,
            "Gunyo Cholo","Traditional Wear",
            3200.00, 50, "Traditional women dress"));
        products.push_back(Product(i++,
            "Pashmina Shawl","Traditional Wear",
            3500.00, 80, "Pure Himalayan pashmina"));

        // Food
        products.push_back(Product(i++,
            "Ilam Tea 500g","Food",
            350.00, 200, "Organic tea from Ilam"));
        products.push_back(Product(i++,
            "Wai Wai Noodles","Food",
            30.00, 500, "Popular Nepali noodles"));
        products.push_back(Product(i++,
            "Nepali Honey 500ml","Food",
            850.00, 120, "Wild honey from Chitwan"));
        products.push_back(Product(i++,
            "Mustang Apple 1kg","Food",
            280.00, 150, "Fresh Mustang apples"));

        // Handicrafts
        products.push_back(Product(i++,
            "Singing Bowl","Handicrafts",
            1200.00, 70, "Handmade Tibetan bowl"));
        products.push_back(Product(i++,
            "Thangka Painting","Handicrafts",
            4500.00, 30, "Traditional Thangka art"));
        products.push_back(Product(i++,
            "Khukuri Knife","Handicrafts",
            2800.00, 40, "Traditional Gurkha knife"));

        // Trekking
        products.push_back(Product(i++,
            "Trekking Boots","Trekking",
            7500.00, 25, "Himalayan trekking boots"));
        products.push_back(Product(i++,
            "Down Jacket","Trekking",
            5500.00, 30, "High altitude jacket"));
        products.push_back(Product(i++,
            "Hiking Backpack 60L","Trekking",
            4200.00, 20, "Multi-day trek backpack"));

        nextProductID = i;
    }

public:
    // Constructor 
    ShopSystem(string name)
        : shopName(name),
          nextProductID(1001),
          nextCustomerID(2001),
          nextOrderID(9001),
          loggedInCustomer(-1),
          adminLoggedIn(false),
          admin(1, "admin", "admin@sastobazar.com.np",
                "admin123", "9800000000",
                "Kathmandu", "Bagmati Pradesh", "ADMIN2007") {
        loadProducts();
    }

    //  CUSTOMER FUNCTIONS
    

    //  Register
    void registerCustomer() {
        cout << endl;
        cout << " REGISTER: " << endl;

        string uname = getString("  Username : ");
        if (findCustomer(uname) != -1) {
            cout << "  [!] Username taken." << endl;
            return;
        }

        string pass1 = getString("  Password : ");
        string pass2 = getString("  Confirm  : ");
        if (pass1 != pass2) {
            cout << "  [!] Passwords do not match." << endl;
            return;
        }

        string email   = getString("  Email    : ");
        string phone   = getString("  Phone    : ");
        string address = getString("  City     : ");
        string prov    = selectProvince();

        customers.push_back(
            Customer(nextCustomerID++, uname, email,
                     pass1, phone, address, prov));

        cout << "\n  [SUCCESS] Account created! Swagat cha!" << endl;
    }

    // Customer Login
    void loginCustomer() {
        cout << endl;
        cout << "  LOGIN: " << endl;

        string uname = getString("  Username : ");
        string pass  = getString("  Password : ");

        int idx = findCustomer(uname);
        if (idx == -1 || !customers[idx].checkPassword(pass)) {
            cout << "  [!] Invalid credentials." << endl;
            return;
        }

        loggedInCustomer = idx;
        cout << "  [SUCCESS] Namaste, " << uname << "!" << endl;
    }

    //  Admin Login 
    void loginAdmin() {
        cout << endl;
        cout << " ADMIN LOGIN: " << endl;

        string uname = getString("  Username  : ");
        string pass  = getString("  Password  : ");
        string code  = getString("  Admin Code: ");

        if (admin.getName() == uname &&
            admin.checkPassword(pass) &&
            admin.verifyCode(code)) {
            adminLoggedIn = true;
            cout << "  [SUCCESS] Admin access granted." << endl;
        } else {
            cout << "  [!] Invalid admin credentials." << endl;
        }
    }

    // Browse Products
    void browseProducts() {
        cout << endl;
        cout << "  1. All Products"    << endl;
        cout << "  2. By Category"     << endl;
        cout << "  3. Product Detail"  << endl;
        int c = getInt("  Choice: ");

        if (c == 1) {
            printTableHeader();
            for (int i = 0; i < (int)products.size(); i++)
                products[i].display();  // POLYMORPHISM via IDisplayable
            printTableFooter();

        } else if (c == 2) {
            vector<string> cats;
            for (int i = 0; i < (int)products.size(); i++) {
                bool exists = false;
                for (int j = 0; j < (int)cats.size(); j++)
                    if (cats[j] == products[i].getCategory())
                    { exists = true; break; }
                if (!exists) cats.push_back(products[i].getCategory());
            }

            cout << endl << "  Categories:" << endl;
            for (int i = 0; i < (int)cats.size(); i++)
                cout << "  " << i + 1 << ". " << cats[i] << endl;

            int ch = getInt("  Choose: ");
            if (ch < 1 || ch > (int)cats.size()) {
                cout << "  [!] Invalid." << endl;
                return;
            }

            printTableHeader();
            for (int i = 0; i < (int)products.size(); i++)
                if (products[i].getCategory() == cats[ch - 1])
                    products[i].display();
            printTableFooter();

        } else if (c == 3) {
            printTableHeader();
            for (int i = 0; i < (int)products.size(); i++)
                products[i].display();
            printTableFooter();
            int id = getInt("  Product ID: ");
            Product *p = findProduct(id);
            if (p) p->displayDetail(); // POLYMORPHISM via IDisplayable
            else   cout << "  [!] Not found." << endl;
        }

        if (loggedInCustomer != -1) {
            int add = getInt("\n  Add to cart? (1=Yes / 0=No): ");
            if (add == 1) addToCart();
        }
    }

    //  Search
    void searchProducts() {
        string kw = getString("\n  Search: ");
        bool found = false;

        printTableHeader();
        for (int i = 0; i < (int)products.size(); i++) {
            if (products[i].getName().find(kw)     != string::npos ||
                products[i].getCategory().find(kw) != string::npos) {
                products[i].display();
                found = true;
            }
        }
        printTableFooter();

        if (!found) cout << "  [!] Kei vetiyena. Nothing found." << endl;
    }

    //  Add to Cart
    void addToCart() {
        if (loggedInCustomer == -1) return;
        Customer &c = customers[loggedInCustomer];

        int id = getInt("  Product ID: ");
        Product *p = findProduct(id);

        if (!p) {
            cout << "  [!] Product not found." << endl;
            return;
        }
        if (!p->isAvailable()) {
            cout << "  [!] Out of stock! Stock chaina." << endl;
            return;
        }

        cout << "  Stock: " << p->getStock() << endl;
        int qty = getInt("  Quantity: ");

        if (qty <= 0 || qty > p->getStock()) {
            cout << "  [!] Invalid quantity." << endl;
            return;
        }

        c.addToCart(p->getID(), p->getName(), p->getPrice(), qty);
    }

    //  View Cart 
    void viewCart() {
        if (loggedInCustomer == -1) return;
        Customer &c = customers[loggedInCustomer];

        c.displayCartItems();

        if (c.cartIsEmpty()) return;

        cout << endl;
        cout << "  1. Remove item"     << endl;
        cout << "  2. Checkout"        << endl;
        cout << "  3. Back"            << endl;
        int ch = getInt("  Choice: ");

        if (ch == 1) {
            int pid = getInt("  Product ID to remove: ");
            if (c.removeFromCart(pid))
                cout << "  [+] Removed." << endl;
            else
                cout << "  [!] Not found in cart." << endl;
        } else if (ch == 2) {
            checkout();
        }
    }

    // Checkout (uses Payment + Shipping polymorphism) 
    void checkout() {
        if (loggedInCustomer == -1) return;
        Customer &c = customers[loggedInCustomer];

        if (c.cartIsEmpty()) {
            cout << "  [!] Cart is empty!" << endl;
            return;
        }

        c.displayCartItems();

        cout << endl;
        cout << "  ===== CHECKOUT =====" << endl;
        cout << "  Address  : " << c.getAddress()  << endl;
        cout << "  Province : " << c.getProvince() << endl;

        int useAddr = getInt("  Use this address? (1=Yes / 0=Change): ");

        string address  = c.getAddress();
        string province = c.getProvince();

        if (useAddr == 0) {
            address  = getString("  New Address: ");
            province = selectProvince();
        }

        //  POLYMORPHISM: Choose Shipping 
        cout << endl;
        cout << "  Shipping Method:" << endl;
        cout << "  1. Standard (Nepal Post)" << endl;
        cout << "  2. Express (Sajha Express)" << endl;
        int sc = getInt("  Choice: ");

        ShippingStrategy *shipping = createShipping(sc);
        double shippingFee = shipping->calculateFee(province);
        int    estDays     = shipping->getEstimatedDays(province);
        string carrier     = shipping->getCarrierName();

        cout << "  Carrier  : " << carrier     << endl;
        cout << "  Fee      : Rs. " << fixed
             << setprecision(2) << shippingFee  << endl;
        cout << "  Est. Days: " << estDays     << " days" << endl;

        //  POLYMORPHISM: Choose Payment
        cout << endl;
        cout << "  Payment Method:"    << endl;
        cout << "  1. Cash on Delivery"<< endl;
        cout << "  2. eSewa"           << endl;
        cout << "  3. Khalti"          << endl;
        cout << "  4. IME Pay"         << endl;
        cout << "  5. ConnectIPS"      << endl;
        cout << "  6. Bank Transfer"   << endl;
        int pc = getInt("  Choice: ");

        double subtotal = c.cartTotal();
        double vat      = subtotal * 0.13;
        double total    = subtotal + vat + shippingFee;

        // Summary
        cout << endl;
        cout << "  ORDER SUMMARY: " << endl;
        cout << "  Subtotal  : Rs. " << fixed << setprecision(2) << subtotal    << endl;
        cout << "  VAT (13%) : Rs. " << fixed << setprecision(2) << vat         << endl;
        cout << "  Shipping  : Rs. " << fixed << setprecision(2) << shippingFee << endl;
        cout << "  --------------------------"                                   << endl;
        cout << "  TOTAL     : Rs. " << fixed << setprecision(2) << total       << endl;
        cout << "  Address   : "     << address                                 << endl;
        cout << "  Province  : "     << province                                << endl;

        int confirm = getInt("\n  Confirm? (1=Yes / 0=Cancel): ");
        if (confirm != 1) {
            cout << "  [!] Cancelled." << endl;
            delete shipping;
            return;
        }

        // Process Payment (POLYMORPHISM)
        Payment *payment = createPayment(pc, total);
        if (!payment->processPayment()) {
            cout << "  [!] Payment failed." << endl;
            delete payment;
            delete shipping;
            return;
        }

        //  Display Receipt (POLYMORPHISM) 
        payment->displayReceipt();

        //  Reduce Stock 
        vector<CartItem> &cart = c.getCart();
        for (int i = 0; i < (int)cart.size(); i++) {
            Product *p = findProduct(cart[i].productID);
            if (p) p->reduceStock(cart[i].quantity);
        }

        //  Create Order 
        Order order;
        order.orderID      = nextOrderID++;
        order.customerName = c.getName();
        order.items        = cart;
        order.subtotal     = subtotal;
        order.vat          = vat;
        order.shipping     = shippingFee;
        order.total        = total;
        order.paymentMethod= payment->getMethodName();
        order.status       = "Processing";
        order.date         = getDate();
        order.address      = address;
        order.province     = province;

        c.addOrder(order);
        c.clearCart();

        cout << endl;
        cout << "  [SUCCESS] Order placed! Dhanyabad!" << endl;
        cout << "  Order ID  : " << order.orderID << endl;
        cout << "  Carrier   : " << carrier       << endl;
        cout << "  Delivery  : " << estDays << " business days" << endl;

        order.display();

        delete payment;
        delete shipping;
    }

    // Order History 
    void viewOrderHistory() {
        if (loggedInCustomer == -1) return;
        Customer &c = customers[loggedInCustomer];

        c.displayOrders();

        if (!c.hasOrders()) return;

        int view = getInt("\n  View detail? (1=Yes / 0=No): ");
        if (view == 1) {
            int oid = getInt("  Order ID: ");
            vector<Order> &orders = c.getOrders();
            bool found = false;
            for (int i = 0; i < (int)orders.size(); i++) {
                if (orders[i].orderID == oid) {
                    orders[i].display();
                    found = true;
                    break;
                }
            }
            if (!found) cout << "  [!] Order not found." << endl;
        }
    }

    // Track Order 
    void trackOrder() {
        if (loggedInCustomer == -1) return;
        Customer &c = customers[loggedInCustomer];

        int oid = getInt("\n  Order ID to track: ");
        vector<Order> &orders = c.getOrders();

        for (int i = 0; i < (int)orders.size(); i++) {
            if (orders[i].orderID == oid) {
                Order &o = orders[i];
                cout << endl;
                cout << "  ===== TRACKING ORDER #" << o.orderID << " =====" << endl;
                cout << "  Status   : " << o.status   << endl;
                cout << "  Province : " << o.province << endl;
                cout << endl;
                cout << "  [x] Order Placed"       << endl;
                cout << "  [x] Payment Confirmed"  << endl;
                if (o.status == "Shipped" || o.status == "Delivered") {
                    cout << "  [x] Packed"         << endl;
                    cout << "  [x] Out for Delivery"<< endl;
                } else {
                    cout << "  [ ] Packed"         << endl;
                    cout << "  [ ] Out for Delivery"<< endl;
                }
                cout << (o.status == "Delivered"
                         ? "  [x]" : "  [ ]")
                     << " Delivered"               << endl;
                return;
            }
        }
        cout << "  [!] Order not found." << endl;
    }

    //  My Account 
    void myAccount() {
        if (loggedInCustomer == -1) return;
        Customer &c = customers[loggedInCustomer];

        // POLYMORPHISM: calls Customer's displayInfo()
        c.displayInfo();

        cout << endl;
        cout << "  1. Update Email"    << endl;
        cout << "  2. Update Phone"    << endl;
        cout << "  3. Update Address"  << endl;
        cout << "  4. Update Province" << endl;
        cout << "  5. Back"            << endl;
        int ch = getInt("  Choice: ");

        if (ch == 1) {
            c.setEmail(getString("  New Email: "));
            cout << "  [+] Updated." << endl;
        } else if (ch == 2) {
            c.setPhone(getString("  New Phone: "));
            cout << "  [+] Updated." << endl;
        } else if (ch == 3) {
            c.setAddress(getString("  New Address: "));
            cout << "  [+] Updated." << endl;
        } else if (ch == 4) {
            c.setProvince(selectProvince());
            cout << "  [+] Updated." << endl;
        }
    }

    
    //  ADMIN FUNCTIONS
    
    void adminPanel() {
        bool inAdmin = true;
        while (inAdmin) {
            // POLYMORPHISM: calls Admin's showMenu()
            admin.showMenu();
            int c = getInt("  Choice: ");

            if (c == 1) {
                // View all products
                printTableHeader();
                for (int i = 0; i < (int)products.size(); i++)
                    products[i].display();
                printTableFooter();

            } else if (c == 2) {
                // Add product
                cout << endl;
                cout << "  ADD PRODUCT: " << endl;
                string n    = getString("  Name        : ");
                string cat  = getString("  Category    : ");
                double pr   = getDouble("  Price (Rs.) : ");
                int    stk  = getInt   ("  Stock       : ");
                string desc = getString("  Description : ");

                products.push_back(Product(nextProductID++,
                                           n, cat, pr, stk, desc));
                cout << "  [+] Product added." << endl;

            } else if (c == 3) {
                // Update price
                printTableHeader();
                for (int i = 0; i < (int)products.size(); i++)
                    products[i].display();
                printTableFooter();
                int id = getInt("  Product ID: ");
                Product *p = findProduct(id);
                if (p) {
                    double np = getDouble("  New Price: Rs. ");
                    p->setPrice(np);
                    cout << "  [+] Price updated." << endl;
                } else {
                    cout << "  [!] Not found." << endl;
                }

            } else if (c == 4) {
                // Update stock
                printTableHeader();
                for (int i = 0; i < (int)products.size(); i++)
                    products[i].display();
                printTableFooter();
                int id = getInt("  Product ID: ");
                Product *p = findProduct(id);
                if (p) {
                    int ns = getInt("  New Stock: ");
                    p->setStock(ns);
                    cout << "  [+] Stock updated." << endl;
                } else {
                    cout << "  [!] Not found." << endl;
                }

            } else if (c == 5) {
                // Remove product
                printTableHeader();
                for (int i = 0; i < (int)products.size(); i++)
                    products[i].display();
                printTableFooter();
                int id = getInt("  Product ID to remove: ");
                for (int i = 0; i < (int)products.size(); i++) {
                    if (products[i].getID() == id) {
                        products.erase(products.begin() + i);
                        cout << "  [+] Product removed." << endl;
                        break;
                    }
                }

            } else if (c == 6) {
                // View all customers
                cout << endl;
                cout << "  ===== ALL CUSTOMERS =====" << endl;
                if (customers.empty()) {
                    cout << "  No customers yet." << endl;
                } else {
                    for (int i = 0; i < (int)customers.size(); i++) {
                        cout << "  " << i + 1 << ". "
                             << customers[i].getName()
                             << " | " << customers[i].getEmail()
                             << " | " << customers[i].getProvince()
                             << endl;
                    }
                }

            } else if (c == 7) {
                adminLoggedIn = false;
                inAdmin = false;
                cout << "  [+] Admin logged out." << endl;
            } else {
                cout << "  [!] Invalid choice." << endl;
            }
        }
    }

    // Main Run Loop 
    void run() {
        cout << endl;
        cout << "                                              " << endl;
        cout << "           " << shopName                       << endl;
        cout << "       Nepal ko Aafno Online Pasal!"           << endl;
        cout << "     Delivering Across All 7 Provinces"        << endl;
        cout << "                                              " << endl;

        bool running = true;
        while (running) {

            if (adminLoggedIn) {
                adminPanel();

            } else if (loggedInCustomer == -1) {
                // ── GUEST MENU ───────────────────────────────
                cout << endl;
                cout << "  MAIN MENU: " << endl;
                cout << "  1. Browse Products"    << endl;
                cout << "  2. Login"              << endl;
                cout << "  3. Register"           << endl;
                cout << "  4. Admin Login"        << endl;
                cout << "  5. Exit"               << endl;
                int c = getInt("  Choice: ");

                switch (c) {
                    case 1: browseProducts();    break;
                    case 2: loginCustomer();     break;
                    case 3: registerCustomer();  break;
                    case 4: loginAdmin();        break;
                    case 5:
                        cout << "\n  Dhanyabad! Feri Aaunuhos!" << endl;
                        running = false;
                        break;
                    default:
                        cout << "  [!] Invalid choice." << endl;
                }

            } else {
                // CUSTOMER MENU 
                // POLYMORPHISM: calls Customer's showMenu()
                customers[loggedInCustomer].showMenu();
                int c = getInt("  Choice: ");

                switch (c) {
                    case 1: browseProducts();   break;
                    case 2: searchProducts();   break;
                    case 3: viewCart();         break;
                    case 4: checkout();         break;
                    case 5: viewOrderHistory(); break;
                    case 6: trackOrder();       break;
                    case 7: myAccount();        break;
                    case 8:
                        cout << "\n  Logged out. Dhanyabad!" << endl;
                        loggedInCustomer = -1;
                        break;
                    default:
                        cout << "  [!] Invalid choice." << endl;
                }
            }
        }
    }
};


//  MAIN
int main() {
    ShopSystem shop("SastoBazar Nepal");
    shop.run();
    return 0;
}