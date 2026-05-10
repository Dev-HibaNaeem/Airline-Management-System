#include "Passenger.h"
#include "SystemUtils.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;

// ===================== RAYLIB COMPATIBILITY FIX =====================

static void DrawRoundedLinesEx(Rectangle rec, float roundness, int segments, float lineThick, Color color) {
    DrawRectangleRoundedLines(rec, roundness, segments, color);
}

// ===================== THEME COLORS =====================

Color PMS_NAVY = { 4, 18, 38, 255 };
Color PMS_TOP_NAVY = { 8, 28, 58, 255 };
Color PMS_BLUE = { 25, 118, 210, 255 };
Color PMS_LIGHT_BLUE = { 83, 169, 255, 255 };
Color PMS_YELLOW = { 255, 193, 7, 255 };
Color PMS_WHITE = { 245, 248, 255, 255 };
Color PMS_CARD = { 16, 42, 82, 245 };
Color PMS_RED = { 230, 57, 70, 255 };
Color PMS_GREEN = { 46, 204, 113, 255 };

// ===================== LOCAL HELPERS =====================

static float safeToFloatPassengerLocal(string text) {
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

static int safeToIntPassengerLocal(string text) {
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

static int getMaxSeatsForFlightLocal(string flightNumber) {
    ifstream file("flight_capacity.txt");
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> data;
        string part;

        while (getline(ss, part, '|')) {
            data.push_back(part);
        }

        if ((int)data.size() >= 2 && data[0] == flightNumber) {
            file.close();
            return safeToIntPassengerLocal(data[1]);
        }
    }

    file.close();

    // Default capacity if flight_capacity.txt is not available
    return 60;
}

static bool isSeatOccupiedLocal(string flightNumber, string seatNumber) {
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
        if ((int)data.size() >= 7) {
            if (data[0] == flightNumber && data[6] == seatNumber) {
                file.close();
                return true;
            }
        }
    }

    file.close();
    return false;
}

static vector<string> getAvailableSeatsForFlightLocal(string flightNumber) {
    vector<string> seats;

    int maxSeats = getMaxSeatsForFlightLocal(flightNumber);

    if (maxSeats <= 0) {
        maxSeats = 60;
    }

    string seatLetters[4] = { "A", "B", "C", "D" };

    int seatCounter = 0;
    int row = 1;

    while (seatCounter < maxSeats) {
        for (int i = 0; i < 4 && seatCounter < maxSeats; i++) {
            string seatNumber = to_string(row) + seatLetters[i];

            if (!isSeatOccupiedLocal(flightNumber, seatNumber)) {
                seats.push_back(seatNumber);
            }

            seatCounter++;
        }

        row++;
    }

    return seats;
}

static int getBaggageCountForPassengerLocal(string passengerID) {
    ifstream file("baggage.txt");
    string line;
    int count = 0;

    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> data;
        string part;

        while (getline(ss, part, '|')) {
            data.push_back(part);
        }

        // bagID|flightNumber|passengerID|passengerName|weightKg|fee|status|complaint
        if ((int)data.size() >= 3 && data[2] == passengerID) {
            count++;
        }
    }

    file.close();
    return count;
}

static float getTotalBaggageWeightForPassengerLocal(string passengerID) {
    ifstream file("baggage.txt");
    string line;
    float total = 0;

    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> data;
        string part;

        while (getline(ss, part, '|')) {
            data.push_back(part);
        }

        if ((int)data.size() >= 5 && data[2] == passengerID) {
            total += safeToFloatPassengerLocal(data[4]);
        }
    }

    file.close();
    return total;
}

static string generateNextBaggageIDLocal() {
    int count = countFileLines("baggage.txt") + 1;

    stringstream ss;
    ss << "BG" << setw(4) << setfill('0') << count;

    return ss.str();
}

static float calculateOverweightFeeLocal(float weight) {
    if (weight <= 30) {
        return 0;
    }

    return (weight - 30) * 500;
}

// ===================== PASSENGER BAGGAGE CLASS =====================

PassengerBaggage::PassengerBaggage() {
    bagTag = "";
    weight = 0;
}

PassengerBaggage::PassengerBaggage(string bagTag, float weight) {
    this->bagTag = bagTag;
    this->weight = weight;
}

string PassengerBaggage::getBagTag() const {
    return bagTag;
}

float PassengerBaggage::getWeight() const {
    return weight;
}

string PassengerBaggage::serialize() const {
    stringstream ss;
    ss << bagTag << "," << weight;
    return ss.str();
}

void PassengerBaggage::deserialize(string text) {
    stringstream ss(text);
    string part;

    getline(ss, bagTag, ',');
    getline(ss, part, ',');

    weight = safeToFloatPassengerLocal(part);
}

// ===================== PASSENGER BOOKING CLASS =====================

PassengerBooking::PassengerBooking() {
    flightNumber = "";
    route = "";
    seatNumber = "";
    mealPreference = "";
    ticketStatus = "";
}

PassengerBooking::PassengerBooking(string flightNumber, string route, string seatNumber, string mealPreference, string ticketStatus) {
    this->flightNumber = flightNumber;
    this->route = route;
    this->seatNumber = seatNumber;
    this->mealPreference = mealPreference;
    this->ticketStatus = ticketStatus;
}

string PassengerBooking::getFlightNumber() const {
    return flightNumber;
}

string PassengerBooking::getRoute() const {
    return route;
}

string PassengerBooking::getSeatNumber() const {
    return seatNumber;
}

string PassengerBooking::getMealPreference() const {
    return mealPreference;
}

string PassengerBooking::getTicketStatus() const {
    return ticketStatus;
}

string PassengerBooking::serialize() const {
    stringstream ss;
    ss << flightNumber << "," << route << "," << seatNumber << "," << mealPreference << "," << ticketStatus;
    return ss.str();
}

void PassengerBooking::deserialize(string text) {
    stringstream ss(text);

    getline(ss, flightNumber, ',');
    getline(ss, route, ',');
    getline(ss, seatNumber, ',');
    getline(ss, mealPreference, ',');
    getline(ss, ticketStatus, ',');
}

// ===================== PASSENGER RECORD CLASS =====================

PassengerRecord::PassengerRecord() {
    passengerID = "";
    fullName = "";
    passportNumber = "";
    nationality = "";
    phoneNumber = "";
    email = "";
}

PassengerRecord::PassengerRecord(string passengerID, string fullName, string passportNumber, string nationality, string phoneNumber, string email) {
    this->passengerID = passengerID;
    this->fullName = fullName;
    this->passportNumber = passportNumber;
    this->nationality = nationality;
    this->phoneNumber = phoneNumber;
    this->email = email;
}

string PassengerRecord::getPassengerID() const {
    return passengerID;
}

string PassengerRecord::getFullName() const {
    return fullName;
}

string PassengerRecord::getPassportNumber() const {
    return passportNumber;
}

string PassengerRecord::getNationality() const {
    return nationality;
}

string PassengerRecord::getPhoneNumber() const {
    return phoneNumber;
}

string PassengerRecord::getEmail() const {
    return email;
}

vector<PassengerBooking> PassengerRecord::getBookings() const {
    return bookings;
}

vector<PassengerBaggage> PassengerRecord::getBaggageList() const {
    return baggageList;
}

void PassengerRecord::setFullName(string value) {
    fullName = value;
}

void PassengerRecord::setPassportNumber(string value) {
    passportNumber = value;
}

void PassengerRecord::setNationality(string value) {
    nationality = value;
}

void PassengerRecord::setPhoneNumber(string value) {
    phoneNumber = value;
}

void PassengerRecord::setEmail(string value) {
    email = value;
}

void PassengerRecord::addBooking(PassengerBooking booking) {
    bookings.push_back(booking);
}

void PassengerRecord::addBaggage(PassengerBaggage baggage) {
    baggageList.push_back(baggage);
}

string PassengerRecord::serialize() const {
    stringstream ss;

    ss << passengerID << "|"
       << fullName << "|"
       << passportNumber << "|"
       << nationality << "|"
       << phoneNumber << "|"
       << email;

    return ss.str();
}

void PassengerRecord::deserialize(string line) {
    stringstream ss(line);
    string part;
    vector<string> data;

    while (getline(ss, part, '|')) {
        data.push_back(part);
    }

    if ((int)data.size() < 6) {
        return;
    }

    passengerID = data[0];
    fullName = data[1];
    passportNumber = data[2];
    nationality = data[3];
    phoneNumber = data[4];
    email = data[5];
}

// ===================== PASSENGER MODULE =====================

PassengerModule::PassengerModule() {
    currentPage = MENU;

    message = "";
    messageColor = PMS_WHITE;

    selectedPassengerIndex = -1;

    activeID = false;
    activeName = false;
    activePassport = false;
    activeNationality = false;
    activePhone = false;
    activeEmail = false;

    activeFlight = false;
    activeRoute = false;
    activeSeat = false;
    activeMeal = false;
    activeTicketStatus = false;

    activeBagTag = false;
    activeBagWeight = false;

    loadFromFile();
}

bool PassengerModule::Draw(PassengerAssets assets, Texture2D logo, Texture2D background) {
    if (currentPage == MENU) {
        drawMenu(assets, logo, background);
    }
    else if (currentPage == ADD) {
        drawAddPage(assets, logo, background);
    }
    else if (currentPage == VIEW) {
        drawViewPage(assets, logo, background);
    }
    else if (currentPage == SEARCH) {
        drawSearchPage(assets, logo, background);
    }
    else if (currentPage == UPDATE) {
        drawUpdatePage(assets, logo, background);
    }
    else if (currentPage == BOOK_FLIGHT) {
        drawBookFlightPage(assets, logo, background);
    }
    else if (currentPage == ADD_BAGGAGE) {
        drawAddBaggagePage(assets, logo, background);
    }
    else if (currentPage == REPORT) {
        drawReportPage(assets, logo, background);
    }

    if (button({ 40, 665, 170, 52 }, "BACK", PMS_YELLOW, { 255, 210, 70, 255 }, PMS_NAVY)) {
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

void PassengerModule::drawBackground(Texture2D background) {
    ClearBackground(PMS_NAVY);

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
        Fade(PMS_TOP_NAVY, 0.94f),
        Fade(PMS_NAVY, 0.98f)
    );
}

void PassengerModule::drawHeader(Texture2D logo, const char* title) {
    DrawRectangle(0, 0, 1200, 96, PMS_TOP_NAVY);
    DrawRectangle(0, 93, 1200, 3, PMS_YELLOW);

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

    DrawRoundedLinesEx({ 20, 12, 80, 80 }, 0.12f, 8, 2.0f, PMS_YELLOW);
    DrawText(title, 125, 33, 32, PMS_WHITE);
}

bool PassengerModule::button(Rectangle rect, const char* text, Color color, Color hoverColor, Color textColor) {
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, rect);

    DrawRectangleRounded(rect, 0.20f, 12, hover ? hoverColor : color);
    DrawRoundedLinesEx(rect, 0.20f, 12, 2.0f, PMS_YELLOW);

    int fontSize = 18;
    int textWidth = MeasureText(text, fontSize);

    int textX = (int)(rect.x + rect.width / 2 - textWidth / 2);
    int textY = (int)(rect.y + rect.height / 2 - fontSize / 2);

    DrawText(text, textX, textY, fontSize, textColor);

    return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void PassengerModule::textBox(Rectangle rect, string& text, bool& active, const char* label) {
    DrawText(label, (int)rect.x, (int)rect.y - 24, 18, PMS_WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        active = CheckCollisionPointRec(GetMousePosition(), rect);
    }

    DrawRectangleRounded(rect, 0.15f, 10, PMS_WHITE);
    DrawRoundedLinesEx(rect, 0.15f, 10, 2.0f, active ? PMS_YELLOW : PMS_BLUE);

    DrawText(text.c_str(), (int)rect.x + 12, (int)rect.y + 14, 20, PMS_NAVY);

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

void PassengerModule::drawIcon(Texture2D texture, Rectangle rect) {
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

void PassengerModule::drawPassengerCard(PassengerAssets assets, PassengerRecord passenger, Rectangle rect) {
    DrawRectangleRounded(rect, 0.12f, 12, PMS_CARD);
    DrawRoundedLinesEx(rect, 0.12f, 12, 2.0f, PMS_BLUE);

    drawIcon(assets.passengerIcon, { rect.x + 20, rect.y + 25, 70, 70 });

    DrawText(passenger.getPassengerID().c_str(), (int)rect.x + 110, (int)rect.y + 15, 24, PMS_YELLOW);

    string nameLine = "Name: " + passenger.getFullName();
    DrawText(nameLine.c_str(), (int)rect.x + 110, (int)rect.y + 47, 19, PMS_WHITE);

    string passportLine =
        "Passport: " + passenger.getPassportNumber() +
        " | Nationality: " + passenger.getNationality();

    DrawText(passportLine.c_str(), (int)rect.x + 110, (int)rect.y + 75, 17, PMS_LIGHT_BLUE);

    string contactLine =
        "Phone: " + passenger.getPhoneNumber() +
        " | Email: " + passenger.getEmail();

    DrawText(contactLine.c_str(), (int)rect.x + 110, (int)rect.y + 102, 16, PMS_WHITE);

    int bookingCount = 0;

    ifstream manifest("flight_passengers.txt");
    string line;

    while (getline(manifest, line)) {
        stringstream ss(line);
        vector<string> data;
        string part;

        while (getline(ss, part, '|')) {
            data.push_back(part);
        }

        if ((int)data.size() >= 2 && data[1] == passenger.getPassengerID()) {
            bookingCount++;
        }
    }

    manifest.close();

    int baggageCount = getBaggageCountForPassengerLocal(passenger.getPassengerID());
    float baggageWeight = getTotalBaggageWeightForPassengerLocal(passenger.getPassengerID());

    string bookingLine = "Bookings: " + to_string(bookingCount);
    DrawText(bookingLine.c_str(), (int)rect.x + 700, (int)rect.y + 35, 18, PMS_YELLOW);

    string baggageLine =
        "Baggage: " + to_string(baggageCount) +
        " bags | " + to_string((int)baggageWeight) + " kg";

    DrawText(baggageLine.c_str(), (int)rect.x + 700, (int)rect.y + 70, 18, PMS_GREEN);

    drawIcon(assets.baggage, { rect.x + rect.width - 90, rect.y + 35, 55, 55 });
}

// ===================== MENU PAGE =====================

void PassengerModule::drawMenu(PassengerAssets assets, Texture2D logo, Texture2D background) {
    drawBackground(background);
    drawHeader(logo, "Passenger Management");

    DrawText(
        "Manage passengers, bookings, seats, baggage, meals, and flight connections.",
        55,
        125,
        20,
        Fade(PMS_WHITE, 0.85f)
    );

    vector<string> options = {
        "Add Passenger",
        "View Passengers",
        "Search Passenger",
        "Update Passenger",
        "Book Flight",
        "Add Baggage",
        "Passenger Report"
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

        if (button(rect, options[i].c_str(), PMS_BLUE, PMS_LIGHT_BLUE, PMS_WHITE)) {
            currentPage = (Page)(i + 1);
            message = "";
            resetInputs();
        }
    }

  Rectangle infoBox = { 640, 520, 500, 135 };

DrawRectangleRounded(infoBox, 0.16f, 12, PMS_CARD);
DrawRoundedLinesEx(infoBox, 0.16f, 12, 2.0f, PMS_YELLOW);

drawIcon(assets.passengerIcon, { infoBox.x + 25, infoBox.y + 35, 55, 55 });
drawIcon(assets.baggage, { infoBox.x + 95, infoBox.y + 38, 48, 48 });
drawIcon(assets.ticket, { infoBox.x + 160, infoBox.y + 38, 48, 48 });

DrawText("Connected Passenger System", (int)infoBox.x + 230, (int)infoBox.y + 25, 18, PMS_YELLOW);
DrawText("Booking writes into", (int)infoBox.x + 230, (int)infoBox.y + 60, 15, PMS_WHITE);
DrawText("flight_passengers.txt", (int)infoBox.x + 230, (int)infoBox.y + 88, 15, PMS_WHITE);

    DrawRectangleRounded(infoBox, 0.16f, 12, PMS_CARD);
    DrawRoundedLinesEx(infoBox, 0.16f, 12, 2.0f, PMS_YELLOW);

    drawIcon(assets.passengerIcon, { infoBox.x + 20, infoBox.y + 23, 60, 60 });
    drawIcon(assets.baggage, { infoBox.x + 100, infoBox.y + 28, 52, 52 });
    drawIcon(assets.ticket, { infoBox.x + 170, infoBox.y + 28, 52, 52 });

    DrawText("Connected Passenger System", (int)infoBox.x + 235, (int)infoBox.y + 22, 18, PMS_YELLOW);
    DrawText("Booking writes into", (int)infoBox.x + 235, (int)infoBox.y + 58, 16, PMS_WHITE);
    DrawText("flight_passengers.txt", (int)infoBox.x + 235, (int)infoBox.y + 84, 16, PMS_WHITE);
}

// ===================== ADD PAGE =====================

void PassengerModule::drawAddPage(PassengerAssets assets, Texture2D logo, Texture2D background) {
    drawBackground(background);
    drawHeader(logo, "Add Passenger");

    if (inputID == "") {
        inputID = generateNextID("PI", "passengers.txt");
    }

    drawIcon(assets.passengerIcon, { 880, 140, 140, 140 });

    textBox({ 60, 160, 300, 50 }, inputID, activeID, "Passenger ID Auto");
    textBox({ 400, 160, 300, 50 }, inputName, activeName, "Full Name");

    textBox({ 60, 270, 300, 50 }, inputPassport, activePassport, "Passport Number");
    textBox({ 400, 270, 300, 50 }, inputNationality, activeNationality, "Nationality");

    textBox({ 60, 380, 300, 50 }, inputPhone, activePhone, "Phone Number");
    textBox({ 400, 380, 300, 50 }, inputEmail, activeEmail, "Email");

    if (button({ 60, 510, 230, 55 }, "SAVE PASSENGER", PMS_GREEN, { 70, 220, 140, 255 }, PMS_WHITE)) {
        if (
            inputID == "" ||
            inputName == "" ||
            inputPassport == "" ||
            inputNationality == "" ||
            inputPhone == "" ||
            inputEmail == ""
        ) {
            message = "Please fill all passenger fields.";
            messageColor = PMS_RED;
        }
        else if (findPassenger(inputID) != -1) {
            message = "Passenger ID already exists.";
            messageColor = PMS_RED;
        }
        else {
            PassengerRecord passenger(
                inputID,
                inputName,
                inputPassport,
                inputNationality,
                inputPhone,
                inputEmail
            );

            passengers.push_back(passenger);
            saveToFile();

            message = "Passenger added successfully.";
            messageColor = PMS_GREEN;
            resetInputs();
        }
    }

    DrawText(message.c_str(), 320, 528, 20, messageColor);
}

// ===================== VIEW PAGE =====================

void PassengerModule::drawViewPage(PassengerAssets assets, Texture2D logo, Texture2D background) {
    drawBackground(background);
    drawHeader(logo, "View Passengers");

    DrawText("Passenger Records With Booking And Baggage Details", 60, 120, 24, PMS_YELLOW);

    if (passengers.empty()) {
        DrawText("No passenger records found.", 60, 185, 22, PMS_WHITE);
        return;
    }

    int y = 155;

    for (int i = 0; i < (int)passengers.size() && i < 4; i++) {
        drawPassengerCard(assets, passengers[i], { 60, (float)y, 1080, 130 });
        y += 142;
    }

    if ((int)passengers.size() > 4) {
        DrawText(
            "Only first 4 passengers are shown because each card displays booking and baggage details.",
            60,
            690,
            18,
            PMS_YELLOW
        );
    }
}

// ===================== SEARCH PAGE =====================

void PassengerModule::drawSearchPage(PassengerAssets assets, Texture2D logo, Texture2D background) {
    drawBackground(background);
    drawHeader(logo, "Search Passenger");

    textBox({ 60, 145, 320, 50 }, inputID, activeID, "Enter Passenger ID");

    if (button({ 410, 145, 170, 50 }, "SEARCH", PMS_BLUE, PMS_LIGHT_BLUE, PMS_WHITE)) {
        selectedPassengerIndex = findPassenger(inputID);

        if (selectedPassengerIndex == -1) {
            message = "Passenger not found.";
            messageColor = PMS_RED;
        }
        else {
            message = "Passenger found.";
            messageColor = PMS_GREEN;
        }
    }

    DrawText(message.c_str(), 610, 160, 20, messageColor);

    if (selectedPassengerIndex != -1) {
        PassengerRecord passenger = passengers[selectedPassengerIndex];

        drawPassengerCard(assets, passenger, { 60, 230, 1080, 130 });

        DrawText("Bookings From flight_passengers.txt", 60, 390, 23, PMS_YELLOW);

        ifstream file("flight_passengers.txt");
        string line;
        int y = 430;
        bool foundBooking = false;

        while (getline(file, line)) {
            stringstream ss(line);
            vector<string> data;
            string part;

            while (getline(ss, part, '|')) {
                data.push_back(part);
            }

            if ((int)data.size() >= 9 && data[1] == passenger.getPassengerID()) {
                foundBooking = true;

                string bookingLine =
                    "Flight: " + data[0] +
                    " | Seat: " + data[6] +
                    " | Meal: " + data[7] +
                    " | Ticket: " + data[8];

                DrawText(bookingLine.c_str(), 60, y, 17, PMS_WHITE);
                y += 28;
            }
        }

        file.close();

        if (!foundBooking) {
            DrawText("No bookings found for this passenger.", 60, 430, 18, PMS_WHITE);
        }
    }
}

// ===================== UPDATE PAGE =====================

void PassengerModule::drawUpdatePage(PassengerAssets assets, Texture2D logo, Texture2D background) {
    drawBackground(background);
    drawHeader(logo, "Update Passenger");

    DrawText("Enter passenger ID, load record, then update fields.", 60, 125, 18, PMS_YELLOW);

    textBox({ 60, 170, 300, 50 }, inputID, activeID, "Passenger ID");

    if (button({ 400, 170, 170, 50 }, "LOAD", PMS_BLUE, PMS_LIGHT_BLUE, PMS_WHITE)) {
        selectedPassengerIndex = findPassenger(inputID);

        if (selectedPassengerIndex == -1) {
            message = "Passenger not found.";
            messageColor = PMS_RED;
        }
        else {
            inputName = passengers[selectedPassengerIndex].getFullName();
            inputPassport = passengers[selectedPassengerIndex].getPassportNumber();
            inputNationality = passengers[selectedPassengerIndex].getNationality();
            inputPhone = passengers[selectedPassengerIndex].getPhoneNumber();
            inputEmail = passengers[selectedPassengerIndex].getEmail();

            message = "Passenger loaded. Edit fields and save.";
            messageColor = PMS_GREEN;
        }
    }

    if (selectedPassengerIndex != -1) {
        textBox({ 60, 280, 300, 50 }, inputName, activeName, "Full Name");
        textBox({ 400, 280, 300, 50 }, inputPassport, activePassport, "Passport");

        textBox({ 60, 390, 300, 50 }, inputNationality, activeNationality, "Nationality");
        textBox({ 400, 390, 300, 50 }, inputPhone, activePhone, "Phone");

        textBox({ 60, 500, 300, 50 }, inputEmail, activeEmail, "Email");

        if (button({ 400, 500, 210, 55 }, "SAVE UPDATE", PMS_GREEN, { 70, 220, 140, 255 }, PMS_WHITE)) {
            passengers[selectedPassengerIndex].setFullName(inputName);
            passengers[selectedPassengerIndex].setPassportNumber(inputPassport);
            passengers[selectedPassengerIndex].setNationality(inputNationality);
            passengers[selectedPassengerIndex].setPhoneNumber(inputPhone);
            passengers[selectedPassengerIndex].setEmail(inputEmail);

            saveToFile();

            message = "Passenger updated successfully.";
            messageColor = PMS_GREEN;
            resetInputs();
        }
    }

    DrawText(message.c_str(), 60, 610, 20, messageColor);
}

// ===================== BOOK FLIGHT PAGE =====================

void PassengerModule::drawBookFlightPage(PassengerAssets assets, Texture2D logo, Texture2D background) {
    drawBackground(background);
    drawHeader(logo, "Book Passenger Flight");

    drawIcon(assets.ticket, { 890, 130, 135, 135 });

    DrawText("Booking is connected with Flight Module using flight_passengers.txt.", 60, 115, 18, PMS_YELLOW);
    DrawText("Enter Flight ID first. Occupied seats are hidden automatically.", 60, 140, 18, PMS_WHITE);

    textBox({ 60, 190, 300, 50 }, inputID, activeID, "Passenger ID");
    textBox({ 400, 190, 300, 50 }, inputFlight, activeFlight, "Flight Number");

    textBox({ 60, 300, 300, 50 }, inputRoute, activeRoute, "Route");
    textBox({ 400, 300, 300, 50 }, inputMeal, activeMeal, "Meal Preference");

    if (inputTicketStatus == "") {
        inputTicketStatus = "Confirmed";
    }

    DrawText("Ticket Status", 60, 390, 18, PMS_WHITE);
    DrawRectangleRounded({ 60, 415, 300, 50 }, 0.15f, 10, PMS_WHITE);
    DrawRoundedLinesEx({ 60, 415, 300, 50 }, 0.15f, 10, 2.0f, PMS_BLUE);
    DrawText(inputTicketStatus.c_str(), 75, 429, 20, PMS_NAVY);

    DrawText("Available Seats", 400, 390, 18, PMS_WHITE);

    if (inputFlight == "") {
        DrawText("Enter Flight Number first to show seats.", 400, 425, 18, PMS_YELLOW);
    }
    else if (!flightExistsInFile(inputFlight)) {
        DrawText("Flight ID not found in flights.txt.", 400, 425, 18, PMS_RED);
    }
    else {
        vector<string> availableSeats = getAvailableSeatsForFlightLocal(inputFlight);

        int maxSeats = getMaxSeatsForFlightLocal(inputFlight);
        int occupiedSeats = maxSeats - (int)availableSeats.size();

        string seatInfo =
            "Max Seats: " + to_string(maxSeats) +
            " | Occupied: " + to_string(occupiedSeats) +
            " | Available: " + to_string((int)availableSeats.size());

        DrawText(seatInfo.c_str(), 400, 415, 16, PMS_YELLOW);

        if (availableSeats.empty()) {
            DrawText("No seats available for this flight.", 400, 445, 18, PMS_RED);
        }
        else {
            int startX = 400;
            int startY = 445;

            int seatW = 58;
            int seatH = 34;
            int gap = 8;

            for (int i = 0; i < (int)availableSeats.size() && i < 24; i++) {
                int row = i / 6;
                int col = i % 6;

                Rectangle seatRect = {
                    (float)(startX + col * (seatW + gap)),
                    (float)(startY + row * (seatH + gap)),
                    (float)seatW,
                    (float)seatH
                };

                Color seatColor = PMS_BLUE;

                if (inputSeat == availableSeats[i]) {
                    seatColor = PMS_GREEN;
                }

                if (button(seatRect, availableSeats[i].c_str(), seatColor, PMS_LIGHT_BLUE, PMS_WHITE)) {
                    inputSeat = availableSeats[i];
                }
            }

            if ((int)availableSeats.size() > 24) {
                DrawText("Only first 24 available seats are shown.", 400, 625, 16, PMS_YELLOW);
            }
        }
    }

    string selectedSeatLine = "Selected Seat: " + inputSeat;
    DrawText(selectedSeatLine.c_str(), 60, 485, 20, inputSeat == "" ? PMS_RED : PMS_GREEN);

    if (button({ 60, 545, 230, 55 }, "SAVE BOOKING", PMS_GREEN, { 70, 220, 140, 255 }, PMS_WHITE)) {
        int index = findPassenger(inputID);

        if (index == -1) {
            message = "Passenger not found.";
            messageColor = PMS_RED;
        }
        else if (inputFlight == "" || inputRoute == "" || inputMeal == "" || inputTicketStatus == "") {
            message = "Please fill flight, route, meal, and ticket status.";
            messageColor = PMS_RED;
        }
        else if (!flightExistsInFile(inputFlight)) {
            message = "Flight number does not exist in flights.txt.";
            messageColor = PMS_RED;
        }
        else if (inputSeat == "") {
            message = "Please select an available seat.";
            messageColor = PMS_RED;
        }
        else if (isSeatOccupiedLocal(inputFlight, inputSeat)) {
            message = "This seat is already occupied. Please select another seat.";
            messageColor = PMS_RED;
            inputSeat = "";
        }
        else if (passengerAlreadyInFlight(inputFlight, inputID)) {
            message = "Passenger is already booked on this flight.";
            messageColor = PMS_RED;
        }
        else {
            PassengerBooking booking(
                inputFlight,
                inputRoute,
                inputSeat,
                inputMeal,
                inputTicketStatus
            );

            passengers[index].addBooking(booking);
            saveToFile();

            PassengerBasicInfo passengerInfo = findPassengerInFile(inputID);

            addPassengerToFlightManifest(
                inputFlight,
                passengerInfo,
                inputSeat,
                inputMeal,
                inputTicketStatus
            );

            message = "Booking saved. Passenger is now linked with Flight Module.";
            messageColor = PMS_GREEN;

            resetInputs();
        }
    }

    DrawText(message.c_str(), 310, 562, 20, messageColor);
}

// ===================== ADD BAGGAGE PAGE =====================

void PassengerModule::drawAddBaggagePage(PassengerAssets assets, Texture2D logo, Texture2D background) {
    drawBackground(background);
    drawHeader(logo, "Add Passenger Baggage");

    drawIcon(assets.baggage, { 890, 140, 130, 130 });

    DrawText("Baggage is saved in baggage.txt and shown in Passenger + Flight modules.", 60, 120, 18, PMS_YELLOW);

    if (inputBagTag == "") {
        inputBagTag = generateNextBaggageIDLocal();
    }

    textBox({ 60, 180, 300, 50 }, inputBagTag, activeBagTag, "Bag ID Auto");
    textBox({ 400, 180, 300, 50 }, inputID, activeID, "Passenger ID");

    textBox({ 60, 290, 300, 50 }, inputFlight, activeFlight, "Flight Number");
    textBox({ 400, 290, 300, 50 }, inputBagWeight, activeBagWeight, "Baggage Weight KG");

    if (inputFlight != "" && !flightExistsInFile(inputFlight)) {
        DrawText("Flight ID not found in flights.txt.", 60, 365, 18, PMS_RED);
    }

    if (inputID != "" && findPassenger(inputID) == -1) {
        DrawText("Passenger ID not found.", 400, 365, 18, PMS_RED);
    }

    if (button({ 60, 430, 230, 55 }, "SAVE BAGGAGE", PMS_GREEN, { 70, 220, 140, 255 }, PMS_WHITE)) {
        int index = findPassenger(inputID);
        float weight = safeToFloatPassengerLocal(inputBagWeight);

        if (index == -1) {
            message = "Passenger not found.";
            messageColor = PMS_RED;
        }
        else if (inputFlight == "" || inputBagWeight == "") {
            message = "Please enter flight number and baggage weight.";
            messageColor = PMS_RED;
        }
        else if (!flightExistsInFile(inputFlight)) {
            message = "Flight number does not exist in flights.txt.";
            messageColor = PMS_RED;
        }
        else if (weight <= 0) {
            message = "Please enter valid baggage weight.";
            messageColor = PMS_RED;
        }
        else {
            string passengerName = passengers[index].getFullName();
            float fee = calculateOverweightFeeLocal(weight);

            ofstream file("baggage.txt", ios::app);

            file << inputBagTag << "|"
                 << inputFlight << "|"
                 << inputID << "|"
                 << passengerName << "|"
                 << weight << "|"
                 << fee << "|"
                 << "Checked-In" << "|"
                 << "None" << endl;

            file.close();

            passengers[index].addBaggage(PassengerBaggage(inputBagTag, weight));
            saveToFile();

            message = "Baggage saved and linked with Passenger + Flight Module.";
            messageColor = PMS_GREEN;

            resetInputs();
        }
    }

    DrawText(message.c_str(), 320, 448, 20, messageColor);
}

// ===================== REPORT PAGE =====================

void PassengerModule::drawReportPage(PassengerAssets assets, Texture2D logo, Texture2D background) {
    drawBackground(background);
    drawHeader(logo, "Passenger Report");

    int totalPassengers = (int)passengers.size();

    int totalBookings = 0;
    ifstream manifest("flight_passengers.txt");
    string line;

    while (getline(manifest, line)) {
        if (line.length() > 0) {
            totalBookings++;
        }
    }

    manifest.close();

    int totalBaggage = 0;
    float totalBaggageWeight = 0;

    ifstream baggageFile("baggage.txt");

    while (getline(baggageFile, line)) {
        stringstream ss(line);
        vector<string> data;
        string part;

        while (getline(ss, part, '|')) {
            data.push_back(part);
        }

        if ((int)data.size() >= 5) {
            totalBaggage++;
            totalBaggageWeight += safeToFloatPassengerLocal(data[4]);
        }
    }

    baggageFile.close();

    DrawText("Passenger Module Summary", 60, 140, 32, PMS_YELLOW);

    DrawRectangleRounded({ 60, 210, 650, 330 }, 0.14f, 12, PMS_CARD);
    DrawRoundedLinesEx({ 60, 210, 650, 330 }, 0.14f, 12, 2.0f, PMS_BLUE);

    DrawText(("Total Passengers: " + to_string(totalPassengers)).c_str(), 100, 260, 24, PMS_WHITE);
    DrawText(("Total Bookings: " + to_string(totalBookings)).c_str(), 100, 310, 24, PMS_YELLOW);
    DrawText(("Total Baggage Records: " + to_string(totalBaggage)).c_str(), 100, 360, 24, PMS_WHITE);
    DrawText(("Total Baggage Weight: " + to_string((int)totalBaggageWeight) + " kg").c_str(), 100, 410, 24, PMS_GREEN);

    DrawText(
        "Bookings are saved in flight_passengers.txt and baggage is saved in baggage.txt.",
        60,
        590,
        19,
        Fade(PMS_WHITE, 0.80f)
    );
}

// ===================== FILE HANDLING =====================

void PassengerModule::saveToFile() {
    ofstream file("passengers.txt");

    for (int i = 0; i < (int)passengers.size(); i++) {
        file << passengers[i].serialize() << endl;
    }

    file.close();
}

void PassengerModule::loadFromFile() {
    passengers.clear();

    ifstream file("passengers.txt");

    if (!file) {
        return;
    }

    string line;

    while (getline(file, line)) {
        if (line.length() > 0) {
            PassengerRecord passenger;
            passenger.deserialize(line);
            passengers.push_back(passenger);
        }
    }

    file.close();
}

// ===================== HELPERS =====================

void PassengerModule::resetInputs() {
    inputID = "";
    inputName = "";
    inputPassport = "";
    inputNationality = "";
    inputPhone = "";
    inputEmail = "";

    inputFlight = "";
    inputRoute = "";
    inputSeat = "";
    inputMeal = "";
    inputTicketStatus = "";

    inputBagTag = "";
    inputBagWeight = "";

    activeID = false;
    activeName = false;
    activePassport = false;
    activeNationality = false;
    activePhone = false;
    activeEmail = false;

    activeFlight = false;
    activeRoute = false;
    activeSeat = false;
    activeMeal = false;
    activeTicketStatus = false;

    activeBagTag = false;
    activeBagWeight = false;

    selectedPassengerIndex = -1;
}

int PassengerModule::findPassenger(string passengerID) {
    for (int i = 0; i < (int)passengers.size(); i++) {
        if (passengers[i].getPassengerID() == passengerID) {
            return i;
        }
    }

    return -1;
}