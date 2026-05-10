#ifndef FLIGHT_H
#define FLIGHT_H

#include "raylib.h"
#include <string>
#include <vector>
using namespace std;

// ===================== FLIGHT CLASS =====================

class Flight {
private:
    string flightNumber;
    string origin;
    string destination;
    string departureTime;
    string arrivalTime;
    string status;

    string aircraftID;
    string gateNumber;
    string pilotID;
    string cabinCrewIDs;
    string passengerIDs;

    int delayMinutes;
    string delayReason;

public:
    Flight();

    Flight(
        string flightNumber,
        string origin,
        string destination,
        string departureTime,
        string arrivalTime
    );

    string getFlightNumber() const;
    string getOrigin() const;
    string getDestination() const;
    string getDepartureTime() const;
    string getArrivalTime() const;
    string getStatus() const;

    string getAircraftID() const;
    string getGateNumber() const;
    string getPilotID() const;
    string getCabinCrewIDs() const;
    string getPassengerIDs() const;

    int getDelayMinutes() const;
    string getDelayReason() const;

    void setOrigin(string value);
    void setDestination(string value);
    void setDepartureTime(string value);
    void setArrivalTime(string value);

    void updateStatus(string newStatus);
    void applyDelay(int minutes, string reason);

    void connectAircraft(string id);
    void connectGate(string gate);
    void connectPilot(string id);
    void connectCabinCrew(string ids);
    void connectPassengers(string ids);

    string serialize() const;
    void deserialize(string line);
};

// ===================== FLIGHT MODULE CLASS =====================

class FlightModule {
private:
    vector<Flight> flights;

    enum Page {
        MENU,
        ADD,
        VIEW,
        SEARCH,
        UPDATE_STATUS,
        AUTO_DELAY,
        CONNECT_RELATIONS,
        DELETE_FLIGHT,
        REPORT
    };

    Page currentPage;

    string message;
    Color messageColor;

    string inputFlightNumber;
    string inputOrigin;
    string inputDestination;
    string inputDeparture;
    string inputArrival;
    string inputStatus;

    string inputAircraftID;
    string inputGateNumber;
    string inputPilotID;
    string inputCabinCrewIDs;
    string inputPassengerIDs;

    string inputDelayMinutes;
    string inputDelayReason;

    bool activeFlightNumber;
    bool activeOrigin;
    bool activeDestination;
    bool activeDeparture;
    bool activeArrival;
    bool activeStatus;

    bool activeAircraftID;
    bool activeGateNumber;
    bool activePilotID;
    bool activeCabinCrewIDs;
    bool activePassengerIDs;

    bool activeDelayMinutes;
    bool activeDelayReason;

    int selectedFlightIndex;

public:
    FlightModule();

    bool Draw(
        Texture2D logo,
        Texture2D background,
        Texture2D plane,
        Texture2D warningIcon
    );

    void loadFromFile();
    void saveToFile();

private:
    void drawBackground(Texture2D background);
    void drawHeader(Texture2D logo, const char* title);

    void drawMenu(Texture2D logo, Texture2D background, Texture2D plane, Texture2D warningIcon);
    void drawAddPage(Texture2D logo, Texture2D background, Texture2D plane);
    void drawViewPage(Texture2D logo, Texture2D background, Texture2D plane);
    void drawSearchPage(Texture2D logo, Texture2D background, Texture2D plane);
    void drawUpdateStatusPage(Texture2D logo, Texture2D background);
    void drawAutoDelayPage(Texture2D logo, Texture2D background, Texture2D warningIcon);
    void drawConnectRelationsPage(Texture2D logo, Texture2D background, Texture2D plane);
    void drawDeletePage(Texture2D logo, Texture2D background);
    void drawReportPage(Texture2D logo, Texture2D background);

    bool button(Rectangle rect, const char* text, Color color, Color hoverColor, Color textColor);
    void textBox(Rectangle rect, string& text, bool& active, const char* label);
    void drawIcon(Texture2D texture, Rectangle rect);
    void drawFlightCard(Flight flight, Rectangle rect, Texture2D plane);

    void resetInputs();
    int findFlight(string flightNumber);
    int safeToInt(string text);
};

#endif