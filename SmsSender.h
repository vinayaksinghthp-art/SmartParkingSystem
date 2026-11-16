// ============================================================
//  SmsSender.h
//  The messenger. Everything is STATIC, so we never make an
//  SmsSender object. We just call:  SmsSender::send(phone,text)
//
//  Test Mode ON  (1) : the SMS is only printed on the screen (free)
//  Test Mode OFF (0) : the real SMS is sent through send_sms.py
// ============================================================
#ifndef SMSSENDER_H
#define SMSSENDER_H

#include <bits/stdc++.h>
using namespace std;

class SmsSender
{
    private:
    static int testMode;                // 1 = only print, 0 = really send
    static string pythonCommand;        // "python" (or "py" on some Windows PCs)
    static string lastStatus;           // what happened to the last SMS (used by the website)

    public:
    static void setTestMode(int mode);          // mutators
    static void setPythonCommand(string cmd);
    static int getTestMode();                   // accessors
    static string getLastStatus();              // "TEST", "SENT", "FAILED" or "SKIPPED"
    static int send(string phone,string message);   // facilitator: 1 = sent, 0 = not sent
};

int SmsSender::testMode = 1;                // starts in Test Mode, so nothing costs money
string SmsSender::pythonCommand = "python"; // change to "py" if "python" does not work on your PC
string SmsSender::lastStatus = "NONE";


void SmsSender::setTestMode(int mode){
    if (mode==0){
        testMode = 0;
    }
    else {
        testMode = 1;
    }
}

void SmsSender::setPythonCommand(string cmd){
    pythonCommand = cmd;
}

int SmsSender::getTestMode(){
    return testMode;
}

string SmsSender::getLastStatus(){
    return lastStatus;
}

int SmsSender::send(string phone,string message){
    if (phone=="0000000000"){
        cout<<"  (No valid phone number, SMS skipped.)"<<endl;
        lastStatus = "SKIPPED";
        return 0;
    }

    // ---------- Test Mode: just show the SMS on the screen ----------
    if (testMode==1){
        cout<<endl<<"  ----- [TEST MODE] SMS to "<<phone<<" -----"<<endl<<"   ";
        for (int i=0;i<message.length();i++){
            if (message[i]=='\n'){
                cout<<endl<<"   ";          // new line inside the SMS
            }
            else {
                cout<<message[i];
            }
        }
        cout<<endl<<"  -------------------------------------------"<<endl;
        lastStatus = "TEST";
        return 1;
    }

    // ---------- Real Mode: write the text into a file, then run Python ----------
    ofstream file("last_sms.txt");
    if (!file){
        cout<<"  Could not create last_sms.txt, SMS not sent."<<endl;
        lastStatus = "FAILED";
        return 0;
    }
    file<<message;
    file.close();

    cout<<"  Sending SMS to "<<phone<<" ..."<<endl;
    string command = pythonCommand+" send_sms.py "+phone;
    int result = system(command.c_str());     // runs the command, like typing it in Command Prompt

    if (result==0){
        cout<<"  SMS ticket sent successfully."<<endl;
        lastStatus = "SENT";
        return 1;
    }
    cout<<"  SMS could not be sent. (The parking is still saved.)"<<endl;
    lastStatus = "FAILED";
    return 0;
}

#endif
