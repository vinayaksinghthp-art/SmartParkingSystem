// ============================================================
//  Vehicle.h
//  The Vehicle family:
//     Vehicle (parent, abstract)
//       |-- Bike
//       |-- Car
//       |     |-- ElectricCar
//       |-- Truck
// ============================================================
#ifndef VEHICLE_H
#define VEHICLE_H

#include <bits/stdc++.h>
using namespace std;

// ------------------------------------------------------------
//  Vehicle : the parent class (a blueprint only)
// ------------------------------------------------------------
class Vehicle
{
    protected:
    string vehicleNumber;               // protected: children can use it, outsiders cannot

    public:
    static int showMessages;            // 1 = print when objects are made / destroyed (for demo)

    Vehicle(string number="");          // constructor
    virtual ~Vehicle();                 // destructor (virtual, so the child's destructor also runs)

    void setNumber(string number);      // mutator
    string getNumber();                 // accessor

    virtual string getType()=0;         // pure virtual functions:
    virtual string getSlotType()=0;     // every child MUST write its own version.
    virtual int getFirstHourRate()=0;   // Because of these, Vehicle is an ABSTRACT class
    virtual int getExtraHourRate()=0;   // (we can never make a plain Vehicle object).

    int calculateFee(int hours);                    // facilitator: one formula for every vehicle
    virtual int getChargingFee(int parkedHours);    // 0 for normal vehicles, ElectricCar changes it
    virtual int getChargingHours();                 // 0 for normal vehicles
    virtual void stopCharging();                    // does nothing for normal vehicles

    static string cleanNumber(string number);       // "pb02 ab 1234" becomes "PB02AB1234"
};

int Vehicle::showMessages = 0;          // static data is created once, outside the class


Vehicle::Vehicle(string number){
    setNumber(number);
    if (showMessages==1){
        cout<<"  [Constructor] Vehicle      ("<<vehicleNumber<<")"<<endl;
    }
}

Vehicle::~Vehicle(){
    if (showMessages==1){
        cout<<"  [Destructor]  Vehicle      ("<<vehicleNumber<<")"<<endl;
    }
}

void Vehicle::setNumber(string number){
    string clean = cleanNumber(number);
    if (clean.length()>0){
        vehicleNumber = clean;
    }
    else {
        vehicleNumber = "UNKNOWN";      // empty number plate is not allowed
    }
}

string Vehicle::getNumber(){
    return vehicleNumber;
}

int Vehicle::calculateFee(int hours){
    if (hours<1){
        hours = 1;                      // minimum 1 hour
    }
    // getFirstHourRate() and getExtraHourRate() are virtual,
    // so the CHILD's rates are used here automatically.
    return getFirstHourRate()+(hours-1)*getExtraHourRate();
}

int Vehicle::getChargingFee(int parkedHours){
    return 0;                           // normal vehicles do not charge
}

int Vehicle::getChargingHours(){
    return 0;
}

void Vehicle::stopCharging(){
    // nothing to stop for normal vehicles
}

string Vehicle::cleanNumber(string number){
    string clean = "";
    for (int i=0;i<number.length();i++){
        char c = number[i];
        if (c!=' '){                            // skip spaces
            clean = clean+(char)toupper(c);     // make capital letters
        }
    }
    return clean;
}


// ------------------------------------------------------------
//  Bike : child of Vehicle
// ------------------------------------------------------------
class Bike : public Vehicle
{
    public:
    Bike(string number="");             // constructor
    ~Bike();                            // destructor
    string getType();                   // accessors (they replace the parent's pure virtual ones)
    string getSlotType();
    int getFirstHourRate();
    int getExtraHourRate();
};

Bike::Bike(string number) : Vehicle(number){        // ": Vehicle(number)" runs the parent's constructor first
    if (showMessages==1){
        cout<<"  [Constructor] Bike         ("<<vehicleNumber<<")"<<endl;
    }
}
Bike::~Bike(){
    if (showMessages==1){
        cout<<"  [Destructor]  Bike         ("<<vehicleNumber<<")"<<endl;
    }
}
string Bike::getType(){
    return "Bike";
}
string Bike::getSlotType(){
    return "BIKE";
}
int Bike::getFirstHourRate(){
    return 20;
}
int Bike::getExtraHourRate(){
    return 10;
}


// ------------------------------------------------------------
//  Car : child of Vehicle
// ------------------------------------------------------------
class Car : public Vehicle
{
    public:
    Car(string number="");
    ~Car();
    string getType();
    string getSlotType();
    int getFirstHourRate();
    int getExtraHourRate();
};

Car::Car(string number) : Vehicle(number){
    if (showMessages==1){
        cout<<"  [Constructor] Car          ("<<vehicleNumber<<")"<<endl;
    }
}
Car::~Car(){
    if (showMessages==1){
        cout<<"  [Destructor]  Car          ("<<vehicleNumber<<")"<<endl;
    }
}
string Car::getType(){
    return "Car";
}
string Car::getSlotType(){
    return "CAR";
}
int Car::getFirstHourRate(){
    return 50;
}
int Car::getExtraHourRate(){
    return 30;
}


// ------------------------------------------------------------
//  Truck : child of Vehicle
// ------------------------------------------------------------
class Truck : public Vehicle
{
    public:
    Truck(string number="");
    ~Truck();
    string getType();
    string getSlotType();
    int getFirstHourRate();
    int getExtraHourRate();
};

Truck::Truck(string number) : Vehicle(number){
    if (showMessages==1){
        cout<<"  [Constructor] Truck        ("<<vehicleNumber<<")"<<endl;
    }
}
Truck::~Truck(){
    if (showMessages==1){
        cout<<"  [Destructor]  Truck        ("<<vehicleNumber<<")"<<endl;
    }
}
string Truck::getType(){
    return "Truck";
}
string Truck::getSlotType(){
    return "TRUCK";
}
int Truck::getFirstHourRate(){
    return 100;
}
int Truck::getExtraHourRate(){
    return 60;
}


// ------------------------------------------------------------
//  ElectricCar : child of Car  (Vehicle -> Car -> ElectricCar)
//  It gets the Car's parking rates for free and adds charging.
// ------------------------------------------------------------
class ElectricCar : public Car
{
    private:
    int chargingHours;                  // hours the owner wants to charge

    public:
    ElectricCar(string number="",int hours=0);      // constructor with default arguments
    ~ElectricCar();
    void setChargingHours(int hours);               // mutator
    int getChargingHours();                         // accessor
    string getType();
    string getSlotType();
    int getChargingFee(int parkedHours);            // replaces the parent's version (which gives 0)
    void stopCharging();
};

ElectricCar::ElectricCar(string number,int hours) : Car(number){   // Car's constructor runs first
    setChargingHours(hours);
    if (showMessages==1){
        cout<<"  [Constructor] ElectricCar  ("<<vehicleNumber<<")"<<endl;
    }
}
ElectricCar::~ElectricCar(){
    if (showMessages==1){
        cout<<"  [Destructor]  ElectricCar  ("<<vehicleNumber<<")"<<endl;
    }
}
void ElectricCar::setChargingHours(int hours){
    if (hours<0){
        chargingHours = 0;
    }
    else if (hours>12){
        chargingHours = 12;             // a charger session is at most 12 hours
    }
    else {
        chargingHours = hours;
    }
}
int ElectricCar::getChargingHours(){
    return chargingHours;
}
string ElectricCar::getType(){
    return "Electric Car";
}
string ElectricCar::getSlotType(){
    return "EV";
}
int ElectricCar::getChargingFee(int parkedHours){
    int hours = chargingHours;
    if (hours>parkedHours){
        hours = parkedHours;            // cannot charge longer than the car was parked
    }
    return hours*20;                    // Rs 20 per charging hour
}
void ElectricCar::stopCharging(){
    chargingHours = 0;                  // used when no EV slot is free
}

#endif
