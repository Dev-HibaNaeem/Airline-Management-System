#include "Airport.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

// ===================== RAYLIB COMPATIBILITY =====================

static void DrawAirportRoundedLines(Rectangle rec, float roundness, int segments, float lineThick, Color color) {
    DrawRectangleRoundedLines(rec, roundness, segments, color);
}

// ===================== THEME COLORS =====================

static Color AMS_NAVY = { 4, 18, 38, 255 };
static Color AMS_TOP_NAVY = { 8, 28, 58, 255 };
static Color AMS_BLUE = { 25, 118, 210, 255 };
static Color AMS_LIGHT_BLUE = { 83, 169, 255, 255 };
static Color AMS_YELLOW = { 255, 193, 7, 255 };
static Color AMS_WHITE = { 245, 248, 255, 255 };
static Color AMS_CARD = { 16, 42, 82, 245 };
static Color AMS_RED = { 230, 57, 70, 255 };
static Color AMS_GREEN = { 46, 204, 113, 255 };

// ===================== AIRPORT ASSIGNMENT CLASS =====================

AirportAssignment::AirportAssignment() {
    flightNumber = "";
    gateNumber = "";
    runwayNumber = "";
    assignmentType = "";
    status = "";
}

AirportAssignment::AirportAssignment(
    string flightNumber,
    string gateNumber,
    string runwayNumber,
    string assignmentType,
    string status
) {
    this->flightNumber = flightNumber;
    this->gateNumber = gateNumber;
    this->runwayNumber = runwayNumber;
    this->assignmentType = assignmentType;
    this->status = status;
}

string AirportAssignment::getFlightNumber() const {
    return flightNumber;
}

string AirportAssignment::getGateNumber() const {
    return gateNumber;
}

string AirportAssignment::getRunwayNumber() const {
    return runwayNumber;
}

string AirportAssignment::getAssignmentType() const {
    return assignmentType;
}

string AirportAssignment::getStatus() const {
    return status;
}

string AirportAssignment::serialize() const {
    stringstream ss;

    ss << flightNumber << "|"
       << gateNumber << "|"
       << runwayNumber << "|"
       << assignmentType << "|"
       << status;

    return ss.str();
}

void AirportAssignment::deserialize(string line) {
    stringstream ss(line);
    vector<string> data;
    string part;

    while (getline(ss, part, '|')) {
        data.push_back(part);
    }

    if ((int)data.size() < 5) {
        return;
    }

    flightNumber = data[0];
    gateNumber = data[1];
    runwayNumber = data[2];
    assignmentType = data[3];
    status = data[4];
}

// ===================== AIRPORT MODULE =====================

AirportModule::AirportModule() {
    currentPage = MENU;

    message = "";
    messageColor = AMS_WHITE;

    inputFlightNumber = "";
    inputGateNumber = "";
    inputRunwayNumber = "";
    inputAssignmentType = "Departure";

    activeFlightNumber = false;
    activeGateNumber = false;
    activeRunwayNumber = false;
    activeAssignmentType = false;

    loadFromFile();
}

bool AirportModule::Draw(
    Texture2D logo,
    Texture2D background,
    Texture2D gateIcon,
    Texture2D planeIcon
) {
    if (currentPage == MENU) {
        drawMenu(logo, background, gateIcon, planeIcon);
    }
    else if (currentPage == ASSIGN_GATE_RUNWAY) {
        drawAssignPage(logo, background, gateIcon);
    }
    else if (currentPage == VIEW_ASSIGNMENTS) {
        drawViewAssignmentsPage(logo, background, gateIcon);
    }
    else if (currentPage == DEPARTURE_DISPLAY) {
        drawDepartureDisplayPage(logo, background, planeIcon);
    }
    else if (currentPage == ARRIVAL_DISPLAY) {
        drawArrivalDisplayPage(logo, background, planeIcon);
    }
    else if (currentPage == REPORT) {
        drawReportPage(logo, background);
    }

    if (button({ 40, 665, 170, 52 }, "BACK", AMS_YELLOW, { 255, 210, 70, 255 }, AMS_NAVY)) {
        if (currentPage == MENU) {
            return true;
        }

        currentPage = MENU;
        message = "";
        resetInputs();
    }

    return false;
}

// ===================== DRAW HELPERS =====================

void AirportModule::drawBackground(Texture2D background) {
    ClearBackground(AMS_NAVY);

    if (background.id > 0) {
        DrawTexturePro(
            background,
            { 0, 0, (float)background.width, (float)background.height },
            { 0, 0, 1200, 750 },
            { 0, 0 },
            0,
            Fade(WHITE, 0.25f)
        );
    }

    DrawRectangleGradientV(
        0,
        0,
        1200,
        750,
        Fade(AMS_TOP_NAVY, 0.94f),
        Fade(AMS_NAVY, 0.98f)
    );
}

void AirportModule::drawHeader(Texture2D logo, const char* title) {
    DrawRectangle(0, 0, 1200, 96, AMS_TOP_NAVY);
    DrawRectangle(0, 93, 1200, 3, AMS_YELLOW);

    if (logo.id > 0) {
        DrawTexturePro(
            logo,
            { 0, 0, (float)logo.width, (float)logo.height },
            { 25, 17, 70, 70 },
            { 0, 0 },
            0,
            WHITE
        );
    }

    DrawAirportRoundedLines({ 20, 12, 80, 80 }, 0.12f, 8, 2.0f, AMS_YELLOW);
    DrawText(title, 125, 33, 32, AMS_WHITE);
}

bool AirportModule::button(Rectangle rect, const char* text, Color color, Color hoverColor, Color textColor) {
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, rect);

    DrawRectangleRounded(rect, 0.20f, 12, hover ? hoverColor : color);
    DrawAirportRoundedLines(rect, 0.20f, 12, 2.0f, AMS_YELLOW);

    int fontSize = 18;
    int textWidth = MeasureText(text, fontSize);

    int textX = (int)(rect.x + rect.width / 2 - textWidth / 2);
    int textY = (int)(rect.y + rect.height / 2 - fontSize / 2);

    DrawText(text, textX, textY, fontSize, textColor);

    return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void AirportModule::textBox(Rectangle rect, string& text, bool& active, const char* label) {
    DrawText(label, (int)rect.x, (int)rect.y - 24, 18, AMS_WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        active = CheckCollisionPointRec(GetMousePosition(), rect);
    }

    DrawRectangleRounded(rect, 0.15f, 10, AMS_WHITE);
    DrawAirportRoundedLines(rect, 0.15f, 10, 2.0f, active ? AMS_YELLOW : AMS_BLUE);

    DrawText(text.c_str(), (int)rect.x + 12, (int)rect.y + 14, 20, AMS_NAVY);

    if (active) {
        int key = GetCharPressed();

        while (key > 0) {
            if (key >= 32 && key <= 125 && text.length() < 45) {
                text += (char)key;
            }

            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && text.length() > 0) {
            text.pop_back();
        }
    }
}

void AirportModule::drawIcon(Texture2D texture, Rectangle rect) {
    if (texture.id > 0) {
        DrawTexturePro(
            texture,
            { 0, 0, (float)texture.width, (float)texture.height },
            rect,
            { 0, 0 },
            0,
            WHITE
        );
    }
}

// ===================== MENU PAGE =====================

void AirportModule::drawMenu(Texture2D logo, Texture2D background, Texture2D gateIcon, Texture2D planeIcon) {
    drawBackground(background);
    drawHeader(logo, "Airport Management");

    DrawText(
        "Manage gates, runways, ready flights, airport assignments, arrival and departure displays.",
        55,
        125,
        20,
        Fade(AMS_WHITE, 0.85f)
    );

    vector<string> options = {
        "Assign Gate & Runway",
        "View Assignments",
        "Departure Display",
        "Arrival Display",
        "Airport Report"
    };

    int startX = 55;
    int startY = 185;
    int w = 340;
    int h = 60;
    int gapX = 45;
    int gapY = 30;

    for (int i = 0; i < (int)options.size(); i++) {
        int row = i / 2;
        int col = i % 2;

        Rectangle rect = {
            (float)(startX + col * (w + gapX)),
            (float)(startY + row * (h + gapY)),
            (float)w,
            (float)h
        };

        if (button(rect, options[i].c_str(), AMS_BLUE, AMS_LIGHT_BLUE, AMS_WHITE)) {
            currentPage = (Page)(i + 1);
            message = "";
            resetInputs();
        }
    }

    Rectangle infoBox = { 640, 515, 500, 135 };

DrawRectangleRounded(infoBox, 0.16f, 12, AMS_CARD);
DrawAirportRoundedLines(infoBox, 0.16f, 12, 2.0f, AMS_YELLOW);

drawIcon(gateIcon, { infoBox.x + 30, infoBox.y + 35, 58, 58 });
drawIcon(planeIcon, { infoBox.x + 115, infoBox.y + 42, 85, 50 });

DrawText("Airport Control System", (int)infoBox.x + 230, (int)infoBox.y + 25, 19, AMS_YELLOW);
DrawText("Reads ready flights from", (int)infoBox.x + 230, (int)infoBox.y + 62, 15, AMS_WHITE);
DrawText("flights.txt file.", (int)infoBox.x + 230, (int)infoBox.y + 90, 15, AMS_WHITE);

    DrawRectangleRounded(infoBox, 0.16f, 12, AMS_CARD);
    DrawAirportRoundedLines(infoBox, 0.16f, 12, 2.0f, AMS_YELLOW);

    drawIcon(gateIcon, { infoBox.x + 25, infoBox.y + 28, 65, 65 });
    drawIcon(planeIcon, { infoBox.x + 115, infoBox.y + 35, 90, 55 });

    DrawText("Airport Control System", (int)infoBox.x + 225, (int)infoBox.y + 25, 20, AMS_YELLOW);
    DrawText("Reads ready flights from", (int)infoBox.x + 225, (int)infoBox.y + 62, 16, AMS_WHITE);
    DrawText("flights.txt file.", (int)infoBox.x + 225, (int)infoBox.y + 88, 16, AMS_WHITE);
}

// ===================== ASSIGN PAGE =====================

void AirportModule::drawAssignPage(Texture2D logo, Texture2D background, Texture2D gateIcon) {
    drawBackground(background);
    drawHeader(logo, "Assign Gate And Runway");

    drawIcon(gateIcon, { 880, 140, 135, 135 });

    DrawText(
        "Only Ready To Fly, Scheduled, and Boarding flights can be assigned.",
        60,
        120,
        19,
        AMS_YELLOW
    );

    textBox({ 60, 190, 300, 50 }, inputFlightNumber, activeFlightNumber, "Flight Number");
    textBox({ 400, 190, 300, 50 }, inputGateNumber, activeGateNumber, "Preferred Gate Example G1");

    textBox({ 60, 310, 300, 50 }, inputRunwayNumber, activeRunwayNumber, "Preferred Runway Example R1");
    textBox({ 400, 310, 300, 50 }, inputAssignmentType, activeAssignmentType, "Type Departure / Arrival");

    DrawText("Available Gates: G1 G2 G3 G4 G5", 60, 410, 18, AMS_WHITE);
    DrawText("Available Runways: R1 R2 R3", 60, 440, 18, AMS_WHITE);

    if (button({ 60, 515, 250, 55 }, "ASSIGN NOW", AMS_GREEN, { 70, 220, 140, 255 }, AMS_WHITE)) {
        if (inputFlightNumber == "" || inputGateNumber == "" || inputRunwayNumber == "" || inputAssignmentType == "") {
            message = "Please fill flight number, gate, runway, and type.";
            messageColor = AMS_RED;
        }
        else if (!flightExistsAndReady(inputFlightNumber)) {
            message = "Flight not found or flight status is not ready for airport assignment.";
            messageColor = AMS_RED;
        }
        else if (assignmentExists(inputFlightNumber)) {
            message = "This flight already has airport assignment.";
            messageColor = AMS_RED;
        }
        else {
            string finalGate = getFreeGate(inputGateNumber);
            string finalRunway = getFreeRunway(inputRunwayNumber);

            if (finalGate == "") {
                message = "No free gate available.";
                messageColor = AMS_RED;
            }
            else if (finalRunway == "") {
                message = "No free runway available.";
                messageColor = AMS_RED;
            }
            else {
                AirportAssignment assignment(
                    inputFlightNumber,
                    finalGate,
                    finalRunway,
                    inputAssignmentType,
                    "Assigned"
                );

                assignments.push_back(assignment);
                saveToFile();

                if (finalGate != inputGateNumber || finalRunway != inputRunwayNumber) {
                    message = "Preferred gate/runway was busy. Auto assigned: " + finalGate + " and " + finalRunway;
                }
                else {
                    message = "Gate and runway assigned successfully.";
                }

                messageColor = AMS_GREEN;
                resetInputs();
            }
        }
    }

    DrawText(message.c_str(), 330, 532, 19, messageColor);
}

// ===================== VIEW ASSIGNMENTS PAGE =====================

void AirportModule::drawViewAssignmentsPage(Texture2D logo, Texture2D background, Texture2D gateIcon) {
    drawBackground(background);
    drawHeader(logo, "View Airport Assignments");

    DrawText("Assigned Gates And Runways", 60, 120, 26, AMS_YELLOW);

    if (assignments.empty()) {
        DrawText("No airport assignments found.", 60, 180, 22, AMS_WHITE);
        return;
    }

    int y = 165;

    DrawText("Flight", 80, y, 18, AMS_YELLOW);
    DrawText("Gate", 230, y, 18, AMS_YELLOW);
    DrawText("Runway", 360, y, 18, AMS_YELLOW);
    DrawText("Type", 520, y, 18, AMS_YELLOW);
    DrawText("Status", 720, y, 18, AMS_YELLOW);

    y += 35;

    for (int i = 0; i < (int)assignments.size() && i < 12; i++) {
        Rectangle rowBox = { 60, (float)y - 8, 900, 35 };

        DrawRectangleRounded(rowBox, 0.12f, 8, AMS_CARD);
        DrawAirportRoundedLines(rowBox, 0.12f, 8, 1.5f, AMS_BLUE);

        DrawText(assignments[i].getFlightNumber().c_str(), 80, y, 17, AMS_WHITE);
        DrawText(assignments[i].getGateNumber().c_str(), 230, y, 17, AMS_WHITE);
        DrawText(assignments[i].getRunwayNumber().c_str(), 360, y, 17, AMS_WHITE);
        DrawText(assignments[i].getAssignmentType().c_str(), 520, y, 17, AMS_LIGHT_BLUE);
        DrawText(assignments[i].getStatus().c_str(), 720, y, 17, AMS_GREEN);

        drawIcon(gateIcon, { 985, (float)y - 12, 45, 45 });

        y += 45;
    }
}

// ===================== DEPARTURE DISPLAY =====================

void AirportModule::drawDepartureDisplayPage(Texture2D logo, Texture2D background, Texture2D planeIcon) {
    drawBackground(background);
    drawHeader(logo, "Airport Departure Display");

    DrawText("Real Airport Style Departure Board", 60, 120, 26, AMS_YELLOW);

    vector<AirportFlightInfo> flights = getFlightsFromFlightModule();

    int y = 175;

    DrawText("Flight", 60, y, 18, AMS_YELLOW);
    DrawText("From", 180, y, 18, AMS_YELLOW);
    DrawText("To", 330, y, 18, AMS_YELLOW);
    DrawText("Departure", 500, y, 18, AMS_YELLOW);
    DrawText("Gate", 670, y, 18, AMS_YELLOW);
    DrawText("Runway", 790, y, 18, AMS_YELLOW);
    DrawText("Status", 930, y, 18, AMS_YELLOW);

    y += 35;

    for (int i = 0; i < (int)assignments.size() && y < 635; i++) {
        if (assignments[i].getAssignmentType() != "Departure") {
            continue;
        }

        AirportFlightInfo flight = getFlightInfo(assignments[i].getFlightNumber());

        if (flight.flightNumber == "") {
            continue;
        }

        DrawRectangleRounded({ 40, (float)y - 8, 1100, 38 }, 0.10f, 8, AMS_CARD);
        DrawAirportRoundedLines({ 40, (float)y - 8, 1100, 38 }, 0.10f, 8, 1.5f, AMS_BLUE);

        DrawText(flight.flightNumber.c_str(), 60, y, 16, AMS_WHITE);
        DrawText(flight.origin.c_str(), 180, y, 16, AMS_WHITE);
        DrawText(flight.destination.c_str(), 330, y, 16, AMS_WHITE);
        DrawText(flight.departureTime.c_str(), 500, y, 16, AMS_LIGHT_BLUE);
        DrawText(assignments[i].getGateNumber().c_str(), 670, y, 16, AMS_YELLOW);
        DrawText(assignments[i].getRunwayNumber().c_str(), 790, y, 16, AMS_YELLOW);
        DrawText(flight.status.c_str(), 930, y, 16, AMS_GREEN);

        drawIcon(planeIcon, { 1080, (float)y - 10, 45, 30 });

        y += 48;
    }
}

// ===================== ARRIVAL DISPLAY =====================

void AirportModule::drawArrivalDisplayPage(Texture2D logo, Texture2D background, Texture2D planeIcon) {
    drawBackground(background);
    drawHeader(logo, "Airport Arrival Display");

    DrawText("Real Airport Style Arrival Board", 60, 120, 26, AMS_YELLOW);

    int y = 175;

    DrawText("Flight", 60, y, 18, AMS_YELLOW);
    DrawText("From", 180, y, 18, AMS_YELLOW);
    DrawText("To", 330, y, 18, AMS_YELLOW);
    DrawText("Arrival", 500, y, 18, AMS_YELLOW);
    DrawText("Gate", 670, y, 18, AMS_YELLOW);
    DrawText("Runway", 790, y, 18, AMS_YELLOW);
    DrawText("Status", 930, y, 18, AMS_YELLOW);

    y += 35;

    for (int i = 0; i < (int)assignments.size() && y < 635; i++) {
        if (assignments[i].getAssignmentType() != "Arrival") {
            continue;
        }

        AirportFlightInfo flight = getFlightInfo(assignments[i].getFlightNumber());

        if (flight.flightNumber == "") {
            continue;
        }

        DrawRectangleRounded({ 40, (float)y - 8, 1100, 38 }, 0.10f, 8, AMS_CARD);
        DrawAirportRoundedLines({ 40, (float)y - 8, 1100, 38 }, 0.10f, 8, 1.5f, AMS_BLUE);

        DrawText(flight.flightNumber.c_str(), 60, y, 16, AMS_WHITE);
        DrawText(flight.origin.c_str(), 180, y, 16, AMS_WHITE);
        DrawText(flight.destination.c_str(), 330, y, 16, AMS_WHITE);
        DrawText(flight.arrivalTime.c_str(), 500, y, 16, AMS_LIGHT_BLUE);
        DrawText(assignments[i].getGateNumber().c_str(), 670, y, 16, AMS_YELLOW);
        DrawText(assignments[i].getRunwayNumber().c_str(), 790, y, 16, AMS_YELLOW);
        DrawText(flight.status.c_str(), 930, y, 16, AMS_GREEN);

        drawIcon(planeIcon, { 1080, (float)y - 10, 45, 30 });

        y += 48;
    }
}

// ===================== REPORT PAGE =====================

void AirportModule::drawReportPage(Texture2D logo, Texture2D background) {
    drawBackground(background);
    drawHeader(logo, "Airport Report");

    int totalAssignments = (int)assignments.size();
    int departures = 0;
    int arrivals = 0;

    for (int i = 0; i < (int)assignments.size(); i++) {
        if (assignments[i].getAssignmentType() == "Departure") {
            departures++;
        }
        else if (assignments[i].getAssignmentType() == "Arrival") {
            arrivals++;
        }
    }

    vector<AirportFlightInfo> flights = getFlightsFromFlightModule();
    int readyFlights = 0;

    for (int i = 0; i < (int)flights.size(); i++) {
        if (isActiveFlightStatus(flights[i].status)) {
            readyFlights++;
        }
    }

    DrawText("Airport Module Summary", 60, 140, 32, AMS_YELLOW);

    DrawRectangleRounded({ 60, 210, 650, 300 }, 0.14f, 12, AMS_CARD);
    DrawAirportRoundedLines({ 60, 210, 650, 300 }, 0.14f, 12, 2.0f, AMS_BLUE);

    DrawText(("Ready / Active Flights: " + to_string(readyFlights)).c_str(), 100, 260, 24, AMS_WHITE);
    DrawText(("Total Assignments: " + to_string(totalAssignments)).c_str(), 100, 310, 24, AMS_YELLOW);
    DrawText(("Departures: " + to_string(departures)).c_str(), 100, 360, 24, AMS_GREEN);
    DrawText(("Arrivals: " + to_string(arrivals)).c_str(), 100, 410, 24, AMS_LIGHT_BLUE);

    DrawText("Airport Module reads flights from flights.txt.", 60, 580, 19, Fade(AMS_WHITE, 0.80f));
    DrawText("Assignments are saved in airport_assignments.txt.", 60, 610, 19, Fade(AMS_WHITE, 0.80f));
}

// ===================== FLIGHT READING LOGIC =====================

vector<AirportFlightInfo> AirportModule::getFlightsFromFlightModule() {
    vector<AirportFlightInfo> flights;

    ifstream file("flights.txt");
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> data;
        string part;

        while (getline(ss, part, '|')) {
            data.push_back(part);
        }

        // flights.txt format:
        // flightNumber|origin|destination|departureTime|arrivalTime|status|aircraftID|gateNumber|pilotID|cabinCrewIDs|passengerIDs|delayMinutes|delayReason
        if ((int)data.size() >= 6) {
            AirportFlightInfo flight;

            flight.flightNumber = data[0];
            flight.origin = data[1];
            flight.destination = data[2];
            flight.departureTime = data[3];
            flight.arrivalTime = data[4];
            flight.status = data[5];

            flights.push_back(flight);
        }
    }

    file.close();
    return flights;
}

bool AirportModule::isActiveFlightStatus(string status) {
    if (status == "Ready To Fly") {
        return true;
    }

    if (status == "Scheduled") {
        return true;
    }

    if (status == "Boarding") {
        return true;
    }

    return false;
}

bool AirportModule::flightExistsAndReady(string flightNumber) {
    vector<AirportFlightInfo> flights = getFlightsFromFlightModule();

    for (int i = 0; i < (int)flights.size(); i++) {
        if (flights[i].flightNumber == flightNumber && isActiveFlightStatus(flights[i].status)) {
            return true;
        }
    }

    return false;
}

AirportFlightInfo AirportModule::getFlightInfo(string flightNumber) {
    vector<AirportFlightInfo> flights = getFlightsFromFlightModule();

    for (int i = 0; i < (int)flights.size(); i++) {
        if (flights[i].flightNumber == flightNumber) {
            return flights[i];
        }
    }

    AirportFlightInfo emptyFlight;
    emptyFlight.flightNumber = "";
    emptyFlight.origin = "";
    emptyFlight.destination = "";
    emptyFlight.departureTime = "";
    emptyFlight.arrivalTime = "";
    emptyFlight.status = "";

    return emptyFlight;
}

// ===================== GATE AND RUNWAY LOGIC =====================

vector<string> AirportModule::getDefaultGates() {
    vector<string> gates;

    gates.push_back("G1");
    gates.push_back("G2");
    gates.push_back("G3");
    gates.push_back("G4");
    gates.push_back("G5");

    return gates;
}

vector<string> AirportModule::getDefaultRunways() {
    vector<string> runways;

    runways.push_back("R1");
    runways.push_back("R2");
    runways.push_back("R3");

    return runways;
}

bool AirportModule::isGateBusy(string gateNumber) {
    for (int i = 0; i < (int)assignments.size(); i++) {
        if (assignments[i].getGateNumber() == gateNumber && assignments[i].getStatus() == "Assigned") {
            return true;
        }
    }

    return false;
}

bool AirportModule::isRunwayBusy(string runwayNumber) {
    for (int i = 0; i < (int)assignments.size(); i++) {
        if (assignments[i].getRunwayNumber() == runwayNumber && assignments[i].getStatus() == "Assigned") {
            return true;
        }
    }

    return false;
}

string AirportModule::getFreeGate(string preferredGate) {
    vector<string> gates = getDefaultGates();

    if (preferredGate != "" && !isGateBusy(preferredGate)) {
        return preferredGate;
    }

    for (int i = 0; i < (int)gates.size(); i++) {
        if (!isGateBusy(gates[i])) {
            return gates[i];
        }
    }

    return "";
}

string AirportModule::getFreeRunway(string preferredRunway) {
    vector<string> runways = getDefaultRunways();

    if (preferredRunway != "" && !isRunwayBusy(preferredRunway)) {
        return preferredRunway;
    }

    for (int i = 0; i < (int)runways.size(); i++) {
        if (!isRunwayBusy(runways[i])) {
            return runways[i];
        }
    }

    return "";
}

bool AirportModule::assignmentExists(string flightNumber) {
    for (int i = 0; i < (int)assignments.size(); i++) {
        if (assignments[i].getFlightNumber() == flightNumber) {
            return true;
        }
    }

    return false;
}

// ===================== FILE HANDLING =====================

void AirportModule::saveToFile() {
    ofstream file("airport_assignments.txt");

    for (int i = 0; i < (int)assignments.size(); i++) {
        file << assignments[i].serialize() << endl;
    }

    file.close();
}

void AirportModule::loadFromFile() {
    assignments.clear();

    ifstream file("airport_assignments.txt");

    if (!file) {
        return;
    }

    string line;

    while (getline(file, line)) {
        if (line.length() > 0) {
            AirportAssignment assignment;
            assignment.deserialize(line);
            assignments.push_back(assignment);
        }
    }

    file.close();
}

// ===================== RESET =====================

void AirportModule::resetInputs() {
    inputFlightNumber = "";
    inputGateNumber = "";
    inputRunwayNumber = "";
    inputAssignmentType = "Departure";

    activeFlightNumber = false;
    activeGateNumber = false;
    activeRunwayNumber = false;
    activeAssignmentType = false;
}