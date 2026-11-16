// ============================================================
//  ParkingLot.h
//  The boss class: the whole building.
//  It has 3 floors and keeps a list of open tickets.
//
//     Ground (G) : 5 bike, 5 car (2 reserved), 3 EV charging
//     Floor 1    : 5 bike, 8 car
//     Floor 2    : 6 car, 3 truck
// ============================================================
#ifndef PARKINGLOT_H
#define PARKINGLOT_H

#include <bits/stdc++.h>
#include "Clock.h"
#include "Vehicle.h"
#include "Customer.h"
#include "Floor.h"
#include "Ticket.h"
#include "SmsSender.h"
using namespace std;

class ParkingLot
{
    private:
    Floor floors[3];                    // always exactly 3 floors, so a normal array
    vector<Ticket*> tickets;            // vehicles inside right now (grows and shrinks)
    Slot* searchAllFloors(string type,int needReserved);   // helper used by parkVehicle

    public:
    ParkingLot();                       // constructor
    ~ParkingLot();                      // destructor
    void setupBuilding();
    Ticket* parkVehicle(Vehicle *v,Customer *c,int wantsReserved=0);   // default argument
    Bill exitVehicle(Ticket *t,int lostTicket=0);   // also gives back the bill (used by the website)
    Ticket* findTicket(string vehicleNumber);       // function overloading:
    Ticket* findTicket(int ticketNumber);           // same name, different input
    int refToTicketNumber(string ref);
    int countParked();
    Floor* getFloor(int i);                         // accessors (used by the website)
    Ticket* getTicket(int i);
    void showFreeSlots();
    void showParkedVehicles();
    void showFullestFloor();
};


ParkingLot::ParkingLot(){
    setupBuilding();
}

ParkingLot::~ParkingLot(){
    for (int i=0;i<tickets.size();i++){
        delete tickets[i];              // clean up vehicles still inside when the program closes
    }
    tickets.clear();
}

void ParkingLot::setupBuilding(){
    // ----- Ground floor -----
    floors[0].setName("G");
    for (int i=1;i<=5;i++){
        floors[0].addSlot("G-B"+to_string(i),"BIKE");
    }
    floors[0].addSlot("G-C1","CAR",1);          // reserved
    floors[0].addSlot("G-C2","CAR",1);          // reserved
    for (int i=3;i<=5;i++){
        floors[0].addSlot("G-C"+to_string(i),"CAR");
    }
    for (int i=1;i<=3;i++){
        floors[0].addSlot("G-E"+to_string(i),"EV");
    }

    // ----- Floor 1 -----
    floors[1].setName("1");
    for (int i=1;i<=5;i++){
        floors[1].addSlot("1-B"+to_string(i),"BIKE");
    }
    for (int i=1;i<=8;i++){
        floors[1].addSlot("1-C"+to_string(i),"CAR");
    }

    // ----- Floor 2 -----
    floors[2].setName("2");
    for (int i=1;i<=6;i++){
        floors[2].addSlot("2-C"+to_string(i),"CAR");
    }
    for (int i=1;i<=3;i++){
        floors[2].addSlot("2-T"+to_string(i),"TRUCK");
    }
}

Slot* ParkingLot::searchAllFloors(string type,int needReserved){
    for (int i=0;i<3;i++){
        if (floors[i].countTotal(type)>0){              // this floor has this kind of slot
            Slot *s = floors[i].findFreeSlot(type,needReserved);
            if (s!=nullptr){
                return s;
            }
            if (needReserved==0){
                cout<<"  Floor "<<floors[i].getName()<<" is full for "<<type<<", checking the next floor..."<<endl;
            }
        }
    }
    return nullptr;
}

Ticket* ParkingLot::parkVehicle(Vehicle *v,Customer *c,int wantsReserved){
    // 1. The same vehicle cannot be inside twice
    if (findTicket(v->getNumber())!=nullptr){
        cout<<"  This vehicle ("<<v->getNumber()<<") is already parked inside!"<<endl;
        delete v;
        delete c;
        return nullptr;
    }

    // 2. Find a slot
    string type = v->getSlotType();             // virtual: "BIKE", "CAR", "EV" or "TRUCK"
    Slot *s = nullptr;
    if (wantsReserved==1){
        s = searchAllFloors(type,1);
        if (s==nullptr){
            cout<<"  No reserved slot is free, looking for a normal slot..."<<endl;
        }
    }
    if (s==nullptr){
        s = searchAllFloors(type,0);
    }
    if (s==nullptr && type=="EV"){
        cout<<"  All EV charging slots are full. Parking in a normal car slot (no charging)."<<endl;
        s = searchAllFloors("CAR",0);
        if (s!=nullptr){
            v->stopCharging();
        }
    }
    if (s==nullptr){
        cout<<"  Sorry! Parking is FULL for "<<v->getType()<<"."<<endl;
        delete v;
        delete c;
        return nullptr;
    }

    // 3. Park it and make the ticket
    s->parkHere(v);
    Ticket *t = new Ticket(v,c,s);
    tickets.push_back(t);                       // add to the list of vehicles inside

    // 4. Show the ticket and send the SMS automatically
    t->print();
    SmsSender::send(c->getPhone(),t->makeSmsText());
    return t;
}

Bill ParkingLot::exitVehicle(Ticket *t,int lostTicket){
    time_t exitTime = Clock::now();

    Bill bill(t,exitTime);
    if (lostTicket==1){
        bill = bill+200;                        // operator overloading: adds the Rs 200 fine
    }
    bill.print();

    Report::addEntry(t->getVehicle()->getSlotType(),bill.getTotal(),bill.getFine());
    t->getSlot()->makeFree();

    for (int i=0;i<tickets.size();i++){         // remove the ticket from the list
        if (tickets[i]==t){
            tickets.erase(tickets.begin()+i);
            break;
        }
    }
    delete t;                                   // the destructor also deletes the vehicle and customer
    cout<<"  The slot is now free. Thank you, visit again!"<<endl;
    return bill;
}

Ticket* ParkingLot::findTicket(string vehicleNumber){
    string clean = Vehicle::cleanNumber(vehicleNumber);
    for (int i=0;i<tickets.size();i++){
        if (tickets[i]->getVehicle()->getNumber()==clean){
            return tickets[i];
        }
    }
    return nullptr;
}

Ticket* ParkingLot::findTicket(int ticketNumber){
    for (int i=0;i<tickets.size();i++){
        if (tickets[i]->getTicketNumber()==ticketNumber){
            return tickets[i];
        }
    }
    return nullptr;
}

int ParkingLot::refToTicketNumber(string ref){
    // Reads the digits at the END of the reference number.
    // "PK-1109-104" gives 104.  "104" also gives 104.
    int number = 0;
    int place = 1;
    int i = ref.length()-1;
    while (i>=0 && ref[i]==' '){
        i--;                                    // skip spaces at the end
    }
    while (i>=0 && ref[i]>='0' && ref[i]<='9' && place<=100000){
        number = number+(ref[i]-'0')*place;
        place = place*10;
        i--;
    }
    if (place==1){
        return -1;                              // no digits found
    }
    return number;
}

int ParkingLot::countParked(){
    return tickets.size();
}

Floor* ParkingLot::getFloor(int i){
    if (i<0 || i>=3){
        return nullptr;
    }
    return &floors[i];
}

Ticket* ParkingLot::getTicket(int i){
    if (i<0 || i>=tickets.size()){
        return nullptr;
    }
    return tickets[i];
}

void ParkingLot::showFreeSlots(){
    cout<<endl<<"  ============== FREE SLOTS =============="<<endl;
    for (int i=0;i<3;i++){
        floors[i].showStatus();
    }
    cout<<"  Vehicles inside right now : "<<countParked()<<endl;
    cout<<"  ========================================"<<endl;
}

void ParkingLot::showParkedVehicles(){
    cout<<endl<<"  ======== VEHICLES INSIDE NOW ========"<<endl;
    if (tickets.size()==0){
        cout<<"   No vehicles are parked right now."<<endl;
    }
    for (int i=0;i<tickets.size();i++){
        Ticket *t = tickets[i];
        cout<<"   "<<i+1<<". "<<t->getRefNo()<<" | "<<t->getVehicle()->getNumber();
        cout<<" ("<<t->getVehicle()->getType()<<") | Slot "<<t->getSlot()->getSlotId();
        cout<<" | Since "<<Clock::shortText(t->getEntryTime())<<endl;
    }
    cout<<"  ====================================="<<endl;
}

void ParkingLot::showFullestFloor(){
    int best = -1;
    int bestPercent = -1;
    for (int i=0;i<3;i++){
        int percent = floors[i].countParked()*100/floors[i].getSlotCount();
        if (percent>bestPercent){
            bestPercent = percent;
            best = i;
        }
    }
    if (bestPercent==0){
        cout<<"   Fullest floor     : all floors are empty"<<endl;
        return;
    }
    cout<<"   Fullest floor     : Floor "<<floors[best].getName()<<" (";
    cout<<floors[best].countParked()<<" of "<<floors[best].getSlotCount()<<" slots used, "<<bestPercent<<"%)"<<endl;
}

#endif
