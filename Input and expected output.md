# Input and Output Scenarios


### **1. Administrator: Managing Flights**

**Scenario:** An Administrator logs in to add a new flight to the system.

**Input Sequence:**
```
Welcome to Airline Reservation and Management System
Please select your role:
1. Administrator
2. Booking Agent
3. Passenger
Enter choice: 1

--- Administrator Login ---
Username: adminUser
Password: ********

Login successful!

--- Administrator Menu ---
1. Manage Flights
2. Manage Aircraft
3. Manage Users
4. Generate Reports
5. Logout
Enter choice: 1

--- Manage Flights ---
1. Add New Flight
2. Update Existing Flight
3. Remove Flight
4. View All Flights
5. Back to Main Menu
Enter choice: 1

--- Add New Flight ---
Enter Flight Number: AA123
Enter Origin: New York (JFK)
Enter Destination: Los Angeles (LAX)
Enter Departure Date and Time (YYYY-MM-DD HH:MM): 2023-12-15 08:30
Enter Arrival Date and Time (YYYY-MM-DD HH:MM): 2023-12-15 11:45
Enter Aircraft Type: Boeing 737
Enter Total Seats: 180
Enter Status (Scheduled/Delayed/Canceled): Scheduled

Flight AA123 has been successfully added to the schedule.
```

**Expected Output:**
```
Welcome to Airline Reservation and Management System
Please select your role:
1. Administrator
2. Booking Agent
3. Passenger
Enter choice: 1

--- Administrator Login ---
Username: adminUser
Password: ********

Login successful!

--- Administrator Menu ---
1. Manage Flights
2. Manage Aircraft
3. Manage Users
4. Generate Reports
5. Logout
Enter choice: 1

--- Manage Flights ---
1. Add New Flight
2. Update Existing Flight
3. Remove Flight
4. View All Flights
5. Back to Main Menu
Enter choice: 1

--- Add New Flight ---
Enter Flight Number: AA123
Enter Origin: New York (JFK)
Enter Destination: Los Angeles (LAX)
Enter Departure Date and Time (YYYY-MM-DD HH:MM): 2023-12-15 08:30
Enter Arrival Date and Time (YYYY-MM-DD HH:MM): 2023-12-15 11:45
Enter Aircraft Type: Boeing 737
Enter Total Seats: 180
Enter Status (Scheduled/Delayed/Canceled): Scheduled

Flight AA123 has been successfully added to the schedule.
```

---

### **2. Booking Agent: Booking a Flight for a Passenger**

**Scenario:** A Booking Agent logs in to book a flight for a passenger.

**Input Sequence:**
```
Welcome to Airline Reservation and Management System
Please select your role:
1. Administrator
2. Booking Agent
3. Passenger
Enter choice: 2

--- Booking Agent Login ---
Username: agentJane
Password: ********

Login successful!

--- Booking Agent Menu ---
1. Search Flights
2. Book a Flight
3. Modify Reservation
4. Cancel Reservation
5. Logout
Enter choice: 2

--- Book a Flight ---
Enter Passenger ID: P456
Enter Flight Number: AA123
Enter Seat Number (e.g., 12A): 15B
Enter Payment Method (Credit Card/Cash/PayPal): Credit Card
Enter Payment Details: XXXX-XXXX-XXXX-1234

Booking successful! 
Reservation ID: R789
Passenger: John Doe
Flight: AA123 from New York (JFK) to Los Angeles (LAX)
Seat: 15B
Total Cost: $350.00
Payment Method: Credit Card
```

**Expected Output:**
```
Welcome to Airline Reservation and Management System
Please select your role:
1. Administrator
2. Booking Agent
3. Passenger
Enter choice: 2

--- Booking Agent Login ---
Username: agentJane
Password: ********

Login successful!

--- Booking Agent Menu ---
1. Search Flights
2. Book a Flight
3. Modify Reservation
4. Cancel Reservation
5. Logout
Enter choice: 2

--- Book a Flight ---
Enter Passenger ID: P456
Enter Flight Number: AA123
Enter Seat Number (e.g., 12A): 15B
Enter Payment Method (Credit Card/Cash/PayPal): Credit Card
Enter Payment Details: XXXX-XXXX-XXXX-1234

Booking successful! 
Reservation ID: R789
Passenger: John Doe
Flight: AA123 from New York (JFK) to Los Angeles (LAX)
Seat: 15B
Total Cost: $350.00
Payment Method: Credit Card
```

---

### **3. Passenger: Searching and Booking a Flight**

**Scenario:** A Passenger logs in to search for available flights and books a seat.

**Input Sequence:**
```
Welcome to Airline Reservation and Management System
Please select your role:
1. Administrator
2. Booking Agent
3. Passenger
Enter choice: 3

--- Passenger Login ---
Username: passengerMike
Password: ********

Login successful!

--- Passenger Menu ---
1. Search Flights
2. View My Reservations
3. Check-In
4. Logout
Enter choice: 1

--- Search Flights ---
Enter Origin: Chicago (ORD)
Enter Destination: Miami (MIA)
Enter Departure Date (YYYY-MM-DD): 2023-12-20

Available Flights:
1. Flight Number: BA456
   Departure: 2023-12-20 09:00
   Arrival: 2023-12-20 13:30
   Aircraft: Airbus A320
   Available Seats: 50
   Price: $250.00
2. Flight Number: AA789
   Departure: 2023-12-20 15:45
   Arrival: 2023-12-20 20:15
   Aircraft: Boeing 737
   Available Seats: 75
   Price: $300.00

Enter the Flight Number you wish to book (or '0' to cancel): BA456

Enter Seat Number (e.g., 14C): 22A
Enter Payment Method (Credit Card/Cash/PayPal): PayPal
Enter Payment Details: mike.paypal@example.com

Booking successful! 
Reservation ID: R321
Flight: BA456 from Chicago (ORD) to Miami (MIA)
Seat: 22A
Total Cost: $250.00
Payment Method: PayPal
```

**Expected Output:**
```
Welcome to Airline Reservation and Management System
Please select your role:
1. Administrator
2. Booking Agent
3. Passenger
Enter choice: 3

--- Passenger Login ---
Username: passengerMike
Password: ********

Login successful!

--- Passenger Menu ---
1. Search Flights
2. View My Reservations
3. Check-In
4. Logout
Enter choice: 1

--- Search Flights ---
Enter Origin: Chicago (ORD)
Enter Destination: Miami (MIA)
Enter Departure Date (YYYY-MM-DD): 2023-12-20

Available Flights:
1. Flight Number: BA456
   Departure: 2023-12-20 09:00
   Arrival: 2023-12-20 13:30
   Aircraft: Airbus A320
   Available Seats: 50
   Price: $250.00
2. Flight Number: AA789
   Departure: 2023-12-20 15:45
   Arrival: 2023-12-20 20:15
   Aircraft: Boeing 737
   Available Seats: 75
   Price: $300.00

Enter the Flight Number you wish to book (or '0' to cancel): BA456

Enter Seat Number (e.g., 14C): 22A
Enter Payment Method (Credit Card/Cash/PayPal): PayPal
Enter Payment Details: mike.paypal@example.com

Booking successful! 
Reservation ID: R321
Flight: BA456 from Chicago (ORD) to Miami (MIA)
Seat: 22A
Total Cost: $250.00
Payment Method: PayPal
```

---

### **4. Passenger: Checking In for a Flight**

**Scenario:** A Passenger checks in online and obtains a boarding pass.

**Input Sequence:**
```
--- Passenger Menu ---
1. Search Flights
2. View My Reservations
3. Check-In
4. Logout
Enter choice: 3

--- Check-In ---
Enter Reservation ID: R321

Check-In Successful!
Boarding Pass:
-----------------------------
Reservation ID: R321
Passenger: Mike Smith
Flight: BA456
Origin: Chicago (ORD)
Destination: Miami (MIA)
Departure: 2023-12-20 09:00
Seat: 22A
Gate: B12
Boarding Time: 07:30
-----------------------------
```

**Expected Output:**
```
--- Passenger Menu ---
1. Search Flights
2. View My Reservations
3. Check-In
4. Logout
Enter choice: 3

--- Check-In ---
Enter Reservation ID: R321

Check-In Successful!
Boarding Pass:
-----------------------------
Reservation ID: R321
Passenger: Mike Smith
Flight: BA456
Origin: Chicago (ORD)
Destination: Miami (MIA)
Departure: 2023-12-20 09:00
Seat: 22A
Gate: B12
Boarding Time: 07:30
-----------------------------
```

---

### **5. Administrator: Generating a Flight Performance Report**

**Scenario:** An Administrator generates a report on flight performance for a specific month.

**Input Sequence:**
```
--- Administrator Menu ---
1. Manage Flights
2. Manage Aircraft
3. Manage Users
4. Generate Reports
5. Logout
Enter choice: 4

--- Generate Reports ---
1. Operational Reports
2. Maintenance Reports
3. User Activity Reports
4. Back to Main Menu
Enter choice: 1

--- Operational Reports ---
Enter Month and Year for Report (MM-YYYY): 12-2023

Generating Operational Report for December 2023...

Report Summary:
- Total Flights Scheduled: 150
- Flights Completed: 145
- Flights Delayed: 3
- Flights Canceled: 2
- Total Reservations Made: 10,500
- Total Revenue: $3,675,000.00

Detailed Flight Performance:
1. Flight AA123: On Time (120 Bookings, $42,000)
2. Flight BA456: Delayed (80 Bookings, $20,000)
...
```

**Expected Output:**
```
--- Administrator Menu ---
1. Manage Flights
2. Manage Aircraft
3. Manage Users
4. Generate Reports
5. Logout
Enter choice: 4

--- Generate Reports ---
1. Operational Reports
2. Maintenance Reports
3. User Activity Reports
4. Back to Main Menu
Enter choice: 1

--- Operational Reports ---
Enter Month and Year for Report (MM-YYYY): 12-2023

Generating Operational Report for December 2023...

Report Summary:
- Total Flights Scheduled: 150
- Flights Completed: 145
- Flights Delayed: 3
- Flights Canceled: 2
- Total Reservations Made: 10,500
- Total Revenue: $3,675,000.00

Detailed Flight Performance:
1. Flight AA123: On Time (120 Bookings, $42,000)
2. Flight BA456: Delayed (80 Bookings, $20,000)
...
```

---

### **6. Error Handling: Attempting to Book a Fully Occupied Seat**

**Scenario:** A Booking Agent tries to book a seat that has already been taken.

**Input Sequence:**
```
--- Book a Flight ---
Enter Passenger ID: P789
Enter Flight Number: AA123
Enter Seat Number (e.g., 12A): 15B
Enter Payment Method (Credit Card/Cash/PayPal): Credit Card
Enter Payment Details: XXXX-XXXX-XXXX-5678
```

**Expected Output:**
```
--- Book a Flight ---
Enter Passenger ID: P789
Enter Flight Number: AA123
Enter Seat Number (e.g., 12A): 15B
Enter Payment Method (Credit Card/Cash/PayPal): Credit Card
Enter Payment Details: XXXX-XXXX-XXXX-5678

Error: Seat 15B on Flight AA123 is already occupied. Please choose a different seat.
```

---

### **7. Passenger: Viewing Reservations**

**Scenario:** A Passenger views all their current reservations.

**Input Sequence:**
```
--- Passenger Menu ---
1. Search Flights
2. View My Reservations
3. Check-In
4. Logout
Enter choice: 2

--- My Reservations ---
Fetching reservations for Passenger Mike Smith...

1. Reservation ID: R321
   Flight: BA456 from Chicago (ORD) to Miami (MIA)
   Departure: 2023-12-20 09:00
   Seat: 22A
   Status: Confirmed

2. Reservation ID: R322
   Flight: AA123 from New York (JFK) to Los Angeles (LAX)
   Departure: 2023-12-25 14:00
   Seat: 12C
   Status: Canceled
```

**Expected Output:**
```
--- Passenger Menu ---
1. Search Flights
2. View My Reservations
3. Check-In
4. Logout
Enter choice: 2

--- My Reservations ---
Fetching reservations for Passenger Mike Smith...

1. Reservation ID: R321
   Flight: BA456 from Chicago (ORD) to Miami (MIA)
   Departure: 2023-12-20 09:00
   Seat: 22A
   Status: Confirmed

2. Reservation ID: R322
   Flight: AA123 from New York (JFK) to Los Angeles (LAX)
   Departure: 2023-12-25 14:00
   Seat: 12C
   Status: Canceled
```

---

### **8. Booking Agent: Canceling a Reservation and Processing Refund**

**Scenario:** A Booking Agent cancels a passenger's reservation and processes a refund.

**Input Sequence:**
```
--- Booking Agent Menu ---
1. Search Flights
2. Book a Flight
3. Modify Reservation
4. Cancel Reservation
5. Logout
Enter choice: 4

--- Cancel Reservation ---
Enter Reservation ID: R321

Are you sure you want to cancel Reservation ID R321? (yes/no): yes

Processing refund to PayPal account: mike.paypal@example.com

Reservation R321 has been successfully canceled.
Refund Amount: $250.00 has been credited to your PayPal account.
```

**Expected Output:**
```
--- Booking Agent Menu ---
1. Search Flights
2. Book a Flight
3. Modify Reservation
4. Cancel Reservation
5. Logout
Enter choice: 4

--- Cancel Reservation ---
Enter Reservation ID: R321

Are you sure you want to cancel Reservation ID R321? (yes/no): yes

Processing refund to PayPal account: mike.paypal@example.com

Reservation R321 has been successfully canceled.
Refund Amount: $250.00 has been credited to your PayPal account.
```

---

### **9. Administrator: Assigning Crew to a Flight**

**Scenario:** An Administrator assigns a pilot and flight attendant to a specific flight.

**Input Sequence:**
```
--- Administrator Menu ---
1. Manage Flights
2. Manage Aircraft
3. Manage Users
4. Generate Reports
5. Logout
Enter choice: 1

--- Manage Flights ---
1. Add New Flight
2. Update Existing Flight
3. Remove Flight
4. View All Flights
5. Back to Main Menu
Enter choice: 2

--- Update Existing Flight ---
Enter Flight Number to Update: AA123

Select information to update:
1. Flight Details
2. Crew Assignments
3. Status
4. Back to Manage Flights
Enter choice: 2

--- Crew Assignments ---
Available Pilots:
1. Pilot ID: PL001 - Captain Sarah Connor
2. Pilot ID: PL002 - Captain James Kirk

Select Pilot by ID: PL001

Available Flight Attendants:
1. FA001 - Emma Watson
2. FA002 - John Cena

Select Flight Attendant by ID: FA002

Crew assigned successfully to Flight AA123.
```

**Expected Output:**
```
--- Administrator Menu ---
1. Manage Flights
2. Manage Aircraft
3. Manage Users
4. Generate Reports
5. Logout
Enter choice: 1

--- Manage Flights ---
1. Add New Flight
2. Update Existing Flight
3. Remove Flight
4. View All Flights
5. Back to Main Menu
Enter choice: 2

--- Update Existing Flight ---
Enter Flight Number to Update: AA123

Select information to update:
1. Flight Details
2. Crew Assignments
3. Status
4. Back to Manage Flights
Enter choice: 2

--- Crew Assignments ---
Available Pilots:
1. Pilot ID: PL001 - Captain Sarah Connor
2. Pilot ID: PL002 - Captain James Kirk

Select Pilot by ID: PL001

Available Flight Attendants:
1. FA001 - Emma Watson
2. FA002 - John Cena

Select Flight Attendant by ID: FA002

Crew assigned successfully to Flight AA123.
```

---

### **10. Passenger: Attempting to Select an Invalid Seat**

**Scenario:** A Passenger tries to select a seat number that doesn't exist on the aircraft.

**Input Sequence:**
```
--- Book a Flight ---
Enter Passenger ID: P123
Enter Flight Number: AA123
Enter Seat Number (e.g., 12A): 30Z
Enter Payment Method (Credit Card/Cash/PayPal): Credit Card
Enter Payment Details: XXXX-XXXX-XXXX-9999
```

**Expected Output:**
```
--- Book a Flight ---
Enter Passenger ID: P123
Enter Flight Number: AA123
Enter Seat Number (e.g., 12A): 30Z
Enter Payment Method (Credit Card/Cash/PayPal): Credit Card
Enter Payment Details: XXXX-XXXX-XXXX-9999

Error: Seat number 30Z is invalid. Please enter a valid seat number (e.g., 12A).
```

---

### **Summary of Input and Output Examples**

1. **Administrator Managing Flights:**
   - **Input:** Selecting role, logging in, choosing to add a flight, entering flight details.
   - **Output:** Confirmation message that the flight has been added.

2. **Booking Agent Booking a Flight:**
   - **Input:** Selecting role, logging in, choosing to book a flight, entering passenger and flight details.
   - **Output:** Booking confirmation with reservation details.

3. **Passenger Searching and Booking a Flight:**
   - **Input:** Selecting role, logging in, searching for flights, selecting a flight, entering seat and payment information.
   - **Output:** Booking success message with reservation information.

4. **Passenger Checking In:**
   - **Input:** Selecting check-in option, entering reservation ID.
   - **Output:** Boarding pass details.

5. **Administrator Generating Reports:**
   - **Input:** Selecting role, logging in, choosing to generate operational reports, specifying month and year.
   - **Output:** Summary and detailed operational report.

6. **Error Handling in Booking:**
   - **Input:** Attempting to book an already occupied seat.
   - **Output:** Error message indicating seat is occupied.

7. **Passenger Viewing Reservations:**
   - **Input:** Selecting role, logging in, choosing to view reservations.
   - **Output:** List of current reservations with details.

8. **Booking Agent Canceling Reservation:**
   - **Input:** Selecting role, logging in, choosing to cancel reservation, confirming cancellation.
   - **Output:** Cancellation confirmation and refund processing message.

9. **Administrator Assigning Crew:**
   - **Input:** Selecting role, logging in, choosing to manage flights, updating crew assignments.
   - **Output:** Confirmation of crew assignment.

10. **Passenger Selecting Invalid Seat:**
    - **Input:** Attempting to select a non-existent seat number.
    - **Output:** Error message indicating invalid seat number.
