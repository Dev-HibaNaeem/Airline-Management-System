#ifndef SYSTEM_UTILS_H
#define SYSTEM_UTILS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
using namespace std;

// ===================== ID GENERATOR =====================

inline string makeID(string prefix, int number) {
    stringstream ss;
    ss << prefix << setw(4) << setfill('0') << number;
    return ss.str();
}

inline int countFileLines(string fileName) {
    ifstream file(fileName);
    string line;
    int count = 0;

    while (getline(file, line)) {
        if (line.length() > 0) {
            count++;
        }
    }

    file.close();
    return count;
}

inline string generateNextID(string prefix, string fileName) {
    int nextNumber = countFileLines(fileName) + 1;
    return makeID(prefix, nextNumber);
}

// ===================== BASIC AIRCRAFT DATA =====================

inline bool aircraftExistsInFile(string aircraftID) {
    ifstream file("aircraft.txt");
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string firstPart;

        getline(ss, firstPart, '|');

        if (firstPart == aircraftID) {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

// ===================== BASIC FLIGHT DATA =====================

inline bool flightExistsInFile(string flightNumber) {
    ifstream file("flights.txt");
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string firstPart;

        getline(ss, firstPart, '|');

        if (firstPart == flightNumber) {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

// ===================== BASIC PILOT DATA =====================

inline bool pilotExistsInFile(string pilotID) {
    ifstream file("pilot.txt");
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string firstPart;

        getline(ss, firstPart, '|');

        if (firstPart == pilotID) {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

// ===================== BASIC CABIN CREW DATA =====================

inline bool cabinCrewExistsInFile(string crewID) {
    ifstream file("cabincrew.txt");
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string firstPart;

        getline(ss, firstPart, '|');

        if (firstPart == crewID) {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

// ===================== BASIC PASSENGER DATA =====================

struct PassengerBasicInfo {
    string passengerID;
    string fullName;
    string passportNumber;
    string nationality;
    string phoneNumber;
    string email;
    bool found;
};

inline PassengerBasicInfo findPassengerInFile(string passengerID) {
    PassengerBasicInfo info;

    info.passengerID = "";
    info.fullName = "";
    info.passportNumber = "";
    info.nationality = "";
    info.phoneNumber = "";
    info.email = "";
    info.found = false;

    ifstream file("passengers.txt");
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> data;
        string part;

        while (getline(ss, part, '|')) {
            data.push_back(part);
        }

        if ((int)data.size() >= 6 && data[0] == passengerID) {
            info.passengerID = data[0];
            info.fullName = data[1];
            info.passportNumber = data[2];
            info.nationality = data[3];
            info.phoneNumber = data[4];
            info.email = data[5];
            info.found = true;
            break;
        }
    }

    file.close();
    return info;
}

// ===================== FLIGHT PASSENGER MANIFEST =====================

struct FlightPassengerManifest {
    string flightNumber;
    string passengerID;
    string fullName;
    string passportNumber;
    string phoneNumber;
    string email;
    string seatNumber;
    string mealPreference;
    string ticketStatus;
};

inline bool passengerAlreadyInFlight(string flightNumber, string passengerID) {
    ifstream file("flight_passengers.txt");
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> data;
        string part;

        while (getline(ss, part, '|')) {
            data.push_back(part);
        }

        if ((int)data.size() >= 2 && data[0] == flightNumber && data[1] == passengerID) {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

// This extra function fixes your Passenger.cpp error
inline bool passengerBookedOnFlight(string flightNumber, string passengerID) {
    return passengerAlreadyInFlight(flightNumber, passengerID);
}

inline void addPassengerToFlightManifest(
    string flightNumber,
    PassengerBasicInfo passenger,
    string seatNumber,
    string mealPreference,
    string ticketStatus
) {
    if (passengerAlreadyInFlight(flightNumber, passenger.passengerID)) {
        return;
    }

    ofstream file("flight_passengers.txt", ios::app);

    file << flightNumber << "|"
         << passenger.passengerID << "|"
         << passenger.fullName << "|"
         << passenger.passportNumber << "|"
         << passenger.phoneNumber << "|"
         << passenger.email << "|"
         << seatNumber << "|"
         << mealPreference << "|"
         << ticketStatus << endl;

    file.close();
}

inline vector<FlightPassengerManifest> getPassengersOfFlight(string flightNumber) {
    vector<FlightPassengerManifest> list;

    ifstream file("flight_passengers.txt");
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> data;
        string part;

        while (getline(ss, part, '|')) {
            data.push_back(part);
        }

        if ((int)data.size() >= 9 && data[0] == flightNumber) {
            FlightPassengerManifest item;

            item.flightNumber = data[0];
            item.passengerID = data[1];
            item.fullName = data[2];
            item.passportNumber = data[3];
            item.phoneNumber = data[4];
            item.email = data[5];
            item.seatNumber = data[6];
            item.mealPreference = data[7];
            item.ticketStatus = data[8];

            list.push_back(item);
        }
    }

    file.close();
    return list;
}

inline int getPassengerCountForFlight(string flightNumber) {
    vector<FlightPassengerManifest> list = getPassengersOfFlight(flightNumber);
    return (int)list.size();
}

#endif