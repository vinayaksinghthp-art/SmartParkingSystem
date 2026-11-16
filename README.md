# Smart Multi-Level Parking Management System with EV Support

A C++ application that runs a 3-floor parking building, with **two front ends**: a console (cmd) version and a **website** version. It parks bikes, cars, trucks and electric cars, gives every vehicle a unique reference number, **sends the parking ticket to the customer's phone by SMS**, and calculates the bill at exit.

Built as an Object Oriented Programming (UNC302) project at Thapar Institute of Engineering & Technology.

**Student:** Vinayak Singh

![SmartPark website: live floor map of the three levels and the parking desk](screenshots/website.png)

<p align="center"><img src="screenshots/ticket_sms.png" width="360" alt="Parking ticket and SMS preview"></p>

---

## Features

- **3 floors, 35 slots**: Ground (5 bike, 5 car incl. 2 reserved, 3 EV charging), Floor 1 (5 bike, 8 car), Floor 2 (6 car, 3 truck)
- **Automatic SMS ticket** when a vehicle parks (Fast2SMS, Quick SMS route)
- **Unique reference number** for every vehicle, e.g. `PK-1109-101`
- **EV charging**: electric cars use charger slots and pay Rs 20 per charging hour; if all chargers are full, they park in a normal car slot
- **Real clock billing**: time is read from the computer's clock and rounded up to full hours
- **Smart fee rules**: first-hour rate + extra-hour rate per vehicle type, night charge (10 PM to 6 AM), 20% monthly-pass discount, Rs 200 lost-ticket fine
- **Full-floor alerts**: if a floor is full, the next floor is checked automatically
- **Reserved slots** for disabled drivers / VIPs
- **Search by number plate**
- **Admin panel** (password protected): daily report, fullest floor, list of parked vehicles, SMS Test Mode switch, constructor/destructor demo switch, change password
- **Safe input**: wrong inputs (letters, out-of-range numbers, invalid phone numbers) never crash the program

| Vehicle | First hour | Each extra hour |
|---|---|---|
| Bike | Rs 20 | Rs 10 |
| Car | Rs 50 | Rs 30 |
| Truck | Rs 100 | Rs 60 |
| Electric Car | Rs 50 | Rs 30 (+ Rs 20 per charging hour) |

---

## OOP concepts used

| Concept | Where |
|---|---|
| Classes and objects | Every vehicle, slot, ticket and bill is an object |
| Encapsulation | `Admin` password is private; all data members are private/protected |
| Abstraction | `Vehicle` is an abstract class (pure virtual functions) |
| Inheritance (single, multilevel, hierarchical) | `Bike`, `Car`, `Truck` from `Vehicle`; `Vehicle -> Car -> ElectricCar`; `PassMember` from `Customer` |
| Polymorphism (virtual functions) | `getType()`, rates, `getChargingFee()`, `getDiscountPercent()` |
| Constructors and destructors | Virtual destructors; `Ticket` destructor frees the vehicle and customer |
| Static data members and functions | `Ticket::ticketCounter`, `Report`, `SmsSender`, `Clock` |
| Function overloading | `findTicket(string)` and `findTicket(int)` |
| Default arguments | `parkVehicle(Vehicle*, Customer*, int wantsReserved = 0)` |
| Operator overloading | `Bill operator+(int)` adds the lost-ticket fine |
| Composition | `ParkingLot` has `Floor`s, a `Floor` has `Slot`s |
| Dynamic memory | `new` / `delete` for vehicles, customers and tickets |

---

## Project files

```
SmartParking/
├── main.cpp              menus (main menu + admin menu)
├── Clock.h               time helpers (real clock)
├── Vehicle.h             Vehicle, Bike, Car, Truck, ElectricCar
├── Customer.h            Customer, PassMember, Admin
├── Floor.h               Slot, Floor
├── Ticket.h              Ticket, Bill, Report
├── SmsSender.h           SMS sending (Test Mode / Real Mode)
├── ParkingLot.h          ParkingLot (the main controller class)
├── send_sms.py           Python helper that sends the real SMS
├── web/
│   ├── server.cpp        website server (uses the same classes)
│   ├── MiniServer.h      our own small web server (Request, Response, WebServer)
│   └── public/           index.html, style.css, app.js
├── sms_key_example.txt   shows where your API key goes
└── tests/                test programs (47 checks)
```

---

## Website version

The website uses the **same C++ classes**. `web/server.cpp` connects them to the browser, using our own small web server classes in `web/MiniServer.h` (`Request`, `Response`, `WebServer`), written with plain sockets and no outside library.

```
Browser (web/public: index.html, style.css, app.js)
   |   clicks "Park"
   v
web/server.cpp   --->   ParkingLot::parkVehicle()   --->   SmsSender  --->  send_sms.py  --->  phone
```

Features on the page: live floor map of all 3 levels (green / red / blue lamps, EV chargers), park form with ticket and SMS preview, exit with bill, find vehicle (or click a parked car), and an admin window with the report, a real-SMS switch and the constructor/destructor message switch.

**Compile and run** (from the `Parking_OOP` folder, not from inside `web`):

```
g++ web/server.cpp -o parking_web -lws2_32
parking_web
```
(On Linux / Mac: `g++ web/server.cpp -o parking_web -pthread` and `./parking_web`.)

The browser opens `http://localhost:8080` by itself. Keep the black server window open while using the page; it also shows what the C++ code is doing.

---

## How to run (cmd version)

### 1. Requirements
- A C++ compiler (g++ / MinGW / MSYS2)
- Python 3 with the `requests` library (only needed for real SMS)

### 2. Compile and run
Open a terminal **inside the project folder** and run:

```
g++ main.cpp -o parking
parking
```
(On Linux / Mac, run `./parking` instead.)

The program starts in **SMS Test Mode**: the SMS is only shown on the screen, so nothing costs money.

Admin password: `admin123`

### 3. Turning on real SMS (optional)
1. Install the library: `python -m pip install requests`
2. Make a copy of `sms_key_example.txt` named `sms_key.txt`
3. Open `sms_key.txt` and replace the text with your Fast2SMS API key
4. Run the program, go to **Admin login → SMS Test Mode** and switch it **OFF**

`sms_key.txt` is listed in `.gitignore`, so your key is never uploaded.

> If `python` does not work on your PC but `py` does, open `SmsSender.h` and change `"python"` to `"py"` on the line `string SmsSender::pythonCommand = "python";`

### 4. Running the tests
From the project folder:

```
g++ tests/test_vehicles.cpp -o test_vehicles
test_vehicles

g++ tests/test_bill.cpp -o test_bill
test_bill

g++ tests/test_parking.cpp -o test_parking
test_parking

g++ tests/test_constructor_order.cpp -o test_order
test_order
```

---

## How the SMS works

1. A customer parks and enters their mobile number
2. The C++ program creates the ticket and writes the SMS text into `last_sms.txt`
3. It runs `python send_sms.py <number>`
4. The Python script sends the text through the Fast2SMS API and reports back
5. If the SMS fails (no internet, no balance), the parking is still saved

Example SMS:
```
SmartPark Ticket
Ref: PK-1109-101
Vehicle: PB02AB1234 (Car)
Slot: G-C3
In: 09-Nov 10:30
Show Ref at exit.
```
