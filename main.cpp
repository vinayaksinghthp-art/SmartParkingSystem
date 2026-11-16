// ============================================================
//  Smart Multi-Level Parking Management System with EV Support
//  OOP Project (UNC302) - C++
//
//  main.cpp : the menus. All the real work is done by the classes
//             in the .h files.
//
//  Compile :  g++ main.cpp -o parking
//  Run     :  parking        (Windows)
//             ./parking      (Linux / Mac)
// ============================================================
#include <bits/stdc++.h>
#include "ParkingLot.h"
using namespace std;

// ---------- small helper functions for reading input safely ----------
string readLine(string prompt);
int readNumber(string prompt,int low,int high);
int readYesNo(string prompt);
void waitForEnter();
void printHeader(string title);

// ---------- one function for each menu option ----------
void parkMenu(ParkingLot &lot);
void exitMenu(ParkingLot &lot);
void findMenu(ParkingLot &lot);
void adminMenu(ParkingLot &lot,Admin &admin);


int main(){
    ParkingLot lot;             // the constructor builds all 35 slots
    Admin admin;                // password: admin123
    int choice = -1;

    while (choice!=0){
        printHeader("SMART MULTI-LEVEL PARKING SYSTEM");
        cout<<"   1. Park a vehicle"<<endl;
        cout<<"   2. Exit a vehicle"<<endl;
        cout<<"   3. Find my vehicle"<<endl;
        cout<<"   4. Show free slots"<<endl;
        cout<<"   5. Admin login"<<endl;
        cout<<"   0. Close program"<<endl;
        choice = readNumber("  Enter your choice: ",0,5);

        if (choice==1){
            parkMenu(lot);
        }
        else if (choice==2){
            exitMenu(lot);
        }
        else if (choice==3){
            findMenu(lot);
        }
        else if (choice==4){
            lot.showFreeSlots();
            waitForEnter();
        }
        else if (choice==5){
            adminMenu(lot,admin);
        }
    }
    cout<<"  Closing the program. Goodbye!"<<endl;
    return 0;                   // lot's destructor runs here and cleans up
}


// ============================================================
//  1. Park a vehicle
// ============================================================
void parkMenu(ParkingLot &lot){
    printHeader("PARK A VEHICLE");
    cout<<"   1. Bike   2. Car   3. Truck   4. Electric Car"<<endl;
    int type = readNumber("  Choose vehicle type: ",1,4);
    string number = readLine("  Enter number plate (e.g. PB02AB1234): ");

    int wantsReserved = 0;
    int chargeHours = 0;
    if (type==2){
        wantsReserved = readYesNo("  Need a reserved slot (disabled driver / VIP)?");
    }
    if (type==4){
        chargeHours = readNumber("  How many hours do you want to charge? (0-12): ",0,12);
    }

    string name = readLine("  Enter owner name: ");
    string phone = readLine("  Enter 10-digit mobile number (for the SMS ticket): ");
    while (Customer::isValidPhone(phone)==0 && phone!="0"){
        cout<<"  That is not a valid 10-digit mobile number."<<endl;
        phone = readLine("  Enter again (or type 0 to skip the SMS): ");
    }

    // ----- make the Customer object (Customer or PassMember) -----
    Customer *c = nullptr;
    if (readYesNo("  Do you have a monthly pass?")==1){
        string passId = readLine("  Enter pass ID (e.g. PASS-017): ");
        if (PassMember::isValidPassId(passId)==1){
            c = new PassMember(name,phone,passId);
            cout<<"  Pass accepted: 20% off on parking."<<endl;
        }
        else {
            cout<<"  Invalid pass ID. You will be charged as a regular customer."<<endl;
            c = new Customer(name,phone);
        }
    }
    else {
        c = new Customer(name,phone);
    }

    // ----- make the Vehicle object -----
    Vehicle *v = nullptr;       // a parent pointer can hold ANY child object
    if (type==1){
        v = new Bike(number);
    }
    else if (type==2){
        v = new Car(number);
    }
    else if (type==3){
        v = new Truck(number);
    }
    else {
        v = new ElectricCar(number,chargeHours);
    }

    lot.parkVehicle(v,c,wantsReserved);         // finds a slot, prints the ticket, sends the SMS
    waitForEnter();
}


// ============================================================
//  2. Exit a vehicle
// ============================================================
void exitMenu(ParkingLot &lot){
    printHeader("EXIT A VEHICLE");
    if (lot.countParked()==0){
        cout<<"  No vehicles are parked right now."<<endl;
        waitForEnter();
        return;
    }
    cout<<"   1. I have my reference number"<<endl;
    cout<<"   2. I lost it (Rs 200 fine)"<<endl;
    int option = readNumber("  Choose: ",1,2);

    Ticket *t = nullptr;
    int lost = 0;
    if (option==1){
        string ref = readLine("  Enter reference number (e.g. PK-1109-101): ");
        int ticketNumber = lot.refToTicketNumber(ref);
        t = lot.findTicket(ticketNumber);               // overloaded: int version
    }
    else {
        string number = readLine("  Enter your number plate: ");
        t = lot.findTicket(number);                     // overloaded: string version
        lost = 1;
    }

    if (t==nullptr){
        cout<<"  No parked vehicle found with that detail."<<endl;
        waitForEnter();
        return;
    }
    lot.exitVehicle(t,lost);
    waitForEnter();
}


// ============================================================
//  3. Find my vehicle
// ============================================================
void findMenu(ParkingLot &lot){
    printHeader("FIND MY VEHICLE");
    string number = readLine("  Enter your number plate: ");
    Ticket *t = lot.findTicket(number);
    if (t==nullptr){
        cout<<"  This vehicle is not parked here."<<endl;
    }
    else {
        cout<<"  Found! "<<t->getVehicle()->getNumber()<<" is at slot "<<t->getSlot()->getSlotId();
        cout<<", parked since "<<Clock::shortText(t->getEntryTime())<<"."<<endl;
        cout<<"  Reference number: "<<t->getRefNo()<<endl;
    }
    waitForEnter();
}


// ============================================================
//  5. Admin menu
// ============================================================
void adminMenu(ParkingLot &lot,Admin &admin){
    printHeader("ADMIN LOGIN");
    string typed = readLine("  Enter admin password: ");
    if (admin.login(typed)==0){
        cout<<"  Wrong password!"<<endl;
        waitForEnter();
        return;
    }

    int choice = -1;
    while (choice!=0){
        printHeader("ADMIN MENU");
        cout<<"   1. Today's report"<<endl;
        cout<<"   2. See all parked vehicles"<<endl;
        cout<<"   3. SMS Test Mode        (now: "<<(SmsSender::getTestMode()==1 ? "ON" : "OFF")<<")"<<endl;
        cout<<"   4. Object life messages (now: "<<(Vehicle::showMessages==1 ? "ON" : "OFF")<<")"<<endl;
        cout<<"   5. Change password"<<endl;
        cout<<"   0. Logout"<<endl;
        choice = readNumber("  Enter your choice: ",0,5);

        if (choice==1){
            Report::showReport();
            lot.showFullestFloor();
            waitForEnter();
        }
        else if (choice==2){
            lot.showParkedVehicles();
            waitForEnter();
        }
        else if (choice==3){
            if (SmsSender::getTestMode()==1){
                SmsSender::setTestMode(0);
                cout<<"  Test Mode is now OFF. Real SMS will be sent (each SMS costs money)."<<endl;
            }
            else {
                SmsSender::setTestMode(1);
                cout<<"  Test Mode is now ON. SMS will only be shown on the screen."<<endl;
            }
            waitForEnter();
        }
        else if (choice==4){
            Vehicle::showMessages = 1-Vehicle::showMessages;    // 1 becomes 0, 0 becomes 1
            if (Vehicle::showMessages==1){
                cout<<"  ON: you will now see every constructor and destructor run."<<endl;
            }
            else {
                cout<<"  OFF: constructor and destructor messages are hidden."<<endl;
            }
            waitForEnter();
        }
        else if (choice==5){
            string oldPass = readLine("  Enter old password: ");
            string newPass = readLine("  Enter new password (at least 4 characters): ");
            if (admin.changePassword(oldPass,newPass)==1){
                cout<<"  Password changed."<<endl;
            }
            else {
                cout<<"  Could not change password (old password wrong or new one too short)."<<endl;
            }
            waitForEnter();
        }
    }
}


// ============================================================
//  Helper functions
// ============================================================
string readLine(string prompt){
    cout<<prompt;
    string text;
    if (!getline(cin,text)){        // input has ended (for example Ctrl+Z), so close safely
        cout<<endl<<"  Input ended. Closing the program."<<endl;
        exit(0);
    }
    return text;
}

int readNumber(string prompt,int low,int high){
    while (true){
        string text = readLine(prompt);
        int ok = 1;
        if (text.length()==0 || text.length()>4){
            ok = 0;
        }
        for (int i=0;i<text.length();i++){
            if (text[i]<'0' || text[i]>'9'){
                ok = 0;                     // letters or symbols are not allowed
            }
        }
        if (ok==1){
            int n = stoi(text);             // safe now: only digits, at most 4 of them
            if (n>=low && n<=high){
                return n;
            }
        }
        cout<<"  Please enter a number from "<<low<<" to "<<high<<"."<<endl;
    }
}

int readYesNo(string prompt){
    while (true){
        string text = readLine(prompt+" (y/n): ");
        if (text=="y" || text=="Y"){
            return 1;
        }
        if (text=="n" || text=="N"){
            return 0;
        }
        cout<<"  Please type y or n."<<endl;
    }
}

void waitForEnter(){
    readLine("  Press Enter to continue...");
}

void printHeader(string title){
    cout<<endl;
    cout<<"  ========================================"<<endl;
    cout<<"   "<<title<<endl;
    cout<<"  ========================================"<<endl;
}
