#include "Flight.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;

// ===================== RAYLIB COMPATIBILITY FIX =====================

static void DrawFlightRoundedLines(Rectangle rec, float roundness, int segments, float lineThick, Color color) {
    DrawRectangleRoundedLines(rec, roundness, segments, color);
}

// ===================== THEME COLORS =====================

static Color FMS_NAVY = { 4, 18, 38, 255 };
static Color FMS_TOP_NAVY = { 8, 28, 58, 255 };
static Color FMS_BLUE = { 25, 118, 210, 255 };
static Color FMS_LIGHT_BLUE = { 83, 169, 255, 255 };
static Color FMS_YELLOW = { 255, 193, 7, 255 };
static Color FMS_WHITE = { 245, 248, 255, 255 };
static Color FMS_CARD = { 16, 42, 82, 245 };
static Color FMS_RED = { 230, 57, 70, 255 };
static Color FMS_GREEN = { 46, 204, 113, 255 };

// ===================== LOCAL STRUCTS =====================

struct LocalPassengerManifest {
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

struct FlightLoadInfo {
    int passengerCount;
    float averagePassengerWeight;
    float passengerWeight;
    float baggageWeight;
    float totalWeight;
    float maxAllowedWeight;
    bool overweight;
    string status;
    string source;
};

// ===================== SAFE FUNCTIONS =====================

static int safeToIntFlightLocal(string text) {
    if (text == "") {
        return 0;
    }

    for (int i = 0; i < (int)text.length(); i++) {
        if (text[i] < '0' || text[i] > '9') {
            return 0;
        }
    }

    return stoi(text);
}

static float safeToFloatFlightLocal(string text) {
    if (text == "") {
        return 0;
    }

    bool dotUsed = false;

    for (int i = 0; i < (int)text.length(); i++) {
        if (text[i] == '.') {
            if (dotUsed) {
                return 0;
            }

            dotUsed = true;
        }
        else if (text[i] < '0' || text[i] > '9') {
            return 0;
        }
    }

    return stof(text);
}

static int countFileLinesFlightLocal(string fileName) {
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

static string generateNextFlightIDLocal() {
    int nextNumber = countFileLinesFlightLocal("flights.txt") + 1;

    stringstream ss;
    ss << "FI" << setw(4) << setfill('0') << nextNumber;

    return ss.str();
}

// ===================== BASIC FILE CHECKS =====================

static bool aircraftExistsFlightLocal(string aircraftID) {
    ifstream file("aircraft.txt");
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string id;
        getline(ss, id, '|');

        if (id == aircraftID) {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

static bool pilotExistsFlightLocal(string pilotID) {
    ifstream file("pilot.txt");
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string id;
        getline(ss, id, '|');

        if (id == pilotID) {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

// ===================== AUTOMATIC PILOT AND CREW READING =====================

static string shortenFlightTextLocal(string text, int maxLength) {
    if ((int)text.length() <= maxLength) {
        return text;
    }

    return text.substr(0, maxLength - 3) + "...";
}

static string getAssignedPilotTextLocal(string flightNumber, string oldPilotID) {
    ifstream file("pilot_assignments.txt");
    string line;
    string latestPilot = "";

    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> data;
        string part;

        while (getline(ss, part, '|')) {
            data.push_back(part);
        }

        // Format:
        // flightNumber|pilotID|fullName|rank|licenseType|aircraftType|plannedHours
        if ((int)data.size() >= 3 && data[0] == flightNumber) {
            latestPilot = data[1] + " - " + data[2];
        }
    }

    file.close();

    if (latestPilot != "") {
        return "Pilot: " + latestPilot;
    }

    if (oldPilotID != "" && oldPilotID != "Not Assigned") {
        return "Pilot ID: " + oldPilotID;
    }

    return "Pilot: Not Assigned";
}

static int getAssignedCrewCountLocal(string flightNumber) {
    ifstream file("cabincrew_assignments.txt");
    string line;
    int count = 0;

    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> data;
        string part;

        while (getline(ss, part, '|')) {
            data.push_back(part);
        }

        // Expected:
        // flightNumber|crewID|fullName|role
        if ((int)data.size() >= 2 && data[0] == flightNumber) {
            count++;
        }
    }

    file.close();
    return count;
}

static string getAssignedCrewTextLocal(string flightNumber, string oldCrewIDs) {
    ifstream file("cabincrew_assignments.txt");
    string line;
    vector<string> crewList;

    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> data;
        string part;

        while (getline(ss, part, '|')) {
            data.push_back(part);
        }

        // Expected:
        // flightNumber|crewID|fullName|role
        if ((int)data.size() >= 3 && data[0] == flightNumber) {
            crewList.push_back(data[1] + " - " + data[2]);
        }
        else if ((int)data.size() >= 2 && data[0] == flightNumber) {
            crewList.push_back(data[1]);
        }
    }

    file.close();

    if (!crewList.empty()) {
        string text = "Crew: " + to_string((int)crewList.size()) + " assigned | ";

        for (int i = 0; i < (int)crewList.size(); i++) {
            text += crewList[i];

            if (i != (int)crewList.size() - 1) {
                text += ", ";
            }
        }

        return shortenFlightTextLocal(text, 58);
    }

    if (oldCrewIDs != "" && oldCrewIDs != "Not Assigned") {
        return "Crew IDs: " + oldCrewIDs;
    }

    return "Crew: Not Assigned";
}

// ===================== PASSENGER / BAGGAGE READING =====================

static vector<LocalPassengerManifest> getPassengersOfFlightLocal(string flightNumber) {
    vector<LocalPassengerManifest> passengers;

    ifstream file("flight_passengers.txt");
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> data;
        string part;

        while (getline(ss, part, '|')) {
            data.push_back(part);
        }

        // flightNumber|passengerID|fullName|passportNumber|phoneNumber|email|seatNumber|mealPreference|ticketStatus
        if ((int)data.size() >= 9 && data[0] == flightNumber) {
            LocalPassengerManifest passenger;

            passenger.flightNumber = data[0];
            passenger.passengerID = data[1];
            passenger.fullName = data[2];
            passenger.passportNumber = data[3];
            passenger.phoneNumber = data[4];
            passenger.email = data[5];
            passenger.seatNumber = data[6];
            passenger.mealPreference = data[7];
            passenger.ticketStatus = data[8];

            passengers.push_back(passenger);
        }
    }

    file.close();
    return passengers;
}

static int getPassengerCountForFlightLocal(string flightNumber) {
    vector<LocalPassengerManifest> passengers = getPassengersOfFlightLocal(flightNumber);
    return (int)passengers.size();
}

static float getTotalBaggageWeightForFlightLocal(string flightNumber) {
    ifstream file("baggage.txt");
    string line;
    float totalWeight = 0;

    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> data;
        string part;

        while (getline(ss, part, '|')) {
            data.push_back(part);
        }

        // bagID|flightNumber|passengerID|passengerName|weightKg|fee|status|complaint
        if ((int)data.size() >= 5 && data[1] == flightNumber) {
            totalWeight += safeToFloatFlightLocal(data[4]);
        }
    }

    file.close();
    return totalWeight;
}

static FlightLoadInfo getFlightLoadInfoLocal(string flightNumber) {
    FlightLoadInfo info;

    info.passengerCount = getPassengerCountForFlightLocal(flightNumber);
    info.averagePassengerWeight = 75.0f;
    info.passengerWeight = info.passengerCount * info.averagePassengerWeight;
    info.baggageWeight = getTotalBaggageWeightForFlightLocal(flightNumber);
    info.totalWeight = info.passengerWeight + info.baggageWeight;

    info.maxAllowedWeight = 8000.0f;
    info.overweight = info.totalWeight > info.maxAllowedWeight;
    info.status = info.overweight ? "NOT ELIGIBLE - OVERWEIGHT" : "ELIGIBLE / READY TO GO";
    info.source = "Passenger + Baggage Files";

    ifstream file("weight_balance.txt");
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> data;
        string part;

        while (getline(ss, part, '|')) {
            data.push_back(part);
        }

        // calculationID|flightNumber|aircraftID|aircraftType|emptyWeight|maxTakeoffWeight|passengerCount|averagePassengerWeight|baggageWeight|cargoWeight|fuelWeight|totalPassengerWeight|totalLoad|status
        if ((int)data.size() >= 14 && data[1] == flightNumber) {
            info.maxAllowedWeight = safeToFloatFlightLocal(data[5]);
            info.passengerCount = safeToIntFlightLocal(data[6]);
            info.averagePassengerWeight = safeToFloatFlightLocal(data[7]);
            info.baggageWeight = safeToFloatFlightLocal(data[8]);
            info.passengerWeight = safeToFloatFlightLocal(data[11]);
            info.totalWeight = safeToFloatFlightLocal(data[12]);
            info.status = data[13];
            info.overweight = info.totalWeight > info.maxAllowedWeight;
            info.source = "WeightBalance Final Result";
        }
    }

    file.close();
    return info;
}

// ===================== FLIGHT CLASS =====================

Flight::Flight() {
    flightNumber = "";
    origin = "";
    destination = "";
    departureTime = "";
    arrivalTime = "";
    status = "Scheduled";

    aircraftID = "Not Assigned";
    gateNumber = "Not Assigned";
    pilotID = "Not Assigned";
    cabinCrewIDs = "Not Assigned";
    passengerIDs = "Linked through flight_passengers.txt";

    delayMinutes = 0;
    delayReason = "None";
}

Flight::Flight(string flightNumber, string origin, string destination, string departureTime, string arrivalTime) {
    this->flightNumber = flightNumber;
    this->origin = origin;
    this->destination = destination;
    this->departureTime = departureTime;
    this->arrivalTime = arrivalTime;

    status = "Scheduled";

    aircraftID = "Not Assigned";
    gateNumber = "Not Assigned";
    pilotID = "Not Assigned";
    cabinCrewIDs = "Not Assigned";
    passengerIDs = "Linked through flight_passengers.txt";

    delayMinutes = 0;
    delayReason = "None";
}

string Flight::getFlightNumber() const {
    return flightNumber;
}

string Flight::getOrigin() const {
    return origin;
}

string Flight::getDestination() const {
    return destination;
}

string Flight::getDepartureTime() const {
    return departureTime;
}

string Flight::getArrivalTime() const {
    return arrivalTime;
}

string Flight::getStatus() const {
    return status;
}

string Flight::getAircraftID() const {
    return aircraftID;
}

string Flight::getGateNumber() const {
    return gateNumber;
}

string Flight::getPilotID() const {
    return pilotID;
}

string Flight::getCabinCrewIDs() const {
    return cabinCrewIDs;
}

string Flight::getPassengerIDs() const {
    return passengerIDs;
}

int Flight::getDelayMinutes() const {
    return delayMinutes;
}

string Flight::getDelayReason() const {
    return delayReason;
}

void Flight::setOrigin(string value) {
    origin = value;
}

void Flight::setDestination(string value) {
    destination = value;
}

void Flight::setDepartureTime(string value) {
    departureTime = value;
}

void Flight::setArrivalTime(string value) {
    arrivalTime = value;
}

void Flight::updateStatus(string newStatus) {
    status = newStatus;
}

void Flight::applyDelay(int minutes, string reason) {
    if (minutes <= 0) {
        return;
    }

    delayMinutes += minutes;
    delayReason = reason;
    status = "Delayed";
}

void Flight::connectAircraft(string id) {
    aircraftID = id;
}

void Flight::connectGate(string gate) {
    gateNumber = gate;
}

void Flight::connectPilot(string id) {
    pilotID = id;
}

void Flight::connectCabinCrew(string ids) {
    cabinCrewIDs = ids;
}

void Flight::connectPassengers(string ids) {
    passengerIDs = ids;
}

string Flight::serialize() const {
    stringstream ss;

    ss << flightNumber << "|"
       << origin << "|"
       << destination << "|"
       << departureTime << "|"
       << arrivalTime << "|"
       << status << "|"
       << aircraftID << "|"
       << gateNumber << "|"
       << pilotID << "|"
       << cabinCrewIDs << "|"
       << passengerIDs << "|"
       << delayMinutes << "|"
       << delayReason;

    return ss.str();
}

void Flight::deserialize(string line) {
    stringstream ss(line);
    vector<string> data;
    string part;

    while (getline(ss, part, '|')) {
        data.push_back(part);
    }

    if ((int)data.size() < 13) {
        return;
    }

    flightNumber = data[0];
    origin = data[1];
    destination = data[2];
    departureTime = data[3];
    arrivalTime = data[4];
    status = data[5];

    aircraftID = data[6];
    gateNumber = data[7];
    pilotID = data[8];
    cabinCrewIDs = data[9];
    passengerIDs = data[10];

    delayMinutes = safeToIntFlightLocal(data[11]);
    delayReason = data[12];
}

// ===================== FLIGHT MODULE =====================

FlightModule::FlightModule() {
    currentPage = MENU;

    message = "";
    messageColor = FMS_WHITE;

    selectedFlightIndex = -1;

    activeFlightNumber = false;
    activeOrigin = false;
    activeDestination = false;
    activeDeparture = false;
    activeArrival = false;
    activeStatus = false;

    activeAircraftID = false;
    activeGateNumber = false;
    activePilotID = false;
    activeCabinCrewIDs = false;
    activePassengerIDs = false;

    activeDelayMinutes = false;
    activeDelayReason = false;

    loadFromFile();
}

bool FlightModule::Draw(Texture2D logo, Texture2D background, Texture2D plane, Texture2D warningIcon) {
    if (currentPage == MENU) {
        drawMenu(logo, background, plane, warningIcon);
    }
    else if (currentPage == ADD) {
        drawAddPage(logo, background, plane);
    }
    else if (currentPage == VIEW) {
        drawViewPage(logo, background, plane);
    }
    else if (currentPage == SEARCH) {
        drawSearchPage(logo, background, plane);
    }
    else if (currentPage == UPDATE_STATUS) {
        drawUpdateStatusPage(logo, background);
    }
    else if (currentPage == AUTO_DELAY) {
        drawAutoDelayPage(logo, background, warningIcon);
    }
    else if (currentPage == CONNECT_RELATIONS) {
        drawConnectRelationsPage(logo, background, plane);
    }
    else if (currentPage == DELETE_FLIGHT) {
        drawDeletePage(logo, background);
    }
    else if (currentPage == REPORT) {
        drawReportPage(logo, background);
    }

    if (button({ 40, 665, 170, 52 }, "BACK", FMS_YELLOW, { 255, 210, 70, 255 }, FMS_NAVY)) {
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

void FlightModule::drawBackground(Texture2D background) {
    ClearBackground(FMS_NAVY);

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
        Fade(FMS_TOP_NAVY, 0.94f),
        Fade(FMS_NAVY, 0.98f)
    );
}

void FlightModule::drawHeader(Texture2D logo, const char* title) {
    DrawRectangle(0, 0, 1200, 96, FMS_TOP_NAVY);
    DrawRectangle(0, 93, 1200, 3, FMS_YELLOW);

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

    DrawFlightRoundedLines({ 20, 12, 80, 80 }, 0.12f, 8, 2.0f, FMS_YELLOW);
    DrawText(title, 125, 33, 32, FMS_WHITE);
}

bool FlightModule::button(Rectangle rect, const char* text, Color color, Color hoverColor, Color textColor) {
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, rect);

    DrawRectangleRounded(rect, 0.20f, 12, hover ? hoverColor : color);
    DrawFlightRoundedLines(rect, 0.20f, 12, 2.0f, FMS_YELLOW);

    int fontSize = 18;
    int textWidth = MeasureText(text, fontSize);

    int textX = (int)(rect.x + rect.width / 2 - textWidth / 2);
    int textY = (int)(rect.y + rect.height / 2 - fontSize / 2);

    DrawText(text, textX, textY, fontSize, textColor);

    return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void FlightModule::textBox(Rectangle rect, string& text, bool& active, const char* label) {
    DrawText(label, (int)rect.x, (int)rect.y - 24, 18, FMS_WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        active = CheckCollisionPointRec(GetMousePosition(), rect);
    }

    DrawRectangleRounded(rect, 0.15f, 10, FMS_WHITE);
    DrawFlightRoundedLines(rect, 0.15f, 10, 2.0f, active ? FMS_YELLOW : FMS_BLUE);

    DrawText(text.c_str(), (int)rect.x + 12, (int)rect.y + 14, 20, FMS_NAVY);

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

void FlightModule::drawIcon(Texture2D texture, Rectangle rect) {
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

void FlightModule::drawFlightCard(Flight flight, Rectangle rect, Texture2D plane) {
    FlightLoadInfo loadInfo = getFlightLoadInfoLocal(flight.getFlightNumber());

    DrawRectangleRounded(rect, 0.12f, 12, FMS_CARD);

    Color borderColor = loadInfo.overweight ? FMS_RED : FMS_GREEN;
    DrawFlightRoundedLines(rect, 0.12f, 12, 2.0f, borderColor);

    drawIcon(plane, { rect.x + 18, rect.y + 35, 100, 60 });

    DrawText(flight.getFlightNumber().c_str(), (int)rect.x + 135, (int)rect.y + 10, 23, FMS_YELLOW);

    string route = flight.getOrigin() + " to " + flight.getDestination();
    DrawText(route.c_str(), (int)rect.x + 135, (int)rect.y + 40, 17, FMS_WHITE);

    string timeLine = "Departure: " + flight.getDepartureTime() + " | Arrival: " + flight.getArrivalTime();
    DrawText(timeLine.c_str(), (int)rect.x + 135, (int)rect.y + 66, 15, FMS_LIGHT_BLUE);

    string statusLine = "Status: " + flight.getStatus() + " | Delay: " + to_string(flight.getDelayMinutes()) + " min";
    DrawText(statusLine.c_str(), (int)rect.x + 135, (int)rect.y + 92, 15, FMS_WHITE);

    string pilotLine = getAssignedPilotTextLocal(flight.getFlightNumber(), flight.getPilotID());
    DrawText(pilotLine.c_str(), (int)rect.x + 500, (int)rect.y + 16, 15, FMS_WHITE);

    string crewLine = getAssignedCrewTextLocal(flight.getFlightNumber(), flight.getCabinCrewIDs());
    DrawText(crewLine.c_str(), (int)rect.x + 500, (int)rect.y + 42, 14, FMS_LIGHT_BLUE);

    string passengerLine = "Passengers: " + to_string(loadInfo.passengerCount);
    DrawText(passengerLine.c_str(), (int)rect.x + 500, (int)rect.y + 70, 15, FMS_YELLOW);

    string passengerWeightLine = "Passenger Weight: " + to_string((int)loadInfo.passengerWeight) + " kg";
    DrawText(passengerWeightLine.c_str(), (int)rect.x + 500, (int)rect.y + 96, 15, FMS_WHITE);

    string baggageLine = "Baggage: " + to_string((int)loadInfo.baggageWeight) + " kg";
    DrawText(baggageLine.c_str(), (int)rect.x + 820, (int)rect.y + 16, 15, FMS_WHITE);

    string totalLine = "Total: " + to_string((int)loadInfo.totalWeight) + " / " + to_string((int)loadInfo.maxAllowedWeight) + " kg";
    DrawText(totalLine.c_str(), (int)rect.x + 820, (int)rect.y + 42, 15, FMS_YELLOW);

    DrawText(
        loadInfo.status.c_str(),
        (int)rect.x + 820,
        (int)rect.y + 70,
        15,
        loadInfo.overweight ? FMS_RED : FMS_GREEN
    );

    DrawText(loadInfo.source.c_str(), (int)rect.x + 820, (int)rect.y + 96, 13, FMS_LIGHT_BLUE);
}

// ===================== MENU PAGE =====================

void FlightModule::drawMenu(Texture2D logo, Texture2D background, Texture2D plane, Texture2D warningIcon) {
    drawBackground(background);
    drawHeader(logo, "Flight Management");

    DrawText(
        "Control flight journey, live status, delays, aircraft, pilot, cabin crew, passengers, and weight eligibility.",
        55,
        125,
        19,
        Fade(FMS_WHITE, 0.85f)
    );

    vector<string> options = {
        "Add Flight",
        "View Flights",
        "Search Flight",
        "Update Status",
        "Auto Delay",
        "Connect Relations",
        "Delete Flight",
        "Flight Report"
    };

    int startX = 55;
    int startY = 185;
    int w = 330;
    int h = 58;
    int gapX = 40;
    int gapY = 28;

    for (int i = 0; i < (int)options.size(); i++) {
        int row = i / 3;
        int col = i % 3;

        Rectangle rect = {
            (float)(startX + col * (w + gapX)),
            (float)(startY + row * (h + gapY)),
            (float)w,
            (float)h
        };

        if (button(rect, options[i].c_str(), FMS_BLUE, FMS_LIGHT_BLUE, FMS_WHITE)) {
            currentPage = (Page)(i + 1);
            message = "";
            resetInputs();
        }
    }

    Rectangle infoBox = { 640, 520, 500, 135 };

    DrawRectangleRounded(infoBox, 0.16f, 12, FMS_CARD);
    DrawFlightRoundedLines(infoBox, 0.16f, 12, 2.0f, FMS_YELLOW);

    drawIcon(plane, { infoBox.x + 30, infoBox.y + 38, 100, 60 });
    drawIcon(warningIcon, { infoBox.x + 155, infoBox.y + 40, 48, 48 });

    DrawText("Connected Flight Control", (int)infoBox.x + 235, (int)infoBox.y + 25, 19, FMS_YELLOW);
    DrawText("Reads pilot, cabin crew,", (int)infoBox.x + 235, (int)infoBox.y + 62, 15, FMS_WHITE);
    DrawText("passenger and baggage files.", (int)infoBox.x + 235, (int)infoBox.y + 90, 15, FMS_WHITE);
}

// ===================== ADD FLIGHT PAGE =====================

void FlightModule::drawAddPage(Texture2D logo, Texture2D background, Texture2D plane) {
    drawBackground(background);
    drawHeader(logo, "Add Flight");

    if (inputFlightNumber == "") {
        inputFlightNumber = generateNextFlightIDLocal();
    }

    drawIcon(plane, { 870, 145, 185, 120 });

    textBox({ 60, 160, 300, 50 }, inputFlightNumber, activeFlightNumber, "Flight Number Auto");
    textBox({ 400, 160, 300, 50 }, inputOrigin, activeOrigin, "Origin");

    textBox({ 60, 270, 300, 50 }, inputDestination, activeDestination, "Destination");
    textBox({ 400, 270, 300, 50 }, inputDeparture, activeDeparture, "Departure Time");

    textBox({ 60, 380, 300, 50 }, inputArrival, activeArrival, "Arrival Time");

    if (button({ 60, 510, 220, 55 }, "SAVE FLIGHT", FMS_GREEN, { 70, 220, 140, 255 }, FMS_WHITE)) {
        if (
            inputFlightNumber == "" ||
            inputOrigin == "" ||
            inputDestination == "" ||
            inputDeparture == "" ||
            inputArrival == ""
        ) {
            message = "Please fill all flight fields.";
            messageColor = FMS_RED;
        }
        else if (findFlight(inputFlightNumber) != -1) {
            message = "Flight number already exists.";
            messageColor = FMS_RED;
        }
        else {
            Flight flight(inputFlightNumber, inputOrigin, inputDestination, inputDeparture, inputArrival);

            flights.push_back(flight);
            saveToFile();

            message = "Flight added successfully.";
            messageColor = FMS_GREEN;
            resetInputs();
        }
    }

    DrawText(message.c_str(), 320, 528, 20, messageColor);
}

// ===================== VIEW PAGE =====================

void FlightModule::drawViewPage(Texture2D logo, Texture2D background, Texture2D plane) {
    drawBackground(background);
    drawHeader(logo, "View Flights");

    DrawText("Flight Records With Pilot, Cabin Crew, Passenger & Weight Status", 60, 120, 23, FMS_YELLOW);

    if (flights.empty()) {
        DrawText("No flight records found.", 60, 185, 22, FMS_WHITE);
        return;
    }

    int y = 155;

    for (int i = 0; i < (int)flights.size() && i < 3; i++) {
        drawFlightCard(flights[i], { 60, (float)y, 1080, 145 }, plane);
        y += 158;
    }

    if ((int)flights.size() > 3) {
        DrawText("Only first 3 records are shown because each flight displays connected details.", 60, 650, 18, FMS_YELLOW);
    }
}

// ===================== SEARCH PAGE =====================

void FlightModule::drawSearchPage(Texture2D logo, Texture2D background, Texture2D plane) {
    drawBackground(background);
    drawHeader(logo, "Search Flight");

    textBox({ 60, 130, 320, 50 }, inputFlightNumber, activeFlightNumber, "Enter Flight Number");

    if (button({ 410, 130, 170, 50 }, "SEARCH", FMS_BLUE, FMS_LIGHT_BLUE, FMS_WHITE)) {
        selectedFlightIndex = findFlight(inputFlightNumber);

        if (selectedFlightIndex == -1) {
            message = "Flight not found.";
            messageColor = FMS_RED;
        }
        else {
            message = "Flight found.";
            messageColor = FMS_GREEN;
        }
    }

    DrawText(message.c_str(), 610, 145, 20, messageColor);

    if (selectedFlightIndex != -1) {
        Flight selectedFlight = flights[selectedFlightIndex];

        drawFlightCard(selectedFlight, { 60, 205, 1080, 145 }, plane);

        vector<LocalPassengerManifest> manifest = getPassengersOfFlightLocal(selectedFlight.getFlightNumber());

        DrawText("Passenger Details From Passenger Module", 60, 380, 23, FMS_YELLOW);

        if (manifest.empty()) {
            DrawText("No passengers booked for this flight yet.", 60, 420, 18, FMS_WHITE);
        }
        else {
            int y = 420;

            DrawText("ID", 60, y, 16, FMS_YELLOW);
            DrawText("Name", 140, y, 16, FMS_YELLOW);
            DrawText("Passport", 360, y, 16, FMS_YELLOW);
            DrawText("Phone", 520, y, 16, FMS_YELLOW);
            DrawText("Seat", 690, y, 16, FMS_YELLOW);
            DrawText("Meal", 770, y, 16, FMS_YELLOW);
            DrawText("Ticket", 920, y, 16, FMS_YELLOW);

            y += 30;

            for (int i = 0; i < (int)manifest.size() && i < 8; i++) {
                DrawText(manifest[i].passengerID.c_str(), 60, y, 15, FMS_WHITE);
                DrawText(manifest[i].fullName.c_str(), 140, y, 15, FMS_WHITE);
                DrawText(manifest[i].passportNumber.c_str(), 360, y, 15, FMS_WHITE);
                DrawText(manifest[i].phoneNumber.c_str(), 520, y, 15, FMS_WHITE);
                DrawText(manifest[i].seatNumber.c_str(), 690, y, 15, FMS_WHITE);
                DrawText(manifest[i].mealPreference.c_str(), 770, y, 15, FMS_WHITE);
                DrawText(manifest[i].ticketStatus.c_str(), 920, y, 15, FMS_WHITE);

                y += 26;
            }
        }
    }
}

// ===================== UPDATE STATUS PAGE =====================

void FlightModule::drawUpdateStatusPage(Texture2D logo, Texture2D background) {
    drawBackground(background);
    drawHeader(logo, "Update Flight Status");

    DrawText("Allowed status examples: Scheduled, Boarding, Ready To Fly, Delayed, Departed, Landed, Cancelled", 60, 125, 18, Fade(FMS_WHITE, 0.85f));

    textBox({ 60, 190, 320, 50 }, inputFlightNumber, activeFlightNumber, "Flight Number");
    textBox({ 420, 190, 320, 50 }, inputStatus, activeStatus, "New Status");

    if (button({ 60, 315, 220, 55 }, "UPDATE STATUS", FMS_GREEN, { 70, 220, 140, 255 }, FMS_WHITE)) {
        int index = findFlight(inputFlightNumber);

        if (index == -1) {
            message = "Flight not found.";
            messageColor = FMS_RED;
        }
        else if (inputStatus == "") {
            message = "Please enter new status.";
            messageColor = FMS_RED;
        }
        else {
            flights[index].updateStatus(inputStatus);
            saveToFile();

            message = "Flight status updated successfully.";
            messageColor = FMS_GREEN;
            resetInputs();
        }
    }

    DrawText(message.c_str(), 320, 333, 20, messageColor);
}

// ===================== AUTO DELAY PAGE =====================

void FlightModule::drawAutoDelayPage(Texture2D logo, Texture2D background, Texture2D warningIcon) {
    drawBackground(background);
    drawHeader(logo, "Auto Delay Flight");

    drawIcon(warningIcon, { 850, 145, 150, 150 });

    DrawText("Use this option when weather or technical issues delay a flight.", 60, 125, 20, Fade(FMS_WHITE, 0.86f));

    textBox({ 60, 190, 320, 50 }, inputFlightNumber, activeFlightNumber, "Flight Number");
    textBox({ 420, 190, 260, 50 }, inputDelayMinutes, activeDelayMinutes, "Delay Minutes");
    textBox({ 60, 300, 500, 50 }, inputDelayReason, activeDelayReason, "Reason");

    if (button({ 60, 420, 220, 55 }, "APPLY DELAY", FMS_RED, { 255, 90, 100, 255 }, FMS_WHITE)) {
        int index = findFlight(inputFlightNumber);

        if (index == -1) {
            message = "Flight not found.";
            messageColor = FMS_RED;
        }
        else if (inputDelayMinutes == "" || inputDelayReason == "") {
            message = "Please enter delay minutes and reason.";
            messageColor = FMS_RED;
        }
        else {
            flights[index].applyDelay(safeToInt(inputDelayMinutes), inputDelayReason);
            saveToFile();

            message = "Flight delayed successfully.";
            messageColor = FMS_GREEN;
            resetInputs();
        }
    }

    DrawText(message.c_str(), 320, 438, 20, messageColor);
}

// ===================== CONNECT RELATIONS PAGE =====================

void FlightModule::drawConnectRelationsPage(Texture2D logo, Texture2D background, Texture2D plane) {
    drawBackground(background);
    drawHeader(logo, "Connect Flight Relations");

    drawIcon(plane, { 875, 145, 170, 110 });

    DrawText("This page can still manually connect aircraft, gate, pilot and cabin crew.", 60, 125, 19, Fade(FMS_WHITE, 0.86f));
    DrawText("But View Flight automatically reads pilot_assignments.txt and cabincrew_assignments.txt.", 60, 150, 18, FMS_YELLOW);

    textBox({ 60, 200, 300, 50 }, inputFlightNumber, activeFlightNumber, "Flight Number");
    textBox({ 400, 200, 300, 50 }, inputAircraftID, activeAircraftID, "Aircraft ID");

    textBox({ 60, 310, 300, 50 }, inputGateNumber, activeGateNumber, "Gate Number");
    textBox({ 400, 310, 300, 50 }, inputPilotID, activePilotID, "Pilot ID");

    textBox({ 60, 420, 640, 50 }, inputCabinCrewIDs, activeCabinCrewIDs, "Cabin Crew IDs");

    if (button({ 60, 535, 250, 55 }, "CONNECT DATA", FMS_GREEN, { 70, 220, 140, 255 }, FMS_WHITE)) {
        int index = findFlight(inputFlightNumber);

        if (index == -1) {
            message = "Flight not found.";
            messageColor = FMS_RED;
        }
        else if (inputAircraftID == "" || inputGateNumber == "" || inputPilotID == "" || inputCabinCrewIDs == "") {
            message = "Please fill aircraft, gate, pilot, and cabin crew fields.";
            messageColor = FMS_RED;
        }
        else if (!aircraftExistsFlightLocal(inputAircraftID)) {
            message = "Aircraft ID does not exist in aircraft.txt.";
            messageColor = FMS_RED;
        }
        else if (!pilotExistsFlightLocal(inputPilotID)) {
            message = "Pilot ID does not exist in pilot.txt.";
            messageColor = FMS_RED;
        }
        else {
            flights[index].connectAircraft(inputAircraftID);
            flights[index].connectGate(inputGateNumber);
            flights[index].connectPilot(inputPilotID);
            flights[index].connectCabinCrew(inputCabinCrewIDs);
            flights[index].connectPassengers("Linked through flight_passengers.txt");

            saveToFile();

            message = "Flight relations connected successfully.";
            messageColor = FMS_GREEN;
            resetInputs();
        }
    }

    DrawText(message.c_str(), 340, 553, 20, messageColor);
}

// ===================== DELETE PAGE =====================

void FlightModule::drawDeletePage(Texture2D logo, Texture2D background) {
    drawBackground(background);
    drawHeader(logo, "Delete Flight");

    textBox({ 60, 170, 320, 50 }, inputFlightNumber, activeFlightNumber, "Flight Number");

    if (button({ 410, 170, 180, 50 }, "LOAD", FMS_BLUE, FMS_LIGHT_BLUE, FMS_WHITE)) {
        selectedFlightIndex = findFlight(inputFlightNumber);

        if (selectedFlightIndex == -1) {
            message = "Flight not found.";
            messageColor = FMS_RED;
        }
        else {
            message = "Flight loaded. Press delete to confirm.";
            messageColor = FMS_YELLOW;
        }
    }

    if (selectedFlightIndex != -1) {
        DrawRectangleRounded({ 60, 280, 850, 120 }, 0.12f, 12, FMS_CARD);
        DrawFlightRoundedLines({ 60, 280, 850, 120 }, 0.12f, 12, 2.0f, FMS_RED);

        Flight flight = flights[selectedFlightIndex];

        DrawText(("Flight: " + flight.getFlightNumber()).c_str(), 90, 310, 24, FMS_YELLOW);
        DrawText((flight.getOrigin() + " to " + flight.getDestination()).c_str(), 90, 345, 20, FMS_WHITE);
        DrawText(("Status: " + flight.getStatus()).c_str(), 90, 375, 18, FMS_LIGHT_BLUE);

        if (button({ 60, 455, 220, 55 }, "DELETE FLIGHT", FMS_RED, { 255, 90, 100, 255 }, FMS_WHITE)) {
            flights.erase(flights.begin() + selectedFlightIndex);
            saveToFile();

            selectedFlightIndex = -1;
            message = "Flight deleted successfully.";
            messageColor = FMS_GREEN;
            resetInputs();
        }
    }

    DrawText(message.c_str(), 60, 560, 22, messageColor);
}

// ===================== REPORT PAGE =====================

void FlightModule::drawReportPage(Texture2D logo, Texture2D background) {
    drawBackground(background);
    drawHeader(logo, "Flight Report");

    int total = (int)flights.size();
    int scheduled = 0;
    int boarding = 0;
    int delayed = 0;
    int departed = 0;
    int landed = 0;
    int cancelled = 0;
    int eligible = 0;
    int notEligible = 0;

    int flightsWithPilot = 0;
    int flightsWithCrew = 0;

    for (int i = 0; i < (int)flights.size(); i++) {
        string status = flights[i].getStatus();

        if (status == "Scheduled") scheduled++;
        else if (status == "Boarding") boarding++;
        else if (status == "Delayed") delayed++;
        else if (status == "Departed") departed++;
        else if (status == "Landed") landed++;
        else if (status == "Cancelled") cancelled++;

        FlightLoadInfo loadInfo = getFlightLoadInfoLocal(flights[i].getFlightNumber());

        if (loadInfo.overweight) {
            notEligible++;
        }
        else {
            eligible++;
        }

        string pilotText = getAssignedPilotTextLocal(flights[i].getFlightNumber(), flights[i].getPilotID());
        string crewText = getAssignedCrewTextLocal(flights[i].getFlightNumber(), flights[i].getCabinCrewIDs());

        if (pilotText != "Pilot: Not Assigned") {
            flightsWithPilot++;
        }

        if (crewText != "Crew: Not Assigned") {
            flightsWithCrew++;
        }
    }

    DrawText("Flight Operations Summary", 60, 140, 32, FMS_YELLOW);

    DrawRectangleRounded({ 60, 210, 680, 410 }, 0.14f, 12, FMS_CARD);
    DrawFlightRoundedLines({ 60, 210, 680, 410 }, 0.14f, 12, 2.0f, FMS_BLUE);

    DrawText(("Total Flights: " + to_string(total)).c_str(), 100, 245, 22, FMS_WHITE);
    DrawText(("Scheduled: " + to_string(scheduled)).c_str(), 100, 280, 20, FMS_WHITE);
    DrawText(("Boarding: " + to_string(boarding)).c_str(), 100, 312, 20, FMS_LIGHT_BLUE);
    DrawText(("Delayed: " + to_string(delayed)).c_str(), 100, 344, 20, FMS_RED);
    DrawText(("Departed: " + to_string(departed)).c_str(), 100, 376, 20, FMS_GREEN);
    DrawText(("Landed: " + to_string(landed)).c_str(), 100, 408, 20, FMS_GREEN);
    DrawText(("Cancelled: " + to_string(cancelled)).c_str(), 100, 440, 20, FMS_RED);

    DrawText(("Flights With Pilot Assigned: " + to_string(flightsWithPilot)).c_str(), 100, 490, 21, FMS_YELLOW);
    DrawText(("Flights With Cabin Crew Assigned: " + to_string(flightsWithCrew)).c_str(), 100, 525, 21, FMS_LIGHT_BLUE);

    DrawText(("Eligible / Ready To Go: " + to_string(eligible)).c_str(), 100, 565, 21, FMS_GREEN);
    DrawText(("Not Eligible / Overweight: " + to_string(notEligible)).c_str(), 100, 595, 21, FMS_RED);
}

// ===================== FILE HANDLING =====================

void FlightModule::saveToFile() {
    ofstream file("flights.txt");

    for (int i = 0; i < (int)flights.size(); i++) {
        file << flights[i].serialize() << endl;
    }

    file.close();
}

void FlightModule::loadFromFile() {
    flights.clear();

    ifstream file("flights.txt");

    if (!file) {
        return;
    }

    string line;

    while (getline(file, line)) {
        if (line.length() > 0) {
            Flight flight;
            flight.deserialize(line);
            flights.push_back(flight);
        }
    }

    file.close();
}

// ===================== HELPERS =====================

void FlightModule::resetInputs() {
    inputFlightNumber = "";
    inputOrigin = "";
    inputDestination = "";
    inputDeparture = "";
    inputArrival = "";
    inputStatus = "";

    inputAircraftID = "";
    inputGateNumber = "";
    inputPilotID = "";
    inputCabinCrewIDs = "";
    inputPassengerIDs = "";

    inputDelayMinutes = "";
    inputDelayReason = "";

    activeFlightNumber = false;
    activeOrigin = false;
    activeDestination = false;
    activeDeparture = false;
    activeArrival = false;
    activeStatus = false;

    activeAircraftID = false;
    activeGateNumber = false;
    activePilotID = false;
    activeCabinCrewIDs = false;
    activePassengerIDs = false;

    activeDelayMinutes = false;
    activeDelayReason = false;

    selectedFlightIndex = -1;
}

int FlightModule::findFlight(string flightNumber) {
    for (int i = 0; i < (int)flights.size(); i++) {
        if (flights[i].getFlightNumber() == flightNumber) {
            return i;
        }
    }

    return -1;
}

int FlightModule::safeToInt(string text) {
    return safeToIntFlightLocal(text);
}