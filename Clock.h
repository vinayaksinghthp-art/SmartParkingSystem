// ============================================================
//  Clock.h
//  A small "toolbox" class for everything related to time.
//  Every function is STATIC, so we never make a Clock object.
//  We just call them like this:   Clock::now()
// ============================================================
#ifndef CLOCK_H
#define CLOCK_H

#include <bits/stdc++.h>
using namespace std;

class Clock
{
    public:
    static time_t now();                                // facilitators
    static string twoDigits(int n);
    static string fullText(time_t t);                   // "09-Nov-2026 10:30"
    static string shortText(time_t t);                  // "09-Nov 10:30"
    static string refDatePart(time_t t);                // "1109"  (month + day)
    static int hourOf(time_t t);                        // 0 to 23
    static int hoursBetween(time_t start,time_t end);   // rounded UP, minimum 1
};


time_t Clock::now(){
    return time(0);                     // the computer's clock reading, in seconds
}

string Clock::twoDigits(int n){
    if (n<10){
        return "0"+to_string(n);        // 5 becomes "05"
    }
    return to_string(n);
}

string Clock::fullText(time_t t){
    string months[12] = {"Jan","Feb","Mar","Apr","May","Jun",
                         "Jul","Aug","Sep","Oct","Nov","Dec"};
    tm *parts = localtime(&t);          // break the clock reading into day, month, hour...
    string text = twoDigits(parts->tm_mday)+"-"+months[parts->tm_mon]+"-"+to_string(parts->tm_year+1900);
    text = text+" "+twoDigits(parts->tm_hour)+":"+twoDigits(parts->tm_min);
    return text;
}

string Clock::shortText(time_t t){
    string months[12] = {"Jan","Feb","Mar","Apr","May","Jun",
                         "Jul","Aug","Sep","Oct","Nov","Dec"};
    tm *parts = localtime(&t);
    string text = twoDigits(parts->tm_mday)+"-"+months[parts->tm_mon];
    text = text+" "+twoDigits(parts->tm_hour)+":"+twoDigits(parts->tm_min);
    return text;
}

string Clock::refDatePart(time_t t){
    tm *parts = localtime(&t);
    return twoDigits(parts->tm_mon+1)+twoDigits(parts->tm_mday);     // tm_mon counts from 0, so +1
}

int Clock::hourOf(time_t t){
    tm *parts = localtime(&t);
    return parts->tm_hour;
}

int Clock::hoursBetween(time_t start,time_t end){
    int seconds = (int)(end-start);     // how long the vehicle stayed
    if (seconds<0){
        seconds = 0;
    }
    int hours = (seconds+3599)/3600;    // adding 3599 rounds any extra minutes UP
    if (hours==0){
        hours = 1;                      // minimum charge is 1 hour
    }
    return hours;
}

#endif
