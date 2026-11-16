// ============================================================
//  Customer.h
//  The people in the project:
//     Customer (parent)
//       |-- PassMember (gets 20% off)
//     Admin (runs the parking, has a private password)
// ============================================================
#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <bits/stdc++.h>
using namespace std;

// ------------------------------------------------------------
//  Customer : the owner of a vehicle
// ------------------------------------------------------------
class Customer
{
    protected:
    string name;
    string phone;

    public:
    Customer(string n="",string p="");      // constructor with default arguments
    virtual ~Customer();                    // destructor
    void setName(string n);                 // mutators
    void setPhone(string p);
    string getName();                       // accessors
    string getPhone();
    string getMaskedPhone();                // "98XXXXXX42" for printing on the ticket
    int hasPhone();                         // inquiry: 1 if a real number is saved
    virtual int getDiscountPercent();       // 0 for a normal customer
    virtual string getMemberType();
    static int isValidPhone(string p);      // inquiry: checks a 10-digit Indian mobile number
};


Customer::Customer(string n,string p){
    setName(n);
    setPhone(p);
}

Customer::~Customer(){
    // nothing to clean up
}

void Customer::setName(string n){
    if (n.length()>0){
        name = n;
    }
    else {
        name = "Guest";                 // empty name is not allowed
    }
}

void Customer::setPhone(string p){
    if (isValidPhone(p)==1){
        phone = p;
    }
    else {
        phone = "0000000000";           // wrong number, SMS will not be sent
    }
}

string Customer::getName(){
    return name;
}

string Customer::getPhone(){
    return phone;
}

string Customer::getMaskedPhone(){
    if (hasPhone()==0){
        return "No SMS";
    }
    return phone.substr(0,2)+"XXXXXX"+phone.substr(8,2);     // show only first 2 and last 2 digits
}

int Customer::hasPhone(){
    if (phone=="0000000000"){
        return 0;
    }
    return 1;
}

int Customer::getDiscountPercent(){
    return 0;
}

string Customer::getMemberType(){
    return "Regular";
}

int Customer::isValidPhone(string p){
    if (p.length()!=10){
        return 0;                       // must be exactly 10 digits
    }
    for (int i=0;i<10;i++){
        if (p[i]<'0' || p[i]>'9'){
            return 0;                   // only digits allowed
        }
    }
    if (p[0]<'6'){
        return 0;                       // Indian mobile numbers start with 6, 7, 8 or 9
    }
    return 1;
}


// ------------------------------------------------------------
//  PassMember : child of Customer (has a monthly pass)
// ------------------------------------------------------------
class PassMember : public Customer
{
    private:
    string passId;

    public:
    PassMember(string n="",string p="",string id="");
    ~PassMember();
    void setPassId(string id);
    string getPassId();
    int getDiscountPercent();           // replaces the parent's version: 20 instead of 0
    string getMemberType();
    static int isValidPassId(string id);
};

PassMember::PassMember(string n,string p,string id) : Customer(n,p){   // parent's constructor runs first
    setPassId(id);
}

PassMember::~PassMember(){
}

void PassMember::setPassId(string id){
    if (isValidPassId(id)==1){
        passId = id;
        for (int i=0;i<passId.length();i++){
            passId[i] = toupper(passId[i]);     // save it in capital letters
        }
    }
    else {
        passId = "PASS-000";
    }
}

string PassMember::getPassId(){
    return passId;
}

int PassMember::getDiscountPercent(){
    return 20;
}

string PassMember::getMemberType(){
    return "Pass Member";
}

int PassMember::isValidPassId(string id){
    // A valid pass looks like PASS-017 : "PASS-" followed by 3 digits
    if (id.length()!=8){
        return 0;
    }
    string start = id.substr(0,5);
    for (int i=0;i<start.length();i++){
        start[i] = toupper(start[i]);           // so "pass-017" also works
    }
    if (start!="PASS-"){
        return 0;
    }
    for (int i=5;i<8;i++){
        if (id[i]<'0' || id[i]>'9'){
            return 0;
        }
    }
    return 1;
}


// ------------------------------------------------------------
//  Admin : runs the parking. The password is PRIVATE,
//  so only login() and changePassword() can touch it.
// ------------------------------------------------------------
class Admin
{
    private:
    string password;

    public:
    Admin(string pass="admin123");          // constructor
    int login(string typed);                // inquiry: 1 = correct, 0 = wrong
    int changePassword(string oldPass,string newPass);
};

Admin::Admin(string pass){
    password = pass;
}

int Admin::login(string typed){
    if (typed==password){
        return 1;
    }
    return 0;
}

int Admin::changePassword(string oldPass,string newPass){
    if (oldPass!=password){
        return 0;                       // old password is wrong
    }
    if (newPass.length()<4){
        return 0;                       // new password too short
    }
    password = newPass;
    return 1;
}

#endif
