#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>
#include <ctime>
#include <fstream>
#include <sstream>
using namespace std;

// ─────────────────────────────────────────────
// Input Helpers
// ─────────────────────────────────────────────
void clearInput() { 
    cin.clear(); 
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
}

int getInt(const string &p) {
    int v;
    while (true) {
        cout << p;
        if (cin >> v) { clearInput(); return v; }
        clearInput(); 
        cout << "  Invalid input.\n";
    }
}

string getString(const string &p) { 
    string v; 
    cout << p; 
    getline(cin, v); 
    return v; 
}

string getDate() {
    time_t n = time(0); 
    tm *t = localtime(&n);
    return to_string(1900 + t->tm_year) + "-" + 
           to_string(1 + t->tm_mon)     + "-" + 
           to_string(t->tm_mday);
}

// ─────────────────────────────────────────────
// String utilities for file I/O
// ─────────────────────────────────────────────

// Replace all '|' with a safe placeholder before saving
string encode(const string &s) {
    string r = s;
    for (char &c : r) if (c == '|') c = '\x01';
    return r;
}

// Restore placeholder back to '|'
string decode(const string &s) {
    string r = s;
    for (char &c : r) if (c == '\x01') c = '|';
    return r;
}

// Split a string by a delimiter character
vector<string> split(const string &line, char delim = '|') {
    vector<string> tokens;
    stringstream   ss(line);
    string         tok;
    while (getline(ss, tok, delim)) tokens.push_back(tok);
    return tokens;
}

// ─────────────────────────────────────────────
// File name constants
// ─────────────────────────────────────────────
const string FILE_PRODUCTS  = "products.txt";
const string FILE_CUSTOMERS = "customers.txt";
const string FILE_ORDERS    = "orders.txt";
const string FILE_COUNTERS  = "counters.txt";

// ─────────────────────────────────────────────
// Abstract Base
// ─────────────────────────────────────────────
class Entity {
protected:
    int    id;
    string name;
public:
    Entity(int i, string n) : id(i), name(n) {}
    int    getID()   const { return id;   }
    string getName() const { return name; }
    virtual ~Entity() {}
};

// ─────────────────────────────────────────────
// Product
// ─────────────────────────────────────────────
class Product : public Entity {
    string cat, desc;
    double price;
    int    stock;
public:
    Product(int i, string n, string c, double p, int s, string d)
        : Entity(i, n), cat(c), price(p), stock(s), desc(d) {}

    string getCategory() const { return cat;   }
    double getPrice()    const { return price; }
    int    getStock()    const { return stock; }
    string getDesc()     const { return desc;  }
    void   setPrice(double p)  { price = p;    }
    void   setStock(int s)     { stock = s;    }
    bool   isAvailable() const { return stock > 0; }
    bool   reduceStock(int q)  { if (q > stock) return false; stock -= q; return true; }

    // Serialize to a single pipe-delimited line
    string serialize() const {
        ostringstream ss;
        ss << fixed << setprecision(2);
        ss << id    << "|"
           << encode(name)  << "|"
           << encode(cat)   << "|"
           << price         << "|"
           << stock         << "|"
           << encode(desc);
        return ss.str();
    }

    // Deserialize from a pipe-delimited line
    static Product deserialize(const string &line) {
        vector<string> t = split(line);
        if (t.size() < 6) return Product(0, "", "", 0, 0, "");
        return Product(
            stoi(t[0]),
            decode(t[1]),
            decode(t[2]),
            stod(t[3]),
            stoi(t[4]),
            decode(t[5])
        );
    }

    void display() const {
        cout << "  " << left  << setw(6)  << id
             << setw(22) << name
             << setw(16) << cat
             << "Rs." << right << setw(10)
             << fixed << setprecision(2) << price
             << "  Stock:" << stock << "\n";
    }

    void displayDetail() const {
        cout << "\n  ID       : " << id
             << "\n  Name     : " << name
             << "\n  Category : " << cat
             << "\n  Price    : Rs." << fixed << setprecision(2) << price
             << "\n  Stock    : " << stock
             << "\n  Desc     : " << desc << "\n";
    }
};

// ─────────────────────────────────────────────
// Cart Item
// ─────────────────────────────────────────────
struct CartItem {
    int    pid;
    string pname;
    double price;
    int    qty;

    CartItem(int i, string n, double p, int q) 
        : pid(i), pname(n), price(p), qty(q) {}

    double subtotal() const { return price * qty; }

    // Format: pid~pname~price~qty
    string serialize() const {
        ostringstream ss;
        ss << fixed << setprecision(2);
        ss << pid << "~" << encode(pname) << "~" << price << "~" << qty;
        return ss.str();
    }

    static CartItem deserialize(const string &s) {
        vector<string> t = split(s, '~');
        if (t.size() < 4) return CartItem(0, "", 0, 0);
        return CartItem(stoi(t[0]), decode(t[1]), stod(t[2]), stoi(t[3]));
    }

    void display() const {
        cout << "  " << left << setw(6) << pid
             << setw(20) << pname
             << "Rs." << right << setw(9)
             << fixed << setprecision(2) << price
             << " x" << qty << " = Rs."
             << fixed << setprecision(2) << subtotal() << "\n";
    }
};

// ─────────────────────────────────────────────
// Order
// ─────────────────────────────────────────────
struct Order {
    int              oid;
    string           customer, payment, address, status, date;
    vector<CartItem> items;
    double           sub, ship, total;

    // Serialize order to multiple lines grouped by OID
    // Line format: oid|customer|payment|address|status|date|sub|ship|total|item1;item2;...
    string serialize() const {
        ostringstream ss;
        ss << fixed << setprecision(2);
        ss << oid                << "|"
           << encode(customer)   << "|"
           << encode(payment)    << "|"
           << encode(address)    << "|"
           << encode(status)     << "|"
           << date               << "|"
           << sub                << "|"
           << ship               << "|"
           << total              << "|";
        for (int i = 0; i < (int)items.size(); i++) {
            if (i) ss << ";";
            ss << items[i].serialize();
        }
        return ss.str();
    }

    static Order deserialize(const string &line) {
        Order o;
        vector<string> t = split(line);
        if (t.size() < 10) return o;
        o.oid      = stoi(t[0]);
        o.customer = decode(t[1]);
        o.payment  = decode(t[2]);
        o.address  = decode(t[3]);
        o.status   = decode(t[4]);
        o.date     = t[5];
        o.sub      = stod(t[6]);
        o.ship     = stod(t[7]);
        o.total    = stod(t[8]);
        // Items field (index 9) separated by ';'
        if (!t[9].empty()) {
            stringstream itemStream(t[9]);
            string       itemStr;
            while (getline(itemStream, itemStr, ';'))
                o.items.push_back(CartItem::deserialize(itemStr));
        }
        return o;
    }

    void show() const {
        cout << "\n  Order   : " << oid
             << "\n  Date    : " << date
             << "\n  Status  : " << status
             << "\n  Address : " << address
             << "\n  Payment : " << payment << "\n";
        for (auto &i : items) i.display();
        cout << "  Subtotal: Rs." << fixed << setprecision(2) << sub
             << "\n  Shipping: Rs." << ship
             << "\n  Total   : Rs." << total << "\n";
    }

    void track() const {
        cout << "\n  Order " << oid << " - " << status << "\n"
             << "  [x] Order Placed\n"
             << "  [x] Payment Confirmed\n"
             << (status == "Shipped" || status == "Delivered" ? "  [x]" : "  [ ]")
             << " Out for Delivery\n"
             << (status == "Delivered" ? "  [x]" : "  [ ]")
             << " Delivered\n";
    }
};

// ─────────────────────────────────────────────
// Abstract Payment
// ─────────────────────────────────────────────
class Payment {
protected:
    double amount;
    string status;
public:
    Payment(double a) : amount(a), status("Pending") {}
    virtual bool   processPayment()       = 0;
    virtual string getMethodName()  const = 0;
    virtual void   displayReceipt() const = 0;
    virtual ~Payment() {}
};

class CashOnDelivery : public Payment {
public:
    CashOnDelivery(double a) : Payment(a) {}
    bool processPayment() { 
        cout << "  Pay on delivery.\n"; 
        status = "Confirmed"; 
        return true; 
    }
    string getMethodName()  const { return "Cash on Delivery"; }
    void   displayReceipt() const { 
        cout << "  COD Rs." << fixed << setprecision(2) << amount << "\n"; 
    }
};

class ESewa : public Payment {
    string mob;
public:
    ESewa(double a) : Payment(a) {}
    bool processPayment() {
        mob = getString("  eSewa number : ");
        getString("  OTP          : ");
        cout << "  eSewa confirmed.\n";
        status = "Paid via eSewa";
        return true;
    }
    string getMethodName()  const { return "eSewa"; }
    void   displayReceipt() const { 
        cout << "  eSewa Rs." << fixed << setprecision(2) << amount 
             << " - " << mob << "\n"; 
    }
};

class Khalti : public Payment {
    string mob;
public:
    Khalti(double a) : Payment(a) {}
    bool processPayment() {
        mob = getString("  Khalti number: ");
        getString("  PIN          : ");
        cout << "  Khalti confirmed.\n";
        status = "Paid via Khalti";
        return true;
    }
    string getMethodName()  const { return "Khalti"; }
    void   displayReceipt() const { 
        cout << "  Khalti Rs." << fixed << setprecision(2) << amount 
             << " - " << mob << "\n"; 
    }
};

class BankTransfer : public Payment {
    string bank;
public:
    BankTransfer(double a) : Payment(a) {}
    bool processPayment() {
        cout << "  Banks: Nabil, NIC Asia, Himalayan, NMB\n";
        bank = getString("  Bank name    : ");
        getString("  Account no   : ");
        cout << "  Transfer initiated.\n";
        status = "Transfer Initiated";
        return true;
    }
    string getMethodName()  const { return "Bank Transfer (" + bank + ")"; }
    void   displayReceipt() const { 
        cout << "  Bank Rs." << fixed << setprecision(2) << amount 
             << " - " << bank << "\n"; 
    }
};

// ─────────────────────────────────────────────
// Abstract Shipping
// ─────────────────────────────────────────────
class ShippingStrategy {
public:
    virtual double calculateFee(const string &p)     const = 0;
    virtual string getCarrierName()                  const = 0;
    virtual int    getEstimatedDays(const string &p) const = 0;
    virtual ~ShippingStrategy() {}
};

class StandardShipping : public ShippingStrategy {
public:
    double calculateFee(const string &p) const {
        if (p == "Bagmati Pradesh") return 100;
        if (p == "Karnali Pradesh" || p == "Sudurpashchim Pradesh") return 350;
        return 200;
    }
    string getCarrierName() const { return "Nepal Post"; }
    int    getEstimatedDays(const string &p) const {
        if (p == "Bagmati Pradesh") return 2;
        if (p == "Karnali Pradesh" || p == "Sudurpashchim Pradesh") return 7;
        return 5;
    }
};

class ExpressShipping : public ShippingStrategy {
public:
    double calculateFee(const string &p) const {
        if (p == "Bagmati Pradesh") return 250;
        if (p == "Karnali Pradesh" || p == "Sudurpashchim Pradesh") return 600;
        return 400;
    }
    string getCarrierName() const { return "Sajha Express"; }
    int    getEstimatedDays(const string &p) const {
        if (p == "Bagmati Pradesh") return 1;
        if (p == "Karnali Pradesh" || p == "Sudurpashchim Pradesh") return 3;
        return 2;
    }
};

// ─────────────────────────────────────────────
// Base User
// ─────────────────────────────────────────────
class BaseUser : public Entity {
protected:
    string email, password, phone, address, province;
public:
    BaseUser(int i, string n, string e, string pass,
             string ph, string addr, string prov)
        : Entity(i, n), email(e), password(pass),
          phone(ph), address(addr), province(prov) {}

    virtual void showMenu()    = 0;
    virtual void displayInfo() = 0;

    bool   checkPassword(const string &p) const { return password == p; }
    string getEmail()    const { return email;    }
    string getPhone()    const { return phone;    }
    string getAddress()  const { return address;  }
    string getProvince() const { return province; }
    string getPassword() const { return password; }

    void setEmail   (string e) { email    = e; }
    void setPhone   (string p) { phone    = p; }
    void setAddress (string a) { address  = a; }
    void setProvince(string p) { province = p; }

    virtual ~BaseUser() {}
};

// ─────────────────────────────────────────────
// Customer
// ─────────────────────────────────────────────
class Customer : public BaseUser {
    vector<CartItem> cart;
    vector<Order>    orders;
public:
    Customer(int i, string u, string e, string pass,
             string ph, string addr, string prov)
        : BaseUser(i, u, e, pass, ph, addr, prov) {}

    // ── Menu & Info ──────────────────────────
    void showMenu() {
        cout << "\n  Namaste, " << name << "\n"
             << "  1. Browse Products\n"
             << "  2. Search\n"
             << "  3. View Cart\n"
             << "  4. Checkout\n"
             << "  5. Order History\n"
             << "  6. Track Order\n"
             << "  7. Logout\n";
    }

    void displayInfo() {
        cout << "\n  Name     : " << name
             << "\n  Email    : " << email
             << "\n  Phone    : " << phone
             << "\n  Address  : " << address
             << "\n  Province : " << province << "\n";
    }

    // ── Cart operations ──────────────────────
    double cartTotal() const {
        double t = 0;
        for (auto &i : cart) t += i.subtotal();
        return t;
    }

    void addToCart(int pid, string pn, double pr, int qty) {
        for (auto &i : cart) {
            if (i.pid == pid) { 
                i.qty += qty; 
                cout << "  Quantity updated.\n"; 
                return; 
            }
        }
        cart.push_back(CartItem(pid, pn, pr, qty));
        cout << "  " << pn << " added to cart.\n";
    }

    bool removeFromCart(int pid) {
        for (int i = 0; i < (int)cart.size(); i++)
            if (cart[i].pid == pid) { 
                cart.erase(cart.begin() + i); 
                return true; 
            }
        return false;
    }

    void              clearCart()       { cart.clear();        }
    bool              cartIsEmpty()     { return cart.empty(); }
    vector<CartItem>& getCart()         { return cart;         }
    void              addOrder(Order o) { orders.push_back(o); }
    bool              hasOrders()       { return !orders.empty(); }
    vector<Order>&    getOrders()       { return orders;       }

    void showCart() {
        cout << "\n  Cart\n";
        if (cart.empty()) { cout << "  Empty.\n"; return; }
        for (auto &i : cart) i.display();
        cout << "  Total: Rs." << fixed << setprecision(2) << cartTotal() << "\n";
    }

    void showOrders() {
        cout << "\n  Order History\n";
        if (orders.empty()) { cout << "  No orders yet.\n"; return; }
        for (auto &o : orders)
            cout << "  " << o.oid << "  " << o.date
                 << "  Rs." << fixed << setprecision(2) << o.total
                 << "  " << o.status << "\n";
    }

    // ── Serialization ────────────────────────
    // Customer line: id|name|email|password|phone|address|province
    string serialize() const {
        ostringstream ss;
        ss << id                  << "|"
           << encode(name)        << "|"
           << encode(email)       << "|"
           << encode(password)    << "|"
           << encode(phone)       << "|"
           << encode(address)     << "|"
           << encode(province);
        return ss.str();
    }

    static Customer deserialize(const string &line) {
        vector<string> t = split(line);
        if (t.size() < 7)
            return Customer(0, "", "", "", "", "", "");
        return Customer(
            stoi(t[0]),
            decode(t[1]),
            decode(t[2]),
            decode(t[3]),
            decode(t[4]),
            decode(t[5]),
            decode(t[6])
        );
    }
};

// ─────────────────────────────────────────────
// Admin
// ─────────────────────────────────────────────
class Admin : public BaseUser {
    string code;
public:
    Admin(int i, string u, string e, string pass,
          string ph, string addr, string prov, string c)
        : BaseUser(i, u, e, pass, ph, addr, prov), code(c) {}

    void showMenu() {
        cout << "\n  Admin Panel\n"
             << "  1. View Products\n"
             << "  2. Add Product\n"
             << "  3. Update Price\n"
             << "  4. Update Stock\n"
             << "  5. Remove Product\n"
             << "  6. View Customers\n"
             << "  7. Update Order Status\n"
             << "  8. Logout\n";
    }

    void displayInfo() { cout << "\n  Admin: " << name << "\n"; }
    bool verifyCode(const string &c) const { return code == c; }
};

// ─────────────────────────────────────────────
// FileManager  (all file I/O in one place)
// ─────────────────────────────────────────────
class FileManager {
public:
    // ── Counter persistence ──────────────────
    static void saveCounters(int pid, int cid, int oid) {
        ofstream f(FILE_COUNTERS);
        if (!f) return;
        f << pid << "\n" << cid << "\n" << oid << "\n";
    }

    static void loadCounters(int &pid, int &cid, int &oid) {
        ifstream f(FILE_COUNTERS);
        if (!f) return;          // use defaults if file missing
        f >> pid >> cid >> oid;
    }

    // ── Products ─────────────────────────────
    static void saveProducts(const vector<Product> &products) {
        ofstream f(FILE_PRODUCTS);
        if (!f) { cout << "  [Error] Cannot write " << FILE_PRODUCTS << "\n"; return; }
        for (auto &p : products) f << p.serialize() << "\n";
    }

    static vector<Product> loadProducts() {
        vector<Product> list;
        ifstream f(FILE_PRODUCTS);
        if (!f) return list;     // file may not exist yet
        string line;
        while (getline(f, line)) {
            if (line.empty()) continue;
            Product p = Product::deserialize(line);
            if (p.getID() != 0) list.push_back(p);
        }
        return list;
    }

    // ── Customers (profile only, no orders/cart) ─
    static void saveCustomers(const vector<Customer> &customers) {
        ofstream f(FILE_CUSTOMERS);
        if (!f) { cout << "  [Error] Cannot write " << FILE_CUSTOMERS << "\n"; return; }
        for (auto &c : customers) f << c.serialize() << "\n";
    }

    static vector<Customer> loadCustomers() {
        vector<Customer> list;
        ifstream f(FILE_CUSTOMERS);
        if (!f) return list;
        string line;
        while (getline(f, line)) {
            if (line.empty()) continue;
            Customer c = Customer::deserialize(line);
            if (c.getID() != 0) list.push_back(c);
        }
        return list;
    }

    // ── Orders (all customers, tagged by username) ─
    static void saveOrders(const vector<Customer> &customers) {
        ofstream f(FILE_ORDERS);
        if (!f) { cout << "  [Error] Cannot write " << FILE_ORDERS << "\n"; return; }
        for (auto &c : customers) {
            // Each order stored as: username|<order-serialized>
            for (auto &o : const_cast<Customer &>(c).getOrders())
                f << encode(c.getName()) << "|" << o.serialize() << "\n";
        }
    }

    // Load orders and assign them to matching customers
    static void loadOrders(vector<Customer> &customers) {
        ifstream f(FILE_ORDERS);
        if (!f) return;
        string line;
        while (getline(f, line)) {
            if (line.empty()) continue;
            // First token = username, rest = order data
            size_t sep = line.find('|');
            if (sep == string::npos) continue;
            string uname     = decode(line.substr(0, sep));
            string orderData = line.substr(sep + 1);
            Order  o         = Order::deserialize(orderData);
            if (o.oid == 0) continue;
            for (auto &c : customers)
                if (c.getName() == uname) { c.addOrder(o); break; }
        }
    }

    // ── Save everything at once ───────────────
    static void saveAll(const vector<Product>  &products,
                        const vector<Customer> &customers,
                        int pid, int cid, int oid) {
        saveProducts(products);
        saveCustomers(customers);
        saveOrders(customers);
        saveCounters(pid, cid, oid);
        cout << "  [Saved]\n";
    }
};

// ─────────────────────────────────────────────
// ShopSystem
// ─────────────────────────────────────────────
class ShopSystem {
    vector<Product>  products;
    vector<Customer> customers;
    Admin            admin;
    int  nextPID, nextCID, nextOID, loggedIn;
    bool adminIn;

    string prov[7] = {
        "Koshi Pradesh", "Madhesh Pradesh", "Bagmati Pradesh",
        "Gandaki Pradesh", "Lumbini Pradesh",
        "Karnali Pradesh", "Sudurpashchim Pradesh"
    };

    // ── Helpers ──────────────────────────────
    Product* findProduct(int id) {
        for (auto &p : products) if (p.getID() == id) return &p;
        return NULL;
    }

    int findCustomer(const string &u) {
        for (int i = 0; i < (int)customers.size(); i++)
            if (customers[i].getName() == u) return i;
        return -1;
    }

    string selectProvince() {
        cout << "\n  Select Province\n";
        for (int i = 0; i < 7; i++) 
            cout << "  " << i + 1 << ". " << prov[i] << "\n";
        int c = getInt("  Choice: ");
        return (c >= 1 && c <= 7) ? prov[c - 1] : "Bagmati Pradesh";
    }

    void showTable() {
        cout << "\n  " << left << setw(6) << "ID"
             << setw(22) << "Name"
             << setw(16) << "Category"
             << setw(12) << "Price" << "Stock\n";
        for (auto &p : products) p.display();
    }

    Payment* makePayment(int c, double amt) {
        switch (c) {
            case 1:  return new CashOnDelivery(amt);
            case 2:  return new ESewa(amt);
            case 3:  return new Khalti(amt);
            case 4:  return new BankTransfer(amt);
            default: return new CashOnDelivery(amt);
        }
    }

    ShippingStrategy* makeShipping(int c) {
        return (c == 2) ? (ShippingStrategy*) new ExpressShipping()
                        : (ShippingStrategy*) new StandardShipping();
    }

    // Load defaults only when no file exists
    void loadDefaultProducts() {
        int i = nextPID;
        products.push_back(Product(i++, "Samsung Galaxy A54", "Electronics", 49999, 20, "Latest Samsung"));
        products.push_back(Product(i++, "HP Laptop 15",       "Electronics", 75000, 10, "Popular HP laptop"));
        products.push_back(Product(i++, "Sony Headphones",    "Electronics",  5500, 35, "Noise cancelling"));
        products.push_back(Product(i++, "Lay's",              "Food",          350,200, "Classic chips"));
        products.push_back(Product(i++, "Wai Wai Noodles",   "Food",           30, 500,"Nepali noodles"));
        nextPID = i;
    }

    // Persist everything after any change
    void save() {
        FileManager::saveAll(products, customers, nextPID, nextCID, nextOID);
    }

    // ── Feature methods ───────────────────────

    void browseProducts() {
        cout << "\n  1. All Products\n  2. By Category\n  3. Product Detail\n";
        int c = getInt("  Choice: ");
        if (c == 1) {
            showTable();
        } else if (c == 2) {
            vector<string> cats;
            for (auto &p : products) {
                bool ex = false;
                for (auto &cat : cats) if (cat == p.getCategory()) ex = true;
                if (!ex) cats.push_back(p.getCategory());
            }
            for (int i = 0; i < (int)cats.size(); i++) 
                cout << "  " << i + 1 << ". " << cats[i] << "\n";
            int ch = getInt("  Choose: ");
            if (ch < 1 || ch > (int)cats.size()) { cout << "  Invalid.\n"; return; }
            for (auto &p : products) 
                if (p.getCategory() == cats[ch - 1]) p.display();
        } else if (c == 3) {
            showTable();
            Product *p = findProduct(getInt("  Product ID: "));
            if (p) p->displayDetail(); 
            else   cout << "  Not found.\n";
        }
        if (loggedIn != -1 && getInt("\n  Add to cart? 1=Yes 0=No: ") == 1)
            addToCart();
    }

    void searchProducts() {
        string kw    = getString("\n  Search: ");
        bool   found = false;
        for (auto &p : products)
            if (p.getName().find(kw)     != string::npos ||
                p.getCategory().find(kw) != string::npos)
            { p.display(); found = true; }
        if (!found) cout << "  No results.\n";
        else if (loggedIn != -1 && 
                 getInt("  Add to cart? 1=Yes 0=No: ") == 1) addToCart();
    }

    void addToCart() {
        if (loggedIn == -1) return;
        Customer &c = customers[loggedIn];
        Product *p  = findProduct(getInt("  Product ID: "));
        if (!p)                { cout << "  Not found.\n";    return; }
        if (!p->isAvailable()) { cout << "  Out of stock.\n"; return; }
        cout << "  Stock: " << p->getStock() << "\n";
        int qty = getInt("  Quantity: ");
        if (qty <= 0 || qty > p->getStock()) { cout << "  Invalid quantity.\n"; return; }
        c.addToCart(p->getID(), p->getName(), p->getPrice(), qty);
        // Cart is in-memory only; no save needed until checkout
    }

    void viewCart() {
        if (loggedIn == -1) return;
        Customer &c = customers[loggedIn];
        c.showCart();
        if (c.cartIsEmpty()) return;
        cout << "  1. Remove item\n  2. Checkout\n  3. Back\n";
        int ch = getInt("  Choice: ");
        if (ch == 1) {
            int pid = getInt("  Product ID: ");
            cout << (c.removeFromCart(pid) ? "  Removed.\n" : "  Not in cart.\n");
        } else if (ch == 2) {
            checkout();
        }
    }

    void checkout() {
        if (loggedIn == -1) return;
        Customer &c = customers[loggedIn];
        if (c.cartIsEmpty()) { cout << "  Cart is empty.\n"; return; }
        c.showCart();

        string addr = c.getAddress(), pr = c.getProvince();
        cout << "\n  Address : " << addr << "\n  Province: " << pr << "\n";
        if (getInt("  Use this address? 1=Yes 0=Change: ") == 0) {
            addr = getString("  New Address: ");
            pr   = selectProvince();
        }

        cout << "\n  1. Standard (Nepal Post)\n  2. Express (Sajha)\n";
        ShippingStrategy *ship = makeShipping(getInt("  Shipping: "));
        double fee  = ship->calculateFee(pr);
        int    days = ship->getEstimatedDays(pr);
        cout << "  " << ship->getCarrierName()
             << "  Rs." << fixed << setprecision(2) << fee
             << "  " << days << " days\n";

        cout << "\n  1.COD  2.eSewa  3.Khalti  4.Bank Transfer\n";
        int    pc    = getInt("  Payment: ");
        double sub   = c.cartTotal();
        double total = sub + fee;

        cout << "\n  Subtotal: Rs." << fixed << setprecision(2) << sub
             << "\n  Shipping: Rs." << fee
             << "\n  Total   : Rs." << total << "\n";

        if (getInt("\n  Confirm? 1=Yes 0=No: ") != 1) {
            cout << "  Cancelled.\n"; 
            delete ship; 
            return;
        }

        Payment *pay = makePayment(pc, total);
        pay->processPayment();
        pay->displayReceipt();

        // Reduce stock
        for (auto &item : c.getCart()) {
            Product *p = findProduct(item.pid);
            if (p) p->reduceStock(item.qty);
        }

        // Build order
        Order o;
        o.oid      = nextOID++;
        o.customer = c.getName();
        o.items    = c.getCart();
        o.sub      = sub;
        o.ship     = fee;
        o.total    = total;
        o.payment  = pay->getMethodName();
        o.status   = "Processing";
        o.date     = getDate();
        o.address  = addr;

        c.addOrder(o);
        c.clearCart();

        cout << "\n  Order " << o.oid 
             << " placed. Delivery in " << days << " days. Dhanyabad!\n";

        delete pay;
        delete ship;

        // Persist immediately after checkout
        save();
    }

    void viewOrders() {
        if (loggedIn == -1) return;
        Customer &c = customers[loggedIn];
        c.showOrders();
        if (!c.hasOrders()) return;
        if (getInt("  View detail? 1=Yes 0=No: ") == 1) {
            int oid = getInt("  Order ID: ");
            for (auto &o : c.getOrders()) 
                if (o.oid == oid) { o.show(); return; }
            cout << "  Not found.\n";
        }
    }

    void trackOrder() {
        if (loggedIn == -1) return;
        Customer &c = customers[loggedIn];
        if (!c.hasOrders()) { cout << "  No orders.\n"; return; }
        c.showOrders();
        int oid = getInt("  Order ID: ");
        for (auto &o : c.getOrders()) 
            if (o.oid == oid) { o.track(); return; }
        cout << "  Not found.\n";
    }

    // Admin: update any order's status across all customers
    void adminUpdateOrderStatus() {
        cout << "\n  All Orders\n";
        bool any = false;
        for (auto &c : customers)
            for (auto &o : c.getOrders()) {
                cout << "  OID:" << o.oid << "  " << c.getName()
                     << "  " << o.status << "  Rs."
                     << fixed << setprecision(2) << o.total << "\n";
                any = true;
            }
        if (!any) { cout << "  No orders.\n"; return; }

        int oid = getInt("  Enter Order ID to update: ");
        cout << "  New Status: 1.Processing  2.Shipped  3.Delivered  4.Cancelled\n";
        int sc = getInt("  Choice: ");
        string newStatus;
        switch (sc) {
            case 1: newStatus = "Processing"; break;
            case 2: newStatus = "Shipped";    break;
            case 3: newStatus = "Delivered";  break;
            case 4: newStatus = "Cancelled";  break;
            default: cout << "  Invalid.\n";  return;
        }
        for (auto &c : customers)
            for (auto &o : c.getOrders())
                if (o.oid == oid) { 
                    o.status = newStatus; 
                    cout << "  Status updated to " << newStatus << ".\n";
                    save();
                    return;
                }
        cout << "  Order not found.\n";
    }

    void adminPanel() {
        bool on = true;
        while (on) {
            admin.showMenu();
            int c = getInt("  Choice: ");
            if (c == 1) {
                showTable();

            } else if (c == 2) {
                string n   = getString("  Name     : ");
                string cat = getString("  Category : ");
                string d   = getString("  Desc     : ");
                double pr  = 0; 
                cout << "  Price    : "; cin >> pr; clearInput();
                int stk    = getInt("  Stock    : ");
                products.push_back(Product(nextPID++, n, cat, pr, stk, d));
                cout << "  Product added.\n";
                save();

            } else if (c == 3) {
                showTable();
                Product *p = findProduct(getInt("  ID: "));
                if (p) {
                    double pr = 0; 
                    cout << "  New Price: "; cin >> pr; clearInput();
                    p->setPrice(pr);
                    cout << "  Updated.\n";
                    save();
                } else cout << "  Not found.\n";

            } else if (c == 4) {
                showTable();
                Product *p = findProduct(getInt("  ID: "));
                if (p) { 
                    p->setStock(getInt("  New Stock: ")); 
                    cout << "  Updated.\n"; 
                    save();
                } else cout << "  Not found.\n";

            } else if (c == 5) {
                showTable();
                int id = getInt("  ID to remove: ");
                for (int i = 0; i < (int)products.size(); i++)
                    if (products[i].getID() == id) {
                        products.erase(products.begin() + i);
                        cout << "  Removed.\n";
                        save();
                        break;
                    }

            } else if (c == 6) {
                cout << "\n  Registered Customers\n";
                if (customers.empty()) cout << "  None.\n";
                else for (int i = 0; i < (int)customers.size(); i++) {
                    cout << "  " << i + 1 << ". "
                         << customers[i].getName() << "\n";
                }

            } else if (c == 7) {
                adminUpdateOrderStatus();

            } else if (c == 8) {
                adminIn = false; on = false;
                cout << "  Admin logged out.\n";

            } else cout << "  Invalid.\n";
        }
    }

public:
    ShopSystem()
        : nextPID(1001), nextCID(2001), nextOID(9001),
          loggedIn(-1), adminIn(false),
          admin(1, "admin", "admin@sastobazar.np", "admin123",
                "9800000000", "Kathmandu", "Bagmati Pradesh", "ADMIN2007")
    {
        // Load persisted counters first
        FileManager::loadCounters(nextPID, nextCID, nextOID);

        // Load products (fall back to defaults if no file)
        products = FileManager::loadProducts();
        if (products.empty()) {
            loadDefaultProducts();
            FileManager::saveProducts(products);   // create the file
            FileManager::saveCounters(nextPID, nextCID, nextOID);
        }

        // Load customers and their orders
        customers = FileManager::loadCustomers();
        FileManager::loadOrders(customers);

        cout << "  [Data loaded]\n";
    }

    // ── Auth ──────────────────────────────────
    void registerCustomer() {
        cout << "\n  Register\n";
        string u = getString("  Username : ");
        if (findCustomer(u) != -1) { cout << "  Username taken.\n"; return; }
        string p1 = getString("  Password : ");
        string p2 = getString("  Confirm  : ");
        if (p1 != p2) { cout << "  Passwords do not match.\n"; return; }
        string e  = getString("  Email    : ");
        string ph = getString("  Phone    : ");
        string a  = getString("  City     : ");
        customers.push_back(Customer(nextCID++, u, e, p1, ph, a, selectProvince()));
        cout << "  Account created! Swagat cha!\n";
        save();
    }

    void loginCustomer() {
        cout << "\n  Login\n";
        string u = getString("  Username : ");
        string p = getString("  Password : ");
        int idx  = findCustomer(u);
        if (idx == -1 || !customers[idx].checkPassword(p)) {
            cout << "  Invalid credentials.\n"; 
            return;
        }
        loggedIn = idx;
        cout << "  Namaste, " << u << "\n";
    }

    void loginAdmin() {
        cout << "\n  Admin Login\n";
        string u = getString("  Username : ");
        string p = getString("  Password : ");
        string c = getString("  Code     : ");
        if (admin.getName() == u && admin.checkPassword(p) && admin.verifyCode(c)) {
            adminIn = true; 
            cout << "  Access granted.\n";
        } else cout << "  Invalid credentials.\n";
    }

    // ── Main loop ─────────────────────────────
    void run() {
        cout << "\n  SastoBazar Nepal\n  Nepal ko Aafno Online Pasal\n";
        bool go = true;
        while (go) {
            if (adminIn) {
                adminPanel();
            } else if (loggedIn == -1) {
                cout << "\n  Main Menu\n"
                     << "  1. Browse Products\n"
                     << "  2. Login\n"
                     << "  3. Register\n"
                     << "  4. Admin Login\n"
                     << "  5. Exit\n";
                switch (getInt("  Choice: ")) {
                    case 1: browseProducts();   break;
                    case 2: loginCustomer();    break;
                    case 3: registerCustomer(); break;
                    case 4: loginAdmin();       break;
                    case 5:
                        save();   // final save on exit
                        cout << "  Dhanyabad! Feri Aaunuhos!\n";
                        go = false;
                        break;
                    default: cout << "  Invalid.\n";
                }
            } else {
                customers[loggedIn].showMenu();
                switch (getInt("  Choice: ")) {
                    case 1: browseProducts(); break;
                    case 2: searchProducts(); break;
                    case 3: viewCart();       break;
                    case 4: checkout();       break;
                    case 5: viewOrders();     break;
                    case 6: trackOrder();     break;
                    case 7:
                        cout << "  Logged out.\n";
                        loggedIn = -1;
                        break;
                    default: cout << "  Invalid.\n";
                }
            }
        }
    }
};

// ─────────────────────────────────────────────
int main() {
    ShopSystem shop;
    shop.run();
    return 0;
}