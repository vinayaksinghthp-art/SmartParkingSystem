// ============================================================
//  Floor.h
//  The building parts:
//     Slot  : one parking space
//     Floor : one floor, holding many slots (in a vector)
// ============================================================
#ifndef FLOOR_H
#define FLOOR_H

#include <bits/stdc++.h>
#include "Vehicle.h"
using namespace std;

// ------------------------------------------------------------
//  Slot : one parking space
// ------------------------------------------------------------
class Slot
{
    private:
    string slotId;              // like "G-E2"
    string slotType;            // "BIKE", "CAR", "EV" or "TRUCK"
    int isFree;                 // 1 = empty, 0 = taken
    int isReserved;             // 1 = only for disabled drivers / VIPs
    Vehicle *parkedVehicle;     // the vehicle standing here (nullptr when empty)

    public:
    Slot(string id="",string type="CAR",int reserved=0);   // constructor
    void parkHere(Vehicle *v);                              // mutators
    void makeFree();
    string getSlotId();                                     // accessors
    string getSlotType();
    int getIsFree();
    int getIsReserved();
    Vehicle* getVehicle();
};


Slot::Slot(string id,string type,int reserved){
    slotId = id;
    slotType = type;
    isFree = 1;
    isReserved = reserved;
    parkedVehicle = nullptr;
}

void Slot::parkHere(Vehicle *v){
    parkedVehicle = v;
    isFree = 0;
}

void Slot::makeFree(){
    parkedVehicle = nullptr;
    isFree = 1;
}

string Slot::getSlotId(){
    return slotId;
}
string Slot::getSlotType(){
    return slotType;
}
int Slot::getIsFree(){
    return isFree;
}
int Slot::getIsReserved(){
    return isReserved;
}
Vehicle* Slot::getVehicle(){
    return parkedVehicle;
}


// ------------------------------------------------------------
//  Floor : one floor with many slots
// ------------------------------------------------------------
class Floor
{
    private:
    string floorName;           // "G", "1" or "2"
    vector<Slot> slots;         // a list of slots that grows by itself

    public:
    Floor(string name="");                                  // constructor
    void setName(string name);                              // mutator
    string getName();                                       // accessor
    void addSlot(string id,string type,int reserved=0);     // used once, when the building is made
    Slot* findFreeSlot(string type,int needReserved);       // facilitators
    int countFree(string type);
    int countTotal(string type);
    int countParked();
    int getSlotCount();
    Slot* getSlot(int i);
    void showStatus();
};


Floor::Floor(string name){
    setName(name);
}

void Floor::setName(string name){
    floorName = name;
}

string Floor::getName(){
    return floorName;
}

void Floor::addSlot(string id,string type,int reserved){
    Slot s(id,type,reserved);
    slots.push_back(s);                 // add the new slot at the end of the list
}

Slot* Floor::findFreeSlot(string type,int needReserved){
    for (int i=0;i<slots.size();i++){
        if (slots[i].getSlotType()==type && slots[i].getIsFree()==1 && slots[i].getIsReserved()==needReserved){
            return &slots[i];           // give back the ADDRESS of the real slot
        }
    }
    return nullptr;                     // nothing free of this type
}

int Floor::countFree(string type){
    int count = 0;
    for (int i=0;i<slots.size();i++){
        if (slots[i].getSlotType()==type && slots[i].getIsFree()==1){
            count++;
        }
    }
    return count;
}

int Floor::countTotal(string type){
    int count = 0;
    for (int i=0;i<slots.size();i++){
        if (slots[i].getSlotType()==type){
            count++;
        }
    }
    return count;
}

int Floor::countParked(){
    int count = 0;
    for (int i=0;i<slots.size();i++){
        if (slots[i].getIsFree()==0){
            count++;
        }
    }
    return count;
}

int Floor::getSlotCount(){
    return slots.size();
}

Slot* Floor::getSlot(int i){
    if (i<0 || i>=slots.size()){
        return nullptr;
    }
    return &slots[i];
}

void Floor::showStatus(){
    string types[4] = {"BIKE","CAR","EV","TRUCK"};
    string names[4] = {"Bike","Car","EV","Truck"};
    cout<<"  Floor "<<floorName<<" : ";
    for (int i=0;i<4;i++){
        int total = countTotal(types[i]);
        if (total>0){                   // show only the types this floor has
            cout<<names[i]<<" "<<countFree(types[i])<<"/"<<total<<" free   ";
        }
    }
    cout<<endl;
}

#endif
