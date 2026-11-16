// ============================================================
//  Ticket.h
//  The paperwork:
//     Ticket : made when a vehicle parks (gives the reference number)
//     Bill   : made when a vehicle leaves (works out the money)
//     Report : the daily summary for the admin (everything static)
// ============================================================
#ifndef TICKET_H
#define TICKET_H

#include <bits/stdc++.h>
#include "Clock.h"
#include "Vehicle.h"
#include "Customer.h"
#include "Floor.h"
using namespace std;

// ------------------------------------------------------------
//  Ticket
// ------------------------------------------------------------
class Ticket
{
    private:
    static int ticketCounter;       // ONE shared counter for all tickets
    int ticketNumber;
    string refNo;                   // like "PK-1109-101"
    Vehicle *vehicle;
    Customer *customer;
    Slot *slot;
    time_t entryTime;               // clock reading when the vehicle parked

    public:
    Ticket(Vehicle *v,Customer *c,Slot *s);     // constructor
    ~Ticket();                                  // destructor
    void setEntryTime(time_t t);                // mutator (used only by the test programs)
    int getTicketNumber();                      // accessors
    string getRefNo();
    Vehicle* getVehicle();
    Customer* getCustomer();
    Slot* getSlot();
    time_t getEntryTime();
    string makeSmsText();                       // facilitators
    void print();
    static int getTicketCounter();              // static function: no object needed
};

int Ticket::ticketCounter = 100;    // the first ticket will be 101


Ticket::Ticket(Vehicle *v,Customer *c,Slot *s){
    ticketCounter++;                    // shared counter goes up by 1
    ticketNumber = ticketCounter;
    vehicle = v;
    customer = c;
    slot = s;
    entryTime = Clock::now();           // read the computer's clock
    refNo = "PK-"+Clock::refDatePart(entryTime)+"-"+to_string(ticketNumber);
    if (Vehicle::showMessages==1){
        cout<<"  [Constructor] Ticket       ("<<refNo<<")"<<endl;
    }
}

Ticket::~Ticket(){
    if (Vehicle::showMessages==1){
        cout<<"  [Destructor]  Ticket       ("<<refNo<<")"<<endl;
    }
    delete vehicle;                     // both were made with new, so the ticket deletes them
    delete customer;
}

void Ticket::setEntryTime(time_t t){
    entryTime = t;
}

int Ticket::getTicketNumber(){
    return ticketNumber;
}
string Ticket::getRefNo(){
    return refNo;
}
Vehicle* Ticket::getVehicle(){
    return vehicle;
}
Customer* Ticket::getCustomer(){
    return customer;
}
Slot* Ticket::getSlot(){
    return slot;
}
time_t Ticket::getEntryTime(){
    return entryTime;
}

string Ticket::makeSmsText(){
    // Kept short (under 160 characters) so it goes as ONE SMS.
    string text = "SmartPark Ticket\n";
    text = text+"Ref: "+refNo+"\n";
    text = text+"Vehicle: "+vehicle->getNumber()+" ("+vehicle->getType()+")\n";
    text = text+"Slot: "+slot->getSlotId()+"\n";
    text = text+"In: "+Clock::shortText(entryTime)+"\n";
    text = text+"Show Ref at exit.";
    return text;
}

void Ticket::print(){
    cout<<endl;
    cout<<"  ============ PARKING TICKET ============"<<endl;
    cout<<"   Ref No   : "<<refNo<<endl;
    cout<<"   Vehicle  : "<<vehicle->getNumber()<<" ("<<vehicle->getType()<<")"<<endl;
    cout<<"   Owner    : "<<customer->getName()<<" ("<<customer->getMemberType()<<")"<<endl;
    cout<<"   Phone    : "<<customer->getMaskedPhone()<<endl;
    cout<<"   Slot     : "<<slot->getSlotId()<<endl;
    cout<<"   Entry    : "<<Clock::fullText(entryTime)<<endl;
    if (vehicle->getChargingHours()>0){
        cout<<"   Charging : "<<vehicle->getChargingHours()<<" hour(s) booked"<<endl;
    }
    cout<<"  ========================================"<<endl;
}

int Ticket::getTicketCounter(){
    return ticketCounter;
}


// ------------------------------------------------------------
//  Bill : the money at exit
//  Total = Parking fee + Charging fee + Night charge - Discount + Fine
// ------------------------------------------------------------
class Bill
{
    private:
    string refNo;
    string vehicleText;
    string entryText;
    string exitText;
    int hours;
    int parkingFee;
    int chargingFee;
    int nightCharge;
    int discount;
    int fine;
    int total;

    public:
    Bill();                                 // non-parameterized constructor
    Bill(Ticket *t,time_t exitTime);        // parameterized constructor: does all the maths
    Bill operator+(int fineAmount);         // operator overloading:  bill = bill + 200;
    string getRefNo();                      // accessors
    string getVehicleText();
    string getEntryText();
    string getExitText();
    int getHours();
    int getParkingFee();
    int getChargingFee();
    int getNightCharge();
    int getDiscount();
    int getFine();
    int getTotal();
    void print();                           // facilitator
};


Bill::Bill(){
    refNo = "";
    vehicleText = "";
    entryText = "";
    exitText = "";
    hours = 0;
    parkingFee = 0;
    chargingFee = 0;
    nightCharge = 0;
    discount = 0;
    fine = 0;
    total = 0;
}

Bill::Bill(Ticket *t,time_t exitTime){
    Vehicle *v = t->getVehicle();
    Customer *c = t->getCustomer();

    refNo = t->getRefNo();
    vehicleText = v->getNumber()+" ("+v->getType()+")";
    entryText = Clock::shortText(t->getEntryTime());
    exitText = Clock::shortText(exitTime);

    hours = Clock::hoursBetween(t->getEntryTime(),exitTime);
    parkingFee = v->calculateFee(hours);            // virtual: right rates for Bike / Car / Truck / EV
    chargingFee = v->getChargingFee(hours);         // virtual: 0 unless it is an ElectricCar

    int exitHour = Clock::hourOf(exitTime);
    if (exitHour>=22 || exitHour<6){
        nightCharge = 20;                           // leaving between 10 PM and 6 AM
    }
    else {
        nightCharge = 0;
    }

    discount = parkingFee*c->getDiscountPercent()/100;     // virtual: 20% for PassMember, 0% otherwise
    fine = 0;
    total = parkingFee+chargingFee+nightCharge-discount+fine;
}

Bill Bill::operator+(int fineAmount){
    Bill temp;
    temp = *this;                       // copy every value of this bill into temp
    temp.fine = fine+fineAmount;
    temp.total = total+fineAmount;
    return temp;                        // the original bill is not changed
}

string Bill::getRefNo(){
    return refNo;
}
string Bill::getVehicleText(){
    return vehicleText;
}
string Bill::getEntryText(){
    return entryText;
}
string Bill::getExitText(){
    return exitText;
}
int Bill::getHours(){
    return hours;
}
int Bill::getParkingFee(){
    return parkingFee;
}
int Bill::getChargingFee(){
    return chargingFee;
}
int Bill::getNightCharge(){
    return nightCharge;
}
int Bill::getDiscount(){
    return discount;
}
int Bill::getFine(){
    return fine;
}
int Bill::getTotal(){
    return total;
}

void Bill::print(){
    cout<<endl;
    cout<<"  ================= BILL ================="<<endl;
    cout<<"   Ref No        : "<<refNo<<endl;
    cout<<"   Vehicle       : "<<vehicleText<<endl;
    cout<<"   Entry         : "<<entryText<<endl;
    cout<<"   Exit          : "<<exitText<<endl;
    cout<<"   Time charged  : "<<hours<<" hour(s)"<<endl;
    cout<<"  ----------------------------------------"<<endl;
    cout<<"   Parking fee   : Rs "<<parkingFee<<endl;
    if (chargingFee>0){
        cout<<"   Charging fee  : Rs "<<chargingFee<<endl;
    }
    if (nightCharge>0){
        cout<<"   Night charge  : Rs "<<nightCharge<<endl;
    }
    if (discount>0){
        cout<<"   Pass discount : -Rs "<<discount<<endl;
    }
    if (fine>0){
        cout<<"   Lost ticket   : Rs "<<fine<<endl;
    }
    cout<<"  ----------------------------------------"<<endl;
    cout<<"   TOTAL         : Rs "<<total<<endl;
    cout<<"  ========================================"<<endl;
}


// ------------------------------------------------------------
//  Report : the daily summary. Everything is STATIC,
//  so there is only ONE report for the whole program.
// ------------------------------------------------------------
class Report
{
    private:
    static int totalEarnings;
    static int finesCollected;
    static int bikeCount;
    static int carCount;
    static int truckCount;
    static int evCount;

    public:
    static void addEntry(string slotType,int amount,int fineAmount);   // called after every exit
    static int getTotalEarnings();
    static int getFinesCollected();
    static int getCount(string slotType);   // how many of one type have left today
    static int getVehiclesServed();
    static void showReport();
};

int Report::totalEarnings = 0;
int Report::finesCollected = 0;
int Report::bikeCount = 0;
int Report::carCount = 0;
int Report::truckCount = 0;
int Report::evCount = 0;


void Report::addEntry(string slotType,int amount,int fineAmount){
    totalEarnings = totalEarnings+amount;
    finesCollected = finesCollected+fineAmount;
    if (slotType=="BIKE"){
        bikeCount++;
    }
    else if (slotType=="CAR"){
        carCount++;
    }
    else if (slotType=="TRUCK"){
        truckCount++;
    }
    else if (slotType=="EV"){
        evCount++;
    }
}

int Report::getTotalEarnings(){
    return totalEarnings;
}

int Report::getFinesCollected(){
    return finesCollected;
}

int Report::getCount(string slotType){
    if (slotType=="BIKE"){
        return bikeCount;
    }
    if (slotType=="CAR"){
        return carCount;
    }
    if (slotType=="TRUCK"){
        return truckCount;
    }
    if (slotType=="EV"){
        return evCount;
    }
    return 0;
}

int Report::getVehiclesServed(){
    return bikeCount+carCount+truckCount+evCount;
}

void Report::showReport(){
    cout<<endl;
    cout<<"  ============ TODAY'S REPORT ============"<<endl;
    cout<<"   Total earnings    : Rs "<<totalEarnings<<endl;
    cout<<"   Fines collected   : Rs "<<finesCollected<<endl;
    cout<<"   Vehicles served   : "<<getVehiclesServed()<<endl;
    cout<<"      Bikes          : "<<bikeCount<<endl;
    cout<<"      Cars           : "<<carCount<<endl;
    cout<<"      Electric cars  : "<<evCount<<endl;
    cout<<"      Trucks         : "<<truckCount<<endl;
    cout<<"   Tickets issued    : "<<Ticket::getTicketCounter()-100<<endl;
    cout<<"  ========================================"<<endl;
}

#endif
