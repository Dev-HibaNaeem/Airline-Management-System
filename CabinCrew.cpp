#include "CabinCrew.h"
#include "SystemUtils.h"

#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

// ===================== THEME COLORS =====================

Color CC_NAVY = { 4, 18, 38, 255 };
Color CC_TOP_NAVY = { 8, 28, 58, 255 };
Color CC_BLUE = { 25, 118, 210, 255 };
Color CC_LIGHT_BLUE = { 83, 169, 255, 255 };
Color CC_YELLOW = { 255, 193, 7, 255 };
Color CC_WHITE = { 245, 248, 255, 255 };
Color CC_CARD = { 16, 42, 82, 245 };
Color CC_RED = { 230, 57, 70, 255 };
Color CC_GREEN = { 46, 204, 113, 255 };

// ===================== CABIN CREW CLASS =====================

CabinCrew::CabinCrew() {
    crewID = "";
    fullName = "";
    role = "Flight Attendant";
    languages = "English";

    totalDutyHours = 0;
    hoursSinceRest = 0;
    lastRestHours = 8;

    available = true;
}

CabinCrew::CabinCrew(
    string crewID,
    string fullName,
    string role,
    string languages,
    float totalDutyHours,
    float hoursSinceRest,
    float lastRestHours
) {
    this->crewID = crewID;
    this->fullName = fullName;
    this->role = role;
    this->languages = languages;

    this->totalDutyHours = totalDutyHours;
    this->hoursSinceRest = hoursSinceRest;
    this->lastRestHours = lastRestHours;

    available = hasEnoughRest();
}

string CabinCrew::getCrewID() const {
    return crewID;
}

string CabinCrew::getFullName() const {
    return fullName;
}

string CabinCrew::getRole() const {
    return role;
}

string CabinCrew::getLanguages() const {
    return languages;
}

float CabinCrew::getTotalDutyHours() const {
    return totalDutyHours;
}

float CabinCrew::getHoursSinceRest() const {
    return hoursSinceRest;
}

float CabinCrew::getLastRestHours() const {
    return lastRestHours;
}

bool CabinCrew::isAvailable() const {
    return available;
}

void CabinCrew::setFullName(string value) {
    fullName = value;
}

void CabinCrew::setRole(string value) {
    role = value;
}

void CabinCrew::setLanguages(string value) {
    languages = value;
}

void CabinCrew::addDutyHours(float hours) {
    if (hours <= 0) {
        return;
    }

    totalDutyHours += hours;
    hoursSinceRest += hours;
    lastRestHours = 0;

    available = hasEnoughRest();
}

void CabinCrew::addRestHours(float hours) {
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

bool CabinCrew::hasEnoughRest() const {
    if (lastRestHours < 8) {
        return false;
    }

    if (hoursSinceRest >= 10) {
        return false;
    }

    return true;
}

bool CabinCrew::speaksLanguage(string language) const {
    if (language == "" || language == "None" || language == "Any") {
        return true;
    }

    if (languages.find(language) != string::npos) {
        return true;
    }

    return false;
}

string CabinCrew::getStatusText() const {
    if (!hasEnoughRest()) {
        return "Not Rested";
    }

    return "Available";
}

string CabinCrew::serialize() const {
    stringstream ss;

    ss << crewID << "|"
       << fullName << "|"
       << role << "|"
       << languages << "|"
       << totalDutyHours << "|"
       << hoursSinceRest << "|"
       << lastRestHours << "|"
       << available;

    return ss.str();
}

void CabinCrew::deserialize(string line) {
    stringstream ss(line);
    string part;
    vector<string> data;

    while (getline(ss, part, '|')) {
        data.push_back(part);
    }

    if ((int)data.size() < 8) {
        return;
    }

    crewID = data[0];
    fullName = data[1];
    role = data[2];
    languages = data[3];

    totalDutyHours = stof(data[4]);
    hoursSinceRest = stof(data[5]);
    lastRestHours = stof(data[6]);

    available = stoi(data[7]);
}

// ===================== CABIN CREW MODULE =====================

CabinCrewModule::CabinCrewModule() {
    currentPage = MENU;

    message = "";
    messageColor = CC_WHITE;

    selectedCrewIndex = -1;

    activeCrewID = false;
    activeName = false;
    activeRole = false;
    activeLanguages = false;

    activeTotalHours = false;
    activeRestHours = false;

    activeFlightNumber = false;
    activeRequiredStaff = false;
    activeRequiredLanguage = false;
    activePlannedHours = false;

    activeEmergencyType = false;
    activeEmergencyNote = false;

    loadFromFile();
}

bool CabinCrewModule::Draw(Texture2D logo, Texture2D background, Texture2D crewIcon, Texture2D warningIcon) {
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
    else if (currentPage == EMERGENCY) {
        drawEmergencyPage(logo, background, crewIcon, warningIcon);
    }
    else if (currentPage == REPORT) {
        drawReportPage(logo, background);
    }

    if (button({ 40, 665, 170, 52 }, "BACK", CC_YELLOW, { 255, 210, 70, 255 }, CC_NAVY)) {
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

void CabinCrewModule::drawBackground(Texture2D background) {
    ClearBackground(CC_NAVY);

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

    DrawRectangleGradientV(0, 0, 1200, 750, Fade(CC_TOP_NAVY, 0.94f), Fade(CC_NAVY, 0.98f));
}

void CabinCrewModule::drawHeader(Texture2D logo, const char* title) {
    DrawRectangle(0, 0, 1200, 96, CC_TOP_NAVY);
    DrawRectangle(0, 93, 1200, 3, CC_YELLOW);

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

    DrawRectangleRoundedLinesEx({ 20, 12, 80, 80 }, 0.12f, 8, 2.0f, CC_YELLOW);
    DrawText(title, 125, 33, 32, CC_WHITE);
}

bool CabinCrewModule::button(Rectangle rect, const char* text, Color color, Color hoverColor, Color textColor) {
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, rect);

    DrawRectangleRounded(rect, 0.20f, 12, hover ? hoverColor : color);
    DrawRectangleRoundedLinesEx(rect, 0.20f, 12, 2.0f, CC_YELLOW);

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

void CabinCrewModule::textBox(Rectangle rect, string& text, bool& active, const char* label) {
    DrawText(label, rect.x, rect.y - 24, 18, CC_WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        active = CheckCollisionPointRec(GetMousePosition(), rect);
    }

    DrawRectangleRounded(rect, 0.15f, 10, CC_WHITE);
    DrawRectangleRoundedLinesEx(rect, 0.15f, 10, 2.0f, active ? CC_YELLOW : CC_BLUE);

    DrawText(text.c_str(), rect.x + 12, rect.y + 14, 20, CC_NAVY);

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

void CabinCrewModule::drawIcon(Texture2D texture, Rectangle rect) {
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

void CabinCrewModule::drawCrewCard(CabinCrew crew, Rectangle rect, Texture2D crewIcon, Texture2D warningIcon) {
    DrawRectangleRounded(rect, 0.12f, 12, CC_CARD);
    DrawRectangleRoundedLinesEx(rect, 0.12f, 12, 2.0f, crew.hasEnoughRest() ? CC_BLUE : CC_RED);

    drawIcon(crewIcon, { rect.x + 20, rect.y + 20, 70, 70 });

    DrawText(crew.getCrewID().c_str(), rect.x + 115, rect.y + 15, 24, CC_YELLOW);
    DrawText(crew.getFullName().c_str(), rect.x + 115, rect.y + 47, 19, CC_WHITE);

    string roleLine = "Role: " + crew.getRole();
    DrawText(roleLine.c_str(), rect.x + 115, rect.y + 75, 17, CC_LIGHT_BLUE);

    string langLine = "Languages: " + crew.getLanguages();
    DrawText(langLine.c_str(), rect.x + 470, rect.y + 20, 17, CC_WHITE);

    string hourLine =
        "Duty Hours: " + to_string((int)crew.getTotalDutyHours()) +
        " | Since Rest: " + to_string((int)crew.getHoursSinceRest()) +
        " | Last Rest: " + to_string((int)crew.getLastRestHours());

    DrawText(hourLine.c_str(), rect.x + 470, rect.y + 50, 17, CC_WHITE);

    string statusLine = "Status: " + crew.getStatusText();
    DrawText(statusLine.c_str(), rect.x + 470, rect.y + 80, 19, crew.hasEnoughRest() ? CC_GREEN : CC_RED);

    if (!crew.hasEnoughRest()) {
        drawIcon(warningIcon, { rect.x + rect.width - 85, rect.y + 30, 55, 55 });
    }
}

// ===================== MENU PAGE =====================

void CabinCrewModule::drawMenu(Texture2D logo, Texture2D background, Texture2D crewIcon, Texture2D warningIcon) {
    drawBackground(background);
    drawHeader(logo, "Cabin Crew Management");

    DrawText(
        "Assign cabin crew to flights, check staff availability, languages, rest time, and emergencies.",
        55,
        125,
        20,
        Fade(CC_WHITE, 0.85f)
    );

    vector<string> options = {
        "Add Crew",
        "View Crew",
        "Search Crew",
        "Update Crew",
        "Record Rest",
        "Assign Flight",
        "Emergency Handling",
        "Crew Report"
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

        if (button(rect, options[i].c_str(), CC_BLUE, CC_LIGHT_BLUE, CC_WHITE)) {
            currentPage = (Page)(i + 1);
            message = "";
            resetInputs();
        }
    }

    Rectangle infoBox = { 700, 540, 430, 120 };

    DrawRectangleRounded(infoBox, 0.16f, 12, CC_CARD);
    DrawRectangleRoundedLinesEx(infoBox, 0.16f, 12, 2.0f, CC_YELLOW);

    drawIcon(crewIcon, { infoBox.x + 20, infoBox.y + 25, 65, 65 });
    drawIcon(warningIcon, { infoBox.x + 105, infoBox.y + 28, 55, 55 });

    DrawText("Safety Crew", infoBox.x + 190, infoBox.y + 22, 22, CC_YELLOW);
    DrawText("Enough rested staff", infoBox.x + 190, infoBox.y + 58, 17, CC_WHITE);
    DrawText("must be assigned.", infoBox.x + 190, infoBox.y + 84, 17, CC_WHITE);
}

// ===================== ADD PAGE =====================

void CabinCrewModule::drawAddPage(Texture2D logo, Texture2D background, Texture2D crewIcon) {
    drawBackground(background);
    drawHeader(logo, "Add Cabin Crew");

    if (inputCrewID == "") {
        inputCrewID = generateNextID("CC", "cabincrew.txt");
    }

    drawIcon(crewIcon, { 850, 150, 150, 150 });

    DrawText("Role examples: Lead Attendant, Flight Attendant, Medic Trained", 60, 125, 18, CC_YELLOW);
    DrawText("Languages examples: English, Urdu, Arabic, French, Chinese", 60, 150, 18, CC_WHITE);

    textBox({ 60, 210, 300, 50 }, inputCrewID, activeCrewID, "Crew ID Auto");
    textBox({ 400, 210, 330, 50 }, inputName, activeName, "Full Name");

    textBox({ 60, 320, 300, 50 }, inputRole, activeRole, "Role");
    textBox({ 400, 320, 330, 50 }, inputLanguages, activeLanguages, "Languages");

    textBox({ 60, 430, 300, 50 }, inputTotalHours, activeTotalHours, "Total Duty Hours");
    textBox({ 400, 430, 330, 50 }, inputRestHours, activeRestHours, "Last Rest Hours");

    if (button({ 60, 550, 220, 55 }, "SAVE CREW", CC_GREEN, { 70, 220, 140, 255 }, CC_WHITE)) {
        if (
            inputCrewID == "" ||
            inputName == "" ||
            inputRole == "" ||
            inputLanguages == "" ||
            inputTotalHours == "" ||
            inputRestHours == ""
        ) {
            message = "Please fill all cabin crew fields.";
            messageColor = CC_RED;
        }
        else if (findCrew(inputCrewID) != -1) {
            message = "Crew ID already exists.";
            messageColor = CC_RED;
        }
        else {
            CabinCrew crew(
                inputCrewID,
                inputName,
                inputRole,
                inputLanguages,
                safeToFloat(inputTotalHours),
                0,
                safeToFloat(inputRestHours)
            );

            crewList.push_back(crew);
            saveToFile();

            message = "Cabin crew added successfully with auto ID.";
            messageColor = CC_GREEN;
            resetInputs();
        }
    }

    DrawText(message.c_str(), 320, 568, 20, messageColor);
}

// ===================== VIEW PAGE =====================

void CabinCrewModule::drawViewPage(Texture2D logo, Texture2D background, Texture2D crewIcon, Texture2D warningIcon) {
    drawBackground(background);
    drawHeader(logo, "View Cabin Crew");

    if (crewList.empty()) {
        DrawText("No cabin crew records found.", 60, 150, 22, CC_WHITE);
        return;
    }

    int y = 130;

    for (int i = 0; i < (int)crewList.size() && i < 5; i++) {
        drawCrewCard(crewList[i], { 60, (float)y, 1080, 105 }, crewIcon, warningIcon);
        y += 115;
    }

    if ((int)crewList.size() > 5) {
        DrawText("Only first 5 crew records are shown. All records are saved in cabincrew.txt.", 60, 660, 18, CC_YELLOW);
    }
}

// ===================== SEARCH PAGE =====================

void CabinCrewModule::drawSearchPage(Texture2D logo, Texture2D background, Texture2D crewIcon, Texture2D warningIcon) {
    drawBackground(background);
    drawHeader(logo, "Search Cabin Crew");

    textBox({ 60, 160, 320, 50 }, inputCrewID, activeCrewID, "Crew ID");

    if (button({ 410, 160, 170, 50 }, "SEARCH", CC_BLUE, CC_LIGHT_BLUE, CC_WHITE)) {
        selectedCrewIndex = findCrew(inputCrewID);

        if (selectedCrewIndex == -1) {
            message = "Cabin crew not found.";
            messageColor = CC_RED;
        }
        else {
            message = "Cabin crew found.";
            messageColor = CC_GREEN;
        }
    }

    DrawText(message.c_str(), 610, 175, 20, messageColor);

    if (selectedCrewIndex != -1) {
        drawCrewCard(crewList[selectedCrewIndex], { 60, 270, 1080, 120 }, crewIcon, warningIcon);
    }
}

// ===================== UPDATE PAGE =====================

void CabinCrewModule::drawUpdatePage(Texture2D logo, Texture2D background) {
    drawBackground(background);
    drawHeader(logo, "Update Cabin Crew");

    textBox({ 60, 145, 320, 50 }, inputCrewID, activeCrewID, "Crew ID");

    if (button({ 410, 145, 180, 50 }, "LOAD", CC_BLUE, CC_LIGHT_BLUE, CC_WHITE)) {
        selectedCrewIndex = findCrew(inputCrewID);

        if (selectedCrewIndex == -1) {
            message = "Cabin crew not found.";
            messageColor = CC_RED;
        }
        else {
            CabinCrew c = crewList[selectedCrewIndex];

            inputName = c.getFullName();
            inputRole = c.getRole();
            inputLanguages = c.getLanguages();

            message = "Crew loaded. Update required fields.";
            messageColor = CC_GREEN;
        }
    }

    textBox({ 60, 260, 330, 50 }, inputName, activeName, "Full Name");
    textBox({ 430, 260, 330, 50 }, inputRole, activeRole, "Role");

    textBox({ 60, 370, 500, 50 }, inputLanguages, activeLanguages, "Languages");

    if (button({ 60, 500, 220, 55 }, "UPDATE", CC_GREEN, { 70, 220, 140, 255 }, CC_WHITE)) {
        if (selectedCrewIndex == -1) {
            message = "Load cabin crew first.";
            messageColor = CC_RED;
        }
        else if (inputName == "" || inputRole == "" || inputLanguages == "") {
            message = "Please fill all update fields.";
            messageColor = CC_RED;
        }
        else {
            crewList[selectedCrewIndex].setFullName(inputName);
            crewList[selectedCrewIndex].setRole(inputRole);
            crewList[selectedCrewIndex].setLanguages(inputLanguages);

            saveToFile();

            message = "Cabin crew updated successfully.";
            messageColor = CC_GREEN;
        }
    }

    DrawText(message.c_str(), 320, 518, 20, messageColor);
}

// ===================== REST PAGE =====================

void CabinCrewModule::drawRestPage(Texture2D logo, Texture2D background, Texture2D crewIcon) {
    drawBackground(background);
    drawHeader(logo, "Record Cabin Crew Rest");

    drawIcon(crewIcon, { 850, 155, 140, 140 });

    DrawText("Minimum required rest time is 8 hours.", 60, 125, 20, CC_YELLOW);

    textBox({ 60, 190, 320, 50 }, inputCrewID, activeCrewID, "Crew ID");
    textBox({ 420, 190, 320, 50 }, inputRestHours, activeRestHours, "Rest Hours");

    if (button({ 60, 315, 220, 55 }, "SAVE REST", CC_GREEN, { 70, 220, 140, 255 }, CC_WHITE)) {
        int index = findCrew(inputCrewID);

        if (index == -1) {
            message = "Cabin crew not found.";
            messageColor = CC_RED;
        }
        else if (inputRestHours == "") {
            message = "Please enter rest hours.";
            messageColor = CC_RED;
        }
        else {
            crewList[index].addRestHours(safeToFloat(inputRestHours));
            saveToFile();

            if (crewList[index].hasEnoughRest()) {
                message = "Rest recorded. Crew member is available.";
                messageColor = CC_GREEN;
            }
            else {
                message = "Rest recorded, but crew member has not rested enough.";
                messageColor = CC_RED;
            }

            resetInputs();
        }
    }

    DrawText(message.c_str(), 320, 333, 20, messageColor);
}

// ===================== ASSIGN FLIGHT PAGE =====================

void CabinCrewModule::drawAssignFlightPage(Texture2D logo, Texture2D background, Texture2D crewIcon, Texture2D warningIcon) {
    drawBackground(background);
    drawHeader(logo, "Assign Cabin Crew To Flight");

    drawIcon(crewIcon, { 850, 145, 120, 120 });
    drawIcon(warningIcon, { 985, 170, 65, 65 });

    DrawText("System checks flight number, staff count, rest time, and language requirement.", 60, 125, 19, CC_YELLOW);

    textBox({ 60, 180, 300, 50 }, inputFlightNumber, activeFlightNumber, "Flight Number");
    textBox({ 400, 180, 300, 50 }, inputRequiredStaff, activeRequiredStaff, "Required Staff Count");

    textBox({ 60, 290, 300, 50 }, inputRequiredLanguage, activeRequiredLanguage, "Required Language");
    textBox({ 400, 290, 300, 50 }, inputPlannedHours, activePlannedHours, "Planned Duty Hours");

    if (button({ 60, 420, 260, 55 }, "ASSIGN CREW", CC_GREEN, { 70, 220, 140, 255 }, CC_WHITE)) {
        if (!flightExistsInFile(inputFlightNumber)) {
            message = "Flight number does not exist in flights.txt.";
            messageColor = CC_RED;
        }
        else if (inputRequiredStaff == "" || inputPlannedHours == "") {
            message = "Please enter required staff and planned hours.";
            messageColor = CC_RED;
        }
        else {
            int requiredStaff = safeToInt(inputRequiredStaff);
            float plannedHours = safeToFloat(inputPlannedHours);

            if (requiredStaff <= 0 || plannedHours <= 0) {
                message = "Required staff and planned hours must be valid.";
                messageColor = CC_RED;
            }
            else {
                vector<int> selectedIndexes = getAvailableCrewIndexes(inputRequiredLanguage, requiredStaff);

                if ((int)selectedIndexes.size() < requiredStaff) {
                    message = "Assignment refused: enough rested/language-matched crew not available.";
                    messageColor = CC_RED;
                }
                else {
                    ofstream file("cabincrew_assignments.txt", ios::app);

                    file << inputFlightNumber << "|";

                    for (int i = 0; i < (int)selectedIndexes.size(); i++) {
                        int idx = selectedIndexes[i];

                        crewList[idx].addDutyHours(plannedHours);

                        file << crewList[idx].getCrewID();

                        if (i != (int)selectedIndexes.size() - 1) {
                            file << ",";
                        }
                    }

                    file << "|"
                         << inputRequiredLanguage << "|"
                         << inputRequiredStaff << "|"
                         << inputPlannedHours << endl;

                    file.close();

                    saveToFile();

                    message = "Cabin crew assigned successfully to flight.";
                    messageColor = CC_GREEN;
                    resetInputs();
                }
            }
        }
    }

    DrawText(message.c_str(), 60, 515, 20, messageColor);
}

// ===================== EMERGENCY PAGE =====================

void CabinCrewModule::drawEmergencyPage(Texture2D logo, Texture2D background, Texture2D crewIcon, Texture2D warningIcon) {
    drawBackground(background);
    drawHeader(logo, "Emergency Handling");

    drawIcon(crewIcon, { 850, 145, 120, 120 });
    drawIcon(warningIcon, { 985, 170, 65, 65 });

    DrawText("Emergency types: Fire, Medical, Security, Evacuation", 60, 125, 20, CC_YELLOW);

    textBox({ 60, 180, 300, 50 }, inputFlightNumber, activeFlightNumber, "Flight Number");
    textBox({ 400, 180, 300, 50 }, inputEmergencyType, activeEmergencyType, "Emergency Type");

    textBox({ 60, 290, 640, 50 }, inputEmergencyNote, activeEmergencyNote, "Emergency Note");

    if (button({ 60, 420, 270, 55 }, "HANDLE EMERGENCY", CC_RED, { 255, 90, 100, 255 }, CC_WHITE)) {
        if (!flightExistsInFile(inputFlightNumber)) {
            message = "Flight number does not exist.";
            messageColor = CC_RED;
        }
        else if (inputEmergencyType == "" || inputEmergencyNote == "") {
            message = "Please enter emergency type and note.";
            messageColor = CC_RED;
        }
        else {
            ofstream file("cabincrew_emergencies.txt", ios::app);

            file << inputFlightNumber << "|"
                 << inputEmergencyType << "|"
                 << inputEmergencyNote << "|";

            if (inputEmergencyType == "Fire") {
                file << "Crew used fire extinguisher and secured passengers.";
                message = "Fire emergency handled by cabin crew.";
            }
            else if (inputEmergencyType == "Medical") {
                file << "Crew gave first aid and requested medical support.";
                message = "Medical emergency handled by cabin crew.";
            }
            else if (inputEmergencyType == "Security") {
                file << "Crew followed security protocol and informed cockpit.";
                message = "Security emergency handled by cabin crew.";
            }
            else if (inputEmergencyType == "Evacuation") {
                file << "Crew guided passengers to emergency exits.";
                message = "Evacuation emergency handled by cabin crew.";
            }
            else {
                file << "Crew responded using standard safety procedure.";
                message = "Emergency handled by cabin crew.";
            }

            file << endl;
            file.close();

            messageColor = CC_GREEN;
            resetInputs();
        }
    }

    DrawText(message.c_str(), 60, 515, 20, messageColor);
}

// ===================== REPORT PAGE =====================

void CabinCrewModule::drawReportPage(Texture2D logo, Texture2D background) {
    drawBackground(background);
    drawHeader(logo, "Cabin Crew Report");

    int total = (int)crewList.size();
    int available = 0;
    int notRested = 0;

    for (int i = 0; i < (int)crewList.size(); i++) {
        if (crewList[i].hasEnoughRest()) {
            available++;
        }
        else {
            notRested++;
        }
    }

    DrawText("Cabin Crew Safety Summary", 60, 140, 32, CC_YELLOW);

    DrawRectangleRounded({ 60, 210, 600, 270 }, 0.14f, 12, CC_CARD);
    DrawRectangleRoundedLinesEx({ 60, 210, 600, 270 }, 0.14f, 12, 2.0f, CC_BLUE);

    DrawText(("Total Crew Members: " + to_string(total)).c_str(), 100, 260, 24, CC_WHITE);
    DrawText(("Available Crew: " + to_string(available)).c_str(), 100, 320, 23, CC_GREEN);
    DrawText(("Not Rested Crew: " + to_string(notRested)).c_str(), 100, 380, 23, CC_RED);

    DrawText("Assignments are saved in cabincrew_assignments.txt.", 60, 530, 20, Fade(CC_WHITE, 0.80f));
    DrawText("Emergencies are saved in cabincrew_emergencies.txt.", 60, 560, 20, Fade(CC_WHITE, 0.80f));
}

// ===================== FILE HANDLING =====================

void CabinCrewModule::saveToFile() {
    ofstream file("cabincrew.txt");

    for (int i = 0; i < (int)crewList.size(); i++) {
        file << crewList[i].serialize() << endl;
    }

    file.close();
}

void CabinCrewModule::loadFromFile() {
    crewList.clear();

    ifstream file("cabincrew.txt");

    if (!file) {
        return;
    }

    string line;

    while (getline(file, line)) {
        if (line.length() > 0) {
            CabinCrew crew;
            crew.deserialize(line);
            crewList.push_back(crew);
        }
    }

    file.close();
}

// ===================== HELPERS =====================

void CabinCrewModule::resetInputs() {
    inputCrewID = "";
    inputName = "";
    inputRole = "";
    inputLanguages = "";

    inputTotalHours = "";
    inputRestHours = "";

    inputFlightNumber = "";
    inputRequiredStaff = "";
    inputRequiredLanguage = "";
    inputPlannedHours = "";

    inputEmergencyType = "";
    inputEmergencyNote = "";

    activeCrewID = false;
    activeName = false;
    activeRole = false;
    activeLanguages = false;

    activeTotalHours = false;
    activeRestHours = false;

    activeFlightNumber = false;
    activeRequiredStaff = false;
    activeRequiredLanguage = false;
    activePlannedHours = false;

    activeEmergencyType = false;
    activeEmergencyNote = false;

    selectedCrewIndex = -1;
}

int CabinCrewModule::findCrew(string crewID) {
    for (int i = 0; i < (int)crewList.size(); i++) {
        if (crewList[i].getCrewID() == crewID) {
            return i;
        }
    }

    return -1;
}

int CabinCrewModule::safeToInt(string text) {
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

float CabinCrewModule::safeToFloat(string text) {
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

int CabinCrewModule::countAvailableCrew(string requiredLanguage) {
    int count = 0;

    for (int i = 0; i < (int)crewList.size(); i++) {
        if (crewList[i].hasEnoughRest() && crewList[i].speaksLanguage(requiredLanguage)) {
            count++;
        }
    }

    return count;
}

vector<int> CabinCrewModule::getAvailableCrewIndexes(string requiredLanguage, int requiredCount) {
    vector<int> indexes;

    for (int i = 0; i < (int)crewList.size(); i++) {
        if (crewList[i].hasEnoughRest() && crewList[i].speaksLanguage(requiredLanguage)) {
            indexes.push_back(i);

            if ((int)indexes.size() == requiredCount) {
                break;
            }
        }
    }

    return indexes;
}