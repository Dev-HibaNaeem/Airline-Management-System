#ifndef AIRPORT_H
#define AIRPORT_H

#include "raylib.h"
#include <string>
#include <vector>
using namespace std;

// ===================== AIRPORT FLIGHT INFO =====================

struct AirportFlightInfo {
    string flightNumber;
    string origin;
    string destination;
    string departureTime;
    string arrivalTime;
    string status;
};

// ===================== AIRPORT ASSIGNMENT =====================

class AirportAssignment {
private:
    string flightNumber;
    string gateNumber;
    string runwayNumber;
    string assignmentType;
    string status;

public:
    AirportAssignment();
    AirportAssignment(
        string flightNumber,
        string gateNumber,
        string runwayNumber,
        string assignmentType,
        string status
    );

    string getFlightNumber() const;
    string getGateNumber() const;
    string getRunwayNumber() const;
    string getAssignmentType() const;
    string getStatus() const;

    string serialize() const;
    void deserialize(string line);
};

// ===================== AIRPORT MODULE =====================

class AirportModule {
private:
    vector<AirportAssignment> assignments;

    enum Page {
        MENU,
        ASSIGN_GATE_RUNWAY,
        VIEW_ASSIGNMENTS,
        DEPARTURE_DISPLAY,
        ARRIVAL_DISPLAY,
        REPORT
    };

    Page currentPage;

    string message;
    Color messageColor;

    string inputFlightNumber;
    string inputGateNumber;
    string inputRunwayNumber;
    string inputAssignmentType;

    bool activeFlightNumber;
    bool activeGateNumber;
    bool activeRunwayNumber;
    bool activeAssignmentType;

public:
    AirportModule();

    bool Draw(
        Texture2D logo,
        Texture2D background,
        Texture2D gateIcon,
        Texture2D planeIcon
    );

    void loadFromFile();
    void saveToFile();

private:
    void drawBackground(Texture2D background);
    void drawHeader(Texture2D logo, const char* title);

    void drawMenu(Texture2D logo, Texture2D background, Texture2D gateIcon, Texture2D planeIcon);
    void drawAssignPage(Texture2D logo, Texture2D background, Texture2D gateIcon);
    void drawViewAssignmentsPage(Texture2D logo, Texture2D background, Texture2D gateIcon);
    void drawDepartureDisplayPage(Texture2D logo, Texture2D background, Texture2D planeIcon);
    void drawArrivalDisplayPage(Texture2D logo, Texture2D background, Texture2D planeIcon);
    void drawReportPage(Texture2D logo, Texture2D background);

    bool button(Rectangle rect, const char* text, Color color, Color hoverColor, Color textColor);
    void textBox(Rectangle rect, string& text, bool& active, const char* label);
    void drawIcon(Texture2D texture, Rectangle rect);

    vector<AirportFlightInfo> getFlightsFromFlightModule();
    bool isActiveFlightStatus(string status);
    bool flightExistsAndReady(string flightNumber);
    AirportFlightInfo getFlightInfo(string flightNumber);

    vector<string> getDefaultGates();
    vector<string> getDefaultRunways();

    bool isGateBusy(string gateNumber);
    bool isRunwayBusy(string runwayNumber);

    string getFreeGate(string preferredGate);
    string getFreeRunway(string preferredRunway);

    bool assignmentExists(string flightNumber);

    void resetInputs();
};

#endif