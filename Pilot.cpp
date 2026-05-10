#include "Pilot.h"
#include "SystemUtils.h"

#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

// ===================== THEME COLORS =====================

Color PMS2_NAVY = { 4, 18, 38, 255 };
Color PMS2_TOP_NAVY = { 8, 28, 58, 255 };
Color PMS2_BLUE = { 25, 118, 210, 255 };
Color PMS2_LIGHT_BLUE = { 83, 169, 255, 255 };
Color PMS2_YELLOW = { 255, 193, 7, 255 };
Color PMS2_WHITE = { 245, 248, 255, 255 };
Color PMS2_CARD = { 16, 42, 82, 245 };
Color PMS2_RED = { 230, 57, 70, 255 };
Color PMS2_GREEN = { 46, 204, 113, 255 };

// ===================== PILOT CLASS =====================

Pilot::Pilot() {
    pilotID = "";
    fullName = "";
    rank = "First Officer";
    licenseType = "Passenger";

    totalFlightHours = 0;
    hoursSinceRest = 0;
    lastRestHours = 8;

    available = true;
}

Pilot::Pilot(
    string pilotID,
    string fullName,
    string rank,
    string licenseType,
    float totalFlightHours,
    float hoursSinceRest,
    float lastRestHours
) {
    this->pilotID = pilotID;
    this->fullName = fullName;
    this->rank = rank;
    this->licenseType = licenseType;

    this->totalFlightHours = totalFlightHours;
    this->hoursSinceRest = hoursSinceRest;
    this->lastRestHours = lastRestHours;

    available = hasEnoughRest();
}

string Pilot::getPilotID() const {
    return pilotID;
}

string Pilot::getFullName() const {
    return fullName;
}

string Pilot::getRank() const {
    return rank;
}

string Pilot::getLicenseType() const {
    return licenseType;
}

float Pilot::getTotalFlightHours() const {
    return totalFlightHours;
}

float Pilot::getHoursSinceRest() const {
    return hoursSinceRest;
}

float Pilot::getLastRestHours() const {
    return lastRestHours;
}

bool Pilot::isAvailable() const {
    return available;
}

void Pilot::setFullName(string value) {
    fullName = value;
}

void Pilot::setRank(string value) {
    rank = value;
}

void Pilot::setLicenseType(string value) {
    licenseType = value;
}

void Pilot::addFlightHours(float hours) {
    if (hours <= 0) {
        return;
    }

    totalFlightHours += hours;
    hoursSinceRest += hours;
    lastRestHours = 0;

    available = hasEnoughRest();
}

void Pilot::addRestHours(float hours) {
    if (hours <= 0) {
        return;
    }

    lastRestHours = hours;

    if (hours >= 8) {
        hoursSinceRest = 0;
        available = true;
    }
    else {
        available = false;
    }
}

bool Pilot::hasEnoughRest() const {
    if (lastRestHours < 8) {
        return false;
    }

    if (hoursSinceRest >= 8) {
        return false;
    }

    return true;
}

bool Pilot::licenseMatches(string aircraftType) const {
    if (licenseType == "All") {
        return true;
    }

    if (licenseType == aircraftType) {
        return true;
    }

    return false;
}

bool Pilot::canAssign(string aircraftType, float plannedHours) const {
    if (!hasEnoughRest()) {
        return false;
    }

    if (!licenseMatches(aircraftType)) {
        return false;
    }

    if (plannedHours <= 0) {
        return false;
    }

    if (hoursSinceRest + plannedHours > 8) {
        return false;
    }

    return true;
}

string Pilot::getStatusText() const {
    if (!hasEnoughRest()) {
        return "Not Rested";
    }

    return "Available";
}

string Pilot::serialize() const {
    stringstream ss;

    ss << pilotID << "|"
       << fullName << "|"
       << rank << "|"
       << licenseType << "|"
       << totalFlightHours << "|"
       << hoursSinceRest << "|"
       << lastRestHours << "|"
       << available;

    return ss.str();
}

void Pilot::deserialize(string line) {
    stringstream ss(line);
    string part;
    vector<string> data;

    while (getline(ss, part, '|')) {
        data.push_back(part);
    }

    if ((int)data.size() < 8) {
        return;
    }

    pilotID = data[0];
    fullName = data[1];
    rank = data[2];
    licenseType = data[3];

    totalFlightHours = stof(data[4]);
    hoursSinceRest = stof(data[5]);
    lastRestHours = stof(data[6]);

    available = stoi(data[7]);
}

// ===================== PILOT MODULE =====================

PilotModule::PilotModule() {
    currentPage = MENU;

    message = "";
    messageColor = PMS2_WHITE;

    selectedPilotIndex = -1;

    activePilotID = false;
    activeName = false;
    activeRank = false;
    activeLicense = false;

    activeTotalHours = false;
    activeHoursSinceRest = false;
    activeRestHours = false;

    activeFlightNumber = false;
    activeAircraftType = false;
    activePlannedHours = false;

    loadFromFile();
}

bool PilotModule::Draw(Texture2D logo, Texture2D background, Texture2D crewIcon, Texture2D warningIcon) {
    if (currentPage == MENU) {
        drawMenu(logo, background, crewIcon, warningIcon);
    }
    else if (currentPage == ADD) {
        drawAddPage(logo, background, crewIcon);
    }
    else if (currentPage == VIEW) {
        drawViewPage(logo, background, crewIcon, warningIcon);
    }
    else if (currentPage == SEARCH) {
        drawSearchPage(logo, background, crewIcon, warningIcon);
    }
    else if (currentPage == UPDATE) {
        drawUpdatePage(logo, background);
    }
    else if (currentPage == RECORD_REST) {
        drawRestPage(logo, background, crewIcon);
    }
    else if (currentPage == ASSIGN_FLIGHT) {
        drawAssignFlightPage(logo, background, crewIcon, warningIcon);
    }
    else if (currentPage == REPORT) {
        drawReportPage(logo, background);
    }

    if (button({ 40, 665, 170, 52 }, "BACK", PMS2_YELLOW, { 255, 210, 70, 255 }, PMS2_NAVY)) {
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

void PilotModule::drawBackground(Texture2D background) {
    ClearBackground(PMS2_NAVY);

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

    DrawRectangleGradientV(0, 0, 1200, 750, Fade(PMS2_TOP_NAVY, 0.94f), Fade(PMS2_NAVY, 0.98f));
}

void PilotModule::drawHeader(Texture2D logo, const char* title) {
    DrawRectangle(0, 0, 1200, 96, PMS2_TOP_NAVY);
    DrawRectangle(0, 93, 1200, 3, PMS2_YELLOW);

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

    DrawRectangleRoundedLinesEx({ 20, 12, 80, 80 }, 0.12f, 8, 2.0f, PMS2_YELLOW);
    DrawText(title, 125, 33, 32, PMS2_WHITE);
}

bool PilotModule::button(Rectangle rect, const char* text, Color color, Color hoverColor, Color textColor) {
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, rect);

    DrawRectangleRounded(rect, 0.20f, 12, hover ? hoverColor : color);
    DrawRectangleRoundedLinesEx(rect, 0.20f, 12, 2.0f, PMS2_YELLOW);

    int fontSize = 18;
    int textWidth = MeasureText(text, fontSize);

    DrawText(
        text,
        rect.x + rect.width / 2 - textWidth / 2,
        rect.y + rect.height / 2 - fontSize / 2,
        fontSize,
        textColor
    );

    return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void PilotModule::textBox(Rectangle rect, string& text, bool& active, const char* label) {
    DrawText(label, rect.x, rect.y - 24, 18, PMS2_WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        active = CheckCollisionPointRec(GetMousePosition(), rect);
    }

    DrawRectangleRounded(rect, 0.15f, 10, PMS2_WHITE);
    DrawRectangleRoundedLinesEx(rect, 0.15f, 10, 2.0f, active ? PMS2_YELLOW : PMS2_BLUE);

    DrawText(text.c_str(), rect.x + 12, rect.y + 14, 20, PMS2_NAVY);

    if (active) {
        int key = GetCharPressed();

        while (key > 0) {
            if (key >= 32 && key <= 125 && text.length() < 40) {
                text += (char)key;
            }

            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && text.length() > 0) {
            text.pop_back();
        }
    }
}

void PilotModule::drawIcon(Texture2D texture, Rectangle rect) {
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

void PilotModule::drawPilotCard(Pilot pilot, Rectangle rect, Texture2D crewIcon, Texture2D warningIcon) {
    DrawRectangleRounded(rect, 0.12f, 12, PMS2_CARD);
    DrawRectangleRoundedLinesEx(rect, 0.12f, 12, 2.0f, pilot.hasEnoughRest() ? PMS2_BLUE : PMS2_RED);

    drawIcon(crewIcon, { rect.x + 20, rect.y + 20, 70, 70 });

    DrawText(pilot.getPilotID().c_str(), rect.x + 115, rect.y + 15, 24, PMS2_YELLOW);
    DrawText(pilot.getFullName().c_str(), rect.x + 115, rect.y + 47, 19, PMS2_WHITE);

    string rankLine = "Rank: " + pilot.getRank() + " | License: " + pilot.getLicenseType();
    DrawText(rankLine.c_str(), rect.x + 115, rect.y + 75, 17, PMS2_LIGHT_BLUE);

    string hourLine =
        "Total Hours: " + to_string((int)pilot.getTotalFlightHours()) +
        " | Since Rest: " + to_string((int)pilot.getHoursSinceRest()) +
        " | Last Rest: " + to_string((int)pilot.getLastRestHours()) + " hrs";

    DrawText(hourLine.c_str(), rect.x + 520, rect.y + 35, 18, PMS2_WHITE);

    string statusLine = "Status: " + pilot.getStatusText();
    DrawText(statusLine.c_str(), rect.x + 520, rect.y + 70, 20, pilot.hasEnoughRest() ? PMS2_GREEN : PMS2_RED);

    if (!pilot.hasEnoughRest()) {
        drawIcon(warningIcon, { rect.x + rect.width - 90, rect.y + 30, 55, 55 });
    }
}

// ===================== MENU PAGE =====================

void PilotModule::drawMenu(Texture2D logo, Texture2D background, Texture2D crewIcon, Texture2D warningIcon) {
    drawBackground(background);
    drawHeader(logo, "Pilot Management");

    DrawText(
        "Track pilot ranks, aircraft licenses, flight hours, rest time, and flight assignment safety.",
        55,
        125,
        20,
        Fade(PMS2_WHITE, 0.85f)
    );

    vector<string> options = {
        "Add Pilot",
        "View Pilots",
        "Search Pilot",
        "Update Pilot",
        "Record Rest Time",
        "Assign Flight",
        "Pilot Report"
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

        if (button(rect, options[i].c_str(), PMS2_BLUE, PMS2_LIGHT_BLUE, PMS2_WHITE)) {
            currentPage = (Page)(i + 1);
            message = "";
            resetInputs();
        }
    }

    Rectangle infoBox = { 730, 540, 400, 120 };

    DrawRectangleRounded(infoBox, 0.16f, 12, PMS2_CARD);
    DrawRectangleRoundedLinesEx(infoBox, 0.16f, 12, 2.0f, PMS2_YELLOW);

    drawIcon(crewIcon, { infoBox.x + 20, infoBox.y + 27, 60, 60 });
    drawIcon(warningIcon, { infoBox.x + 95, infoBox.y + 30, 52, 52 });

    DrawText("Assignment Rule", infoBox.x + 175, infoBox.y + 22, 22, PMS2_YELLOW);
    DrawText("No flight if pilot is tired", infoBox.x + 175, infoBox.y + 58, 17, PMS2_WHITE);
    DrawText("or license does not match.", infoBox.x + 175, infoBox.y + 84, 17, PMS2_WHITE);
}

// ===================== ADD PAGE =====================

void PilotModule::drawAddPage(Texture2D logo, Texture2D background, Texture2D crewIcon) {
    drawBackground(background);
    drawHeader(logo, "Add Pilot");

    if (inputPilotID == "") {
        inputPilotID = generateNextID("PL", "pilot.txt");
    }

    drawIcon(crewIcon, { 850, 150, 150, 150 });

    DrawText("License examples: Passenger, Cargo, Private, All", 60, 125, 18, PMS2_YELLOW);
    DrawText("Rank examples: Captain, First Officer, Senior Captain", 60, 150, 18, PMS2_WHITE);

    textBox({ 60, 210, 300, 50 }, inputPilotID, activePilotID, "Pilot ID Auto");
    textBox({ 400, 210, 330, 50 }, inputName, activeName, "Full Name");

    textBox({ 60, 320, 300, 50 }, inputRank, activeRank, "Rank");
    textBox({ 400, 320, 330, 50 }, inputLicense, activeLicense, "Aircraft License");

    textBox({ 60, 430, 300, 50 }, inputTotalHours, activeTotalHours, "Total Flight Hours");
    textBox({ 400, 430, 330, 50 }, inputRestHours, activeRestHours, "Last Rest Hours");

    if (button({ 60, 550, 220, 55 }, "SAVE PILOT", PMS2_GREEN, { 70, 220, 140, 255 }, PMS2_WHITE)) {
        if (
            inputPilotID == "" ||
            inputName == "" ||
            inputRank == "" ||
            inputLicense == "" ||
            inputTotalHours == "" ||
            inputRestHours == ""
        ) {
            message = "Please fill all pilot fields.";
            messageColor = PMS2_RED;
        }
        else if (findPilot(inputPilotID) != -1) {
            message = "Pilot ID already exists.";
            messageColor = PMS2_RED;
        }
        else {
            Pilot pilot(
                inputPilotID,
                inputName,
                inputRank,
                inputLicense,
                safeToFloat(inputTotalHours),
                0,
                safeToFloat(inputRestHours)
            );

            pilots.push_back(pilot);
            saveToFile();

            message = "Pilot added successfully with auto ID.";
            messageColor = PMS2_GREEN;
            resetInputs();
        }
    }

    DrawText(message.c_str(), 320, 568, 20, messageColor);
}

// ===================== VIEW PAGE =====================

void PilotModule::drawViewPage(Texture2D logo, Texture2D background, Texture2D crewIcon, Texture2D warningIcon) {
    drawBackground(background);
    drawHeader(logo, "View Pilots");

    if (pilots.empty()) {
        DrawText("No pilot records found.", 60, 150, 22, PMS2_WHITE);
        return;
    }

    int y = 130;

    for (int i = 0; i < (int)pilots.size() && i < 5; i++) {
        drawPilotCard(pilots[i], { 60, (float)y, 1080, 105 }, crewIcon, warningIcon);
        y += 115;
    }

    if ((int)pilots.size() > 5) {
        DrawText("Only first 5 pilots are shown. All records are saved in pilot.txt.", 60, 660, 18, PMS2_YELLOW);
    }
}

// ===================== SEARCH PAGE =====================

void PilotModule::drawSearchPage(Texture2D logo, Texture2D background, Texture2D crewIcon, Texture2D warningIcon) {
    drawBackground(background);
    drawHeader(logo, "Search Pilot");

    textBox({ 60, 160, 320, 50 }, inputPilotID, activePilotID, "Pilot ID");

    if (button({ 410, 160, 170, 50 }, "SEARCH", PMS2_BLUE, PMS2_LIGHT_BLUE, PMS2_WHITE)) {
        selectedPilotIndex = findPilot(inputPilotID);

        if (selectedPilotIndex == -1) {
            message = "Pilot not found.";
            messageColor = PMS2_RED;
        }
        else {
            message = "Pilot found.";
            messageColor = PMS2_GREEN;
        }
    }

    DrawText(message.c_str(), 610, 175, 20, messageColor);

    if (selectedPilotIndex != -1) {
        drawPilotCard(pilots[selectedPilotIndex], { 60, 270, 1080, 120 }, crewIcon, warningIcon);
    }
}

// ===================== UPDATE PAGE =====================

void PilotModule::drawUpdatePage(Texture2D logo, Texture2D background) {
    drawBackground(background);
    drawHeader(logo, "Update Pilot");

    textBox({ 60, 145, 320, 50 }, inputPilotID, activePilotID, "Pilot ID");

    if (button({ 410, 145, 180, 50 }, "LOAD", PMS2_BLUE, PMS2_LIGHT_BLUE, PMS2_WHITE)) {
        selectedPilotIndex = findPilot(inputPilotID);

        if (selectedPilotIndex == -1) {
            message = "Pilot not found.";
            messageColor = PMS2_RED;
        }
        else {
            Pilot p = pilots[selectedPilotIndex];

            inputName = p.getFullName();
            inputRank = p.getRank();
            inputLicense = p.getLicenseType();

            message = "Pilot loaded. Update required fields.";
            messageColor = PMS2_GREEN;
        }
    }

    textBox({ 60, 260, 330, 50 }, inputName, activeName, "Full Name");
    textBox({ 430, 260, 330, 50 }, inputRank, activeRank, "Rank");

    textBox({ 60, 370, 330, 50 }, inputLicense, activeLicense, "License Type");

    if (button({ 60, 500, 220, 55 }, "UPDATE", PMS2_GREEN, { 70, 220, 140, 255 }, PMS2_WHITE)) {
        if (selectedPilotIndex == -1) {
            message = "Load pilot first.";
            messageColor = PMS2_RED;
        }
        else if (inputName == "" || inputRank == "" || inputLicense == "") {
            message = "Please fill all update fields.";
            messageColor = PMS2_RED;
        }
        else {
            pilots[selectedPilotIndex].setFullName(inputName);
            pilots[selectedPilotIndex].setRank(inputRank);
            pilots[selectedPilotIndex].setLicenseType(inputLicense);

            saveToFile();

            message = "Pilot updated successfully.";
            messageColor = PMS2_GREEN;
        }
    }

    DrawText(message.c_str(), 320, 518, 20, messageColor);
}

// ===================== REST PAGE =====================

void PilotModule::drawRestPage(Texture2D logo, Texture2D background, Texture2D crewIcon) {
    drawBackground(background);
    drawHeader(logo, "Record Pilot Rest Time");

    drawIcon(crewIcon, { 850, 155, 140, 140 });

    DrawText("Minimum required rest time is 8 hours.", 60, 125, 20, PMS2_YELLOW);

    textBox({ 60, 190, 320, 50 }, inputPilotID, activePilotID, "Pilot ID");
    textBox({ 420, 190, 320, 50 }, inputRestHours, activeRestHours, "Rest Hours");

    if (button({ 60, 315, 220, 55 }, "SAVE REST", PMS2_GREEN, { 70, 220, 140, 255 }, PMS2_WHITE)) {
        int index = findPilot(inputPilotID);

        if (index == -1) {
            message = "Pilot not found.";
            messageColor = PMS2_RED;
        }
        else if (inputRestHours == "") {
            message = "Please enter rest hours.";
            messageColor = PMS2_RED;
        }
        else {
            pilots[index].addRestHours(safeToFloat(inputRestHours));
            saveToFile();

            if (pilots[index].hasEnoughRest()) {
                message = "Rest recorded. Pilot is available.";
                messageColor = PMS2_GREEN;
            }
            else {
                message = "Rest recorded, but pilot has not rested enough.";
                messageColor = PMS2_RED;
            }

            resetInputs();
        }
    }

    DrawText(message.c_str(), 320, 333, 20, messageColor);
}

// ===================== ASSIGN FLIGHT PAGE =====================

void PilotModule::drawAssignFlightPage(Texture2D logo, Texture2D background, Texture2D crewIcon, Texture2D warningIcon) {
    drawBackground(background);
    drawHeader(logo, "Assign Pilot To Flight");

    drawIcon(crewIcon, { 850, 145, 120, 120 });
    drawIcon(warningIcon, { 985, 170, 65, 65 });

    DrawText("The system refuses assignment if rest time or license is not valid.", 60, 125, 19, PMS2_YELLOW);

    textBox({ 60, 180, 300, 50 }, inputPilotID, activePilotID, "Pilot ID");
    textBox({ 400, 180, 300, 50 }, inputFlightNumber, activeFlightNumber, "Flight Number");

    textBox({ 60, 290, 300, 50 }, inputAircraftType, activeAircraftType, "Aircraft Type");
    textBox({ 400, 290, 300, 50 }, inputPlannedHours, activePlannedHours, "Planned Flight Hours");

    if (button({ 60, 420, 250, 55 }, "ASSIGN PILOT", PMS2_GREEN, { 70, 220, 140, 255 }, PMS2_WHITE)) {
        int index = findPilot(inputPilotID);

        if (index == -1) {
            message = "Pilot not found.";
            messageColor = PMS2_RED;
        }
        else if (!flightExistsInFile(inputFlightNumber)) {
            message = "Flight number does not exist in flights.txt.";
            messageColor = PMS2_RED;
        }
        else if (inputAircraftType == "" || inputPlannedHours == "") {
            message = "Please enter aircraft type and planned hours.";
            messageColor = PMS2_RED;
        }
        else if (!pilots[index].licenseMatches(inputAircraftType)) {
            message = "Assignment refused: pilot license does not match aircraft type.";
            messageColor = PMS2_RED;
        }
        else if (!pilots[index].hasEnoughRest()) {
            message = "Assignment refused: pilot has not rested enough.";
            messageColor = PMS2_RED;
        }
        else if (!pilots[index].canAssign(inputAircraftType, safeToFloat(inputPlannedHours))) {
            message = "Assignment refused: planned hours exceed duty limit.";
            messageColor = PMS2_RED;
        }
        else {
            pilots[index].addFlightHours(safeToFloat(inputPlannedHours));
            saveToFile();

            ofstream file("pilot_assignments.txt", ios::app);
            file << inputFlightNumber << "|"
                 << pilots[index].getPilotID() << "|"
                 << pilots[index].getFullName() << "|"
                 << pilots[index].getRank() << "|"
                 << pilots[index].getLicenseType() << "|"
                 << inputAircraftType << "|"
                 << inputPlannedHours << endl;
            file.close();

            message = "Pilot assigned successfully. Flight hours updated.";
            messageColor = PMS2_GREEN;
            resetInputs();
        }
    }

    DrawText(message.c_str(), 60, 515, 20, messageColor);
}

// ===================== REPORT PAGE =====================

void PilotModule::drawReportPage(Texture2D logo, Texture2D background) {
    drawBackground(background);
    drawHeader(logo, "Pilot Report");

    int total = (int)pilots.size();
    int available = 0;
    int notRested = 0;

    for (int i = 0; i < (int)pilots.size(); i++) {
        if (pilots[i].hasEnoughRest()) {
            available++;
        }
        else {
            notRested++;
        }
    }

    DrawText("Pilot Safety Summary", 60, 140, 32, PMS2_YELLOW);

    DrawRectangleRounded({ 60, 210, 560, 250 }, 0.14f, 12, PMS2_CARD);
    DrawRectangleRoundedLinesEx({ 60, 210, 560, 250 }, 0.14f, 12, 2.0f, PMS2_BLUE);

    DrawText(("Total Pilots: " + to_string(total)).c_str(), 100, 255, 24, PMS2_WHITE);
    DrawText(("Available Pilots: " + to_string(available)).c_str(), 100, 310, 23, PMS2_GREEN);
    DrawText(("Not Rested Pilots: " + to_string(notRested)).c_str(), 100, 365, 23, PMS2_RED);

    DrawText("Pilot data is saved in pilot.txt and assignments in pilot_assignments.txt.", 60, 520, 20, Fade(PMS2_WHITE, 0.80f));
}

// ===================== FILE HANDLING =====================

void PilotModule::saveToFile() {
    ofstream file("pilot.txt");

    for (int i = 0; i < (int)pilots.size(); i++) {
        file << pilots[i].serialize() << endl;
    }

    file.close();
}

void PilotModule::loadFromFile() {
    pilots.clear();

    ifstream file("pilot.txt");

    if (!file) {
        return;
    }

    string line;

    while (getline(file, line)) {
        if (line.length() > 0) {
            Pilot pilot;
            pilot.deserialize(line);
            pilots.push_back(pilot);
        }
    }

    file.close();
}

// ===================== HELPERS =====================

void PilotModule::resetInputs() {
    inputPilotID = "";
    inputName = "";
    inputRank = "";
    inputLicense = "";

    inputTotalHours = "";
    inputHoursSinceRest = "";
    inputRestHours = "";

    inputFlightNumber = "";
    inputAircraftType = "";
    inputPlannedHours = "";

    activePilotID = false;
    activeName = false;
    activeRank = false;
    activeLicense = false;

    activeTotalHours = false;
    activeHoursSinceRest = false;
    activeRestHours = false;

    activeFlightNumber = false;
    activeAircraftType = false;
    activePlannedHours = false;

    selectedPilotIndex = -1;
}

int PilotModule::findPilot(string pilotID) {
    for (int i = 0; i < (int)pilots.size(); i++) {
        if (pilots[i].getPilotID() == pilotID) {
            return i;
        }
    }

    return -1;
}

float PilotModule::safeToFloat(string text) {
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