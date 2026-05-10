#include "Aircraft.h"
#include "SystemUtils.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
using namespace std;

// ================= THEME COLORS =================

Color AMS_NAVY = { 4, 18, 38, 255 };
Color AMS_BLUE = { 25, 118, 210, 255 };
Color AMS_LIGHT_BLUE = { 83, 169, 255, 255 };
Color AMS_YELLOW = { 255, 193, 7, 255 };
Color AMS_WHITE = { 245, 248, 255, 255 };
Color AMS_CARD = { 16, 42, 82, 245 };
Color AMS_CARD_HOVER = { 28, 73, 130, 255 };
Color AMS_RED = { 230, 57, 70, 255 };
Color AMS_GREEN = { 46, 204, 113, 255 };

// ================= SAFE CONVERSION HELPERS =================

static int safeToIntAircraft(string text) {
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

static float safeToFloatAircraft(string text) {
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

// ================= AIRCRAFT CLASS =================

Aircraft::Aircraft() {
    id = "";
    model = "";
    type = "Passenger";
    capacity = 0;
    fuel = 100;
    condition = 100;
    damage = 0;
    totalFlightHours = 0;
    hoursSinceMaintenance = 0;
    maintenanceLimit = 100;
    inMaintenance = false;
}

Aircraft::Aircraft(string id, string model, string type, int capacity, float fuel,
                   float condition, float maintenanceLimit) {
    this->id = id;
    this->model = model;
    this->type = type;
    this->capacity = capacity;
    this->fuel = fuel;
    this->condition = condition;
    this->maintenanceLimit = maintenanceLimit;

    damage = 0;
    totalFlightHours = 0;
    hoursSinceMaintenance = 0;
    inMaintenance = false;
}

string Aircraft::getId() const {
    return id;
}

string Aircraft::getModel() const {
    return model;
}

string Aircraft::getType() const {
    return type;
}

int Aircraft::getCapacity() const {
    return capacity;
}

float Aircraft::getFuel() const {
    return fuel;
}

float Aircraft::getCondition() const {
    return condition;
}

float Aircraft::getDamage() const {
    return damage;
}

float Aircraft::getTotalFlightHours() const {
    return totalFlightHours;
}

float Aircraft::getHoursSinceMaintenance() const {
    return hoursSinceMaintenance;
}

float Aircraft::getMaintenanceLimit() const {
    return maintenanceLimit;
}

bool Aircraft::getMaintenanceStatus() const {
    return inMaintenance;
}

void Aircraft::setModel(string newModel) {
    model = newModel;
}

void Aircraft::setCapacity(int newCapacity) {
    capacity = newCapacity;

    if (capacity < 0) {
        capacity = 0;
    }
}

void Aircraft::setFuel(float newFuel) {
    fuel = newFuel;

    if (fuel > 100) {
        fuel = 100;
    }

    if (fuel < 0) {
        fuel = 0;
    }
}

void Aircraft::setCondition(float newCondition) {
    condition = newCondition;

    if (condition > 100) {
        condition = 100;
    }

    if (condition < 0) {
        condition = 0;
    }
}

void Aircraft::setMaintenanceLimit(float newLimit) {
    maintenanceLimit = newLimit;

    if (maintenanceLimit < 1) {
        maintenanceLimit = 1;
    }
}

bool Aircraft::canFly() const {
    if (inMaintenance) return false;
    if (fuel < 20) return false;
    if (condition < 60) return false;
    if (damage > 40) return false;
    if (hoursSinceMaintenance >= maintenanceLimit) return false;

    return true;
}

string Aircraft::getStatusText() const {
    if (inMaintenance) {
        return "Under Maintenance";
    }

    if (!canFly()) {
        return "Not Safe";
    }

    return "Ready";
}

void Aircraft::refuel(float amount) {
    if (amount <= 0) {
        return;
    }

    fuel += amount;

    if (fuel > 100) {
        fuel = 100;
    }
}

void Aircraft::recordFlight(string route, float hours, float fuelUsed) {
    if (!canFly()) {
        return;
    }

    if (hours <= 0 || fuelUsed <= 0) {
        return;
    }

    if (fuelUsed > fuel) {
        return;
    }

    fuel -= fuelUsed;
    totalFlightHours += hours;
    hoursSinceMaintenance += hours;

    damage += hours * 1.5f;
    condition -= hours * 1.2f;

    if (damage > 100) {
        damage = 100;
    }

    if (condition < 0) {
        condition = 0;
    }

    stringstream ss;
    ss << route << " | Hours: " << hours << " | Fuel Used: " << fuelUsed << "%";
    flightHistory.push_back(ss.str());

    if (hoursSinceMaintenance >= maintenanceLimit || condition < 60 || damage > 40) {
        inMaintenance = true;
        flightHistory.push_back("System Alert: Aircraft automatically sent for maintenance.");
    }
}

void Aircraft::sendMaintenance(string reason) {
    inMaintenance = true;
    flightHistory.push_back("Maintenance Required: " + reason);
}

void Aircraft::completeMaintenance() {
    inMaintenance = false;
    condition = 100;
    damage = 0;
    hoursSinceMaintenance = 0;
    flightHistory.push_back("Maintenance Completed: Aircraft restored to safe condition.");
}

vector<string> Aircraft::getHistory() const {
    return flightHistory;
}

string Aircraft::serialize() const {
    stringstream ss;

    ss << id << "|"
       << model << "|"
       << type << "|"
       << capacity << "|"
       << fuel << "|"
       << condition << "|"
       << damage << "|"
       << totalFlightHours << "|"
       << hoursSinceMaintenance << "|"
       << maintenanceLimit << "|"
       << inMaintenance << "|";

    for (int i = 0; i < (int)flightHistory.size(); i++) {
        ss << flightHistory[i];

        if (i != (int)flightHistory.size() - 1) {
            ss << ";";
        }
    }

    return ss.str();
}

void Aircraft::deserialize(string line) {
    stringstream ss(line);
    string part;
    vector<string> data;

    while (getline(ss, part, '|')) {
        data.push_back(part);
    }

    if ((int)data.size() < 11) {
        return;
    }

    id = data[0];
    model = data[1];
    type = data[2];

    capacity = safeToIntAircraft(data[3]);
    fuel = safeToFloatAircraft(data[4]);
    condition = safeToFloatAircraft(data[5]);
    damage = safeToFloatAircraft(data[6]);
    totalFlightHours = safeToFloatAircraft(data[7]);
    hoursSinceMaintenance = safeToFloatAircraft(data[8]);
    maintenanceLimit = safeToFloatAircraft(data[9]);
    inMaintenance = safeToIntAircraft(data[10]);

    flightHistory.clear();

    if ((int)data.size() >= 12) {
        stringstream hs(data[11]);
        string h;

        while (getline(hs, h, ';')) {
            if (h != "") {
                flightHistory.push_back(h);
            }
        }
    }
}

// ================= AIRCRAFT MODULE =================

AircraftModule::AircraftModule() {
    currentPage = MENU;
    message = "";
    messageColor = AMS_WHITE;

    selectedAircraftIndex = -1;
    selectedType = 0;

    activeId = false;
    activeModel = false;
    activeCapacity = false;
    activeFuel = false;
    activeCondition = false;
    activeLimit = false;
    activeAmount = false;
    activeRoute = false;
    activeHours = false;
    activeFuelUsed = false;
    activeReason = false;

    loadFromFile();
}

bool AircraftModule::Draw(AircraftAssets assets) {
    if (currentPage == MENU) {
        drawMenu(assets);
    }
    else if (currentPage == ADD) {
        drawAddPage(assets);
    }
    else if (currentPage == VIEW) {
        drawViewPage(assets);
    }
    else if (currentPage == SEARCH) {
        drawSearchPage(assets);
    }
    else if (currentPage == UPDATE) {
        drawUpdatePage(assets);
    }
    else if (currentPage == REFUEL) {
        drawRefuelPage(assets);
    }
    else if (currentPage == RECORD_FLIGHT) {
        drawRecordFlightPage(assets);
    }
    else if (currentPage == DIAGNOSTICS) {
        drawDiagnosticsPage(assets);
    }
    else if (currentPage == SEND_MAINTENANCE) {
        drawSendMaintenancePage(assets);
    }
    else if (currentPage == COMPLETE_MAINTENANCE) {
        drawCompleteMaintenancePage(assets);
    }
    else if (currentPage == HISTORY) {
        drawHistoryPage(assets);
    }
    else if (currentPage == DELETE_AIRCRAFT) {
        drawDeletePage(assets);
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

// ================= DRAWING HELPERS =================

void AircraftModule::drawHeader(AircraftAssets assets, const char* title) {
    ClearBackground(AMS_NAVY);

    if (assets.background.id > 0) {
        DrawTexturePro(
            assets.background,
            { 0, 0, (float)assets.background.width, (float)assets.background.height },
            { 0, 0, 1200, 750 },
            { 0, 0 },
            0,
            Fade(WHITE, 0.25f)
        );
    }

    DrawRectangleGradientV(0, 0, 1200, 750, Fade({ 8, 28, 58, 255 }, 0.94f), Fade(AMS_NAVY, 0.98f));

    DrawRectangle(0, 0, 1200, 96, { 8, 28, 58, 255 });
    DrawRectangle(0, 93, 1200, 3, AMS_YELLOW);

    if (assets.logo.id > 0) {
        DrawTexturePro(
            assets.logo,
            { 0, 0, (float)assets.logo.width, (float)assets.logo.height },
            { 25, 17, 70, 70 },
            { 0, 0 },
            0,
            WHITE
        );
    }

    DrawRectangleRoundedLinesEx({ 20, 12, 80, 80 }, 0.12f, 8, 2.0f, AMS_YELLOW);
    DrawText(title, 125, 33, 32, AMS_WHITE);
}

bool AircraftModule::button(Rectangle rect, const char* text, Color color, Color hoverColor, Color textColor) {
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, rect);

    DrawRectangleRounded(rect, 0.20f, 12, hover ? hoverColor : color);
    DrawRectangleRoundedLinesEx(rect, 0.20f, 12, 2.0f, AMS_YELLOW);

    int fontSize = 19;
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

void AircraftModule::textBox(Rectangle rect, string& text, bool& active, const char* label) {
    DrawText(label, rect.x, rect.y - 25, 18, AMS_WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        active = CheckCollisionPointRec(GetMousePosition(), rect);
    }

    DrawRectangleRounded(rect, 0.15f, 10, AMS_WHITE);
    DrawRectangleRoundedLinesEx(rect, 0.15f, 10, 2.0f, active ? AMS_YELLOW : AMS_BLUE);

    DrawText(text.c_str(), rect.x + 12, rect.y + 14, 20, AMS_NAVY);

    if (active) {
        int key = GetCharPressed();

        while (key > 0) {
            if (key >= 32 && key <= 125 && text.length() < 35) {
                text += (char)key;
            }

            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && text.length() > 0) {
            text.pop_back();
        }
    }
}

// ================= MENU PAGE =================

void AircraftModule::drawMenu(AircraftAssets assets) {
    drawHeader(assets, "Aircraft Management");

    DrawText(
        "Manage aircraft fuel, condition, damage, flight history, and maintenance status.",
        55,
        125,
        20,
        Fade(AMS_WHITE, 0.85f)
    );

    vector<string> options = {
        "Add Aircraft",
        "View Aircraft",
        "Search Aircraft",
        "Update Aircraft",
        "Refuel Aircraft",
        "Record Flight",
        "Run Diagnostics",
        "Send Maintenance",
        "Complete Maintenance",
        "Flight History",
        "Delete Aircraft"
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

        if (button(rect, options[i].c_str(), AMS_BLUE, AMS_LIGHT_BLUE, AMS_WHITE)) {
            currentPage = (Page)(i + 1);
            message = "";
            resetInputs();
        }
    }

    DrawRectangleRounded({ 760, 555, 355, 95 }, 0.16f, 12, AMS_CARD);
    DrawRectangleRoundedLinesEx({ 760, 555, 355, 95 }, 0.16f, 12, 2.0f, AMS_YELLOW);

    DrawText("Safety Rule", 785, 575, 22, AMS_YELLOW);
    DrawText("Aircraft cannot fly if fuel, condition,", 785, 610, 17, AMS_WHITE);
    DrawText("damage, or maintenance limits are unsafe.", 785, 633, 17, AMS_WHITE);
}

// ================= ADD AIRCRAFT PAGE =================

void AircraftModule::drawAddPage(AircraftAssets assets) {
    drawHeader(assets, "Add Aircraft");

    if (inputId == "") {
        inputId = generateNextID("AI", "aircraft.txt");
    }

    DrawText("Select Aircraft Type", 60, 130, 22, AMS_YELLOW);

    if (button({ 60, 165, 180, 45 }, "Passenger", selectedType == 0 ? AMS_LIGHT_BLUE : AMS_BLUE, AMS_LIGHT_BLUE, AMS_WHITE)) {
        selectedType = 0;
    }

    if (button({ 260, 165, 180, 45 }, "Cargo", selectedType == 1 ? AMS_LIGHT_BLUE : AMS_BLUE, AMS_LIGHT_BLUE, AMS_WHITE)) {
        selectedType = 1;
    }

    if (button({ 460, 165, 180, 45 }, "Private", selectedType == 2 ? AMS_LIGHT_BLUE : AMS_BLUE, AMS_LIGHT_BLUE, AMS_WHITE)) {
        selectedType = 2;
    }

    string type = selectedType == 0 ? "Passenger" : selectedType == 1 ? "Cargo" : "Private";
    Texture2D plane = getPlaneTexture(assets, type);

    if (plane.id > 0) {
        DrawTexturePro(
            plane,
            { 0, 0, (float)plane.width, (float)plane.height },
            { 850, 150, 210, 145 },
            { 0, 0 },
            0,
            WHITE
        );
    }

    textBox({ 60, 265, 300, 50 }, inputId, activeId, "Aircraft ID Auto");
    textBox({ 400, 265, 320, 50 }, inputModel, activeModel, "Aircraft Model");

    textBox({ 60, 375, 300, 50 }, inputCapacity, activeCapacity, "Capacity");
    textBox({ 400, 375, 320, 50 }, inputFuel, activeFuel, "Fuel %");

    textBox({ 60, 485, 300, 50 }, inputCondition, activeCondition, "Condition %");
    textBox({ 400, 485, 320, 50 }, inputLimit, activeLimit, "Maintenance Limit Hours");

    if (button({ 60, 570, 250, 55 }, "SAVE AIRCRAFT", AMS_GREEN, { 70, 220, 140, 255 }, AMS_WHITE)) {
        if (
            inputId == "" ||
            inputModel == "" ||
            inputCapacity == "" ||
            inputFuel == "" ||
            inputCondition == "" ||
            inputLimit == ""
        ) {
            message = "Please fill all aircraft fields.";
            messageColor = AMS_RED;
        }
        else if (findAircraft(inputId) != -1) {
            message = "Aircraft ID already exists.";
            messageColor = AMS_RED;
        }
        else {
            Aircraft aircraft(
                inputId,
                inputModel,
                type,
                safeToIntAircraft(inputCapacity),
                safeToFloatAircraft(inputFuel),
                safeToFloatAircraft(inputCondition),
                safeToFloatAircraft(inputLimit)
            );

            aircraftList.push_back(aircraft);
            saveToFile();

            message = "Aircraft added successfully with auto ID.";
            messageColor = AMS_GREEN;
            resetInputs();
        }
    }

    DrawText(message.c_str(), 340, 587, 20, messageColor);
}

// ================= VIEW PAGE =================

void AircraftModule::drawViewPage(AircraftAssets assets) {
    drawHeader(assets, "View Aircraft");

    DrawText("Aircraft Records", 60, 125, 26, AMS_YELLOW);

    if (aircraftList.empty()) {
        DrawText("No aircraft records found.", 60, 185, 22, AMS_WHITE);
        return;
    }

    int y = 165;

    for (int i = 0; i < (int)aircraftList.size() && i < 5; i++) {
        drawAircraftCard(assets, aircraftList[i], { 60, (float)y, 1080, 95 });
        y += 105;
    }

    if ((int)aircraftList.size() > 5) {
        DrawText(
            "Only first 5 records are shown on screen. All records are saved in aircraft.txt.",
            60,
            650,
            18,
            AMS_YELLOW
        );
    }
}

// ================= SEARCH PAGE =================

void AircraftModule::drawSearchPage(AircraftAssets assets) {
    drawHeader(assets, "Search Aircraft");

    textBox({ 60, 175, 320, 50 }, inputId, activeId, "Enter Aircraft ID");

    if (button({ 410, 175, 170, 50 }, "SEARCH", AMS_BLUE, AMS_LIGHT_BLUE, AMS_WHITE)) {
        selectedAircraftIndex = findAircraft(inputId);

        if (selectedAircraftIndex == -1) {
            message = "Aircraft not found.";
            messageColor = AMS_RED;
        }
        else {
            message = "Aircraft found.";
            messageColor = AMS_GREEN;
        }
    }

    DrawText(message.c_str(), 610, 190, 20, messageColor);

    if (selectedAircraftIndex != -1) {
        drawAircraftCard(assets, aircraftList[selectedAircraftIndex], { 60, 290, 1080, 120 });
    }
}

// ================= UPDATE PAGE =================

void AircraftModule::drawUpdatePage(AircraftAssets assets) {
    drawHeader(assets, "Update Aircraft");

    textBox({ 60, 155, 320, 50 }, inputId, activeId, "Aircraft ID");

    if (button({ 410, 155, 180, 50 }, "LOAD", AMS_BLUE, AMS_LIGHT_BLUE, AMS_WHITE)) {
        selectedAircraftIndex = findAircraft(inputId);

        if (selectedAircraftIndex == -1) {
            message = "Aircraft not found.";
            messageColor = AMS_RED;
        }
        else {
            Aircraft aircraft = aircraftList[selectedAircraftIndex];

            inputModel = aircraft.getModel();
            inputCapacity = to_string(aircraft.getCapacity());
            inputFuel = to_string((int)aircraft.getFuel());
            inputCondition = to_string((int)aircraft.getCondition());
            inputLimit = to_string((int)aircraft.getMaintenanceLimit());

            message = "Aircraft loaded. You can update fields.";
            messageColor = AMS_GREEN;
        }
    }

    textBox({ 60, 270, 300, 50 }, inputModel, activeModel, "New Model");
    textBox({ 400, 270, 300, 50 }, inputCapacity, activeCapacity, "New Capacity");

    textBox({ 60, 370, 300, 50 }, inputFuel, activeFuel, "New Fuel %");
    textBox({ 400, 370, 300, 50 }, inputCondition, activeCondition, "New Condition %");

    textBox({ 60, 470, 300, 50 }, inputLimit, activeLimit, "New Maintenance Limit");

    if (button({ 60, 575, 220, 55 }, "UPDATE", AMS_GREEN, { 70, 220, 140, 255 }, AMS_WHITE)) {
        if (selectedAircraftIndex == -1) {
            message = "Load aircraft first.";
            messageColor = AMS_RED;
        }
        else if (
            inputModel == "" ||
            inputCapacity == "" ||
            inputFuel == "" ||
            inputCondition == "" ||
            inputLimit == ""
        ) {
            message = "Please fill all update fields.";
            messageColor = AMS_RED;
        }
        else {
            aircraftList[selectedAircraftIndex].setModel(inputModel);
            aircraftList[selectedAircraftIndex].setCapacity(safeToIntAircraft(inputCapacity));
            aircraftList[selectedAircraftIndex].setFuel(safeToFloatAircraft(inputFuel));
            aircraftList[selectedAircraftIndex].setCondition(safeToFloatAircraft(inputCondition));
            aircraftList[selectedAircraftIndex].setMaintenanceLimit(safeToFloatAircraft(inputLimit));

            saveToFile();

            message = "Aircraft updated successfully.";
            messageColor = AMS_GREEN;
        }
    }

    DrawText(message.c_str(), 320, 592, 20, messageColor);
}

// ================= REFUEL PAGE =================

void AircraftModule::drawRefuelPage(AircraftAssets assets) {
    drawHeader(assets, "Refuel Aircraft");

    textBox({ 60, 180, 320, 50 }, inputId, activeId, "Aircraft ID");
    textBox({ 420, 180, 260, 50 }, inputAmount, activeAmount, "Fuel Amount %");

    if (button({ 60, 280, 220, 55 }, "REFUEL", AMS_GREEN, { 70, 220, 140, 255 }, AMS_WHITE)) {
        int index = findAircraft(inputId);

        if (index == -1) {
            message = "Aircraft not found.";
            messageColor = AMS_RED;
        }
        else if (inputAmount == "") {
            message = "Please enter fuel amount.";
            messageColor = AMS_RED;
        }
        else {
            aircraftList[index].refuel(safeToFloatAircraft(inputAmount));
            saveToFile();

            message = "Aircraft refueled successfully.";
            messageColor = AMS_GREEN;
            resetInputs();
        }
    }

    DrawText(message.c_str(), 60, 380, 22, messageColor);
}

// ================= RECORD FLIGHT PAGE =================

void AircraftModule::drawRecordFlightPage(AircraftAssets assets) {
    drawHeader(assets, "Record Aircraft Journey");

    textBox({ 60, 150, 320, 50 }, inputId, activeId, "Aircraft ID");
    textBox({ 420, 150, 360, 50 }, inputRoute, activeRoute, "Route");

    textBox({ 60, 260, 320, 50 }, inputHours, activeHours, "Flight Hours");
    textBox({ 420, 260, 320, 50 }, inputFuelUsed, activeFuelUsed, "Fuel Used %");

    if (button({ 60, 380, 230, 55 }, "RECORD FLIGHT", AMS_GREEN, { 70, 220, 140, 255 }, AMS_WHITE)) {
        int index = findAircraft(inputId);

        if (index == -1) {
            message = "Aircraft not found.";
            messageColor = AMS_RED;
        }
        else if (inputRoute == "" || inputHours == "" || inputFuelUsed == "") {
            message = "Please fill route, hours, and fuel used.";
            messageColor = AMS_RED;
        }
        else if (!aircraftList[index].canFly()) {
            message = "Aircraft cannot fly. Check diagnostics or maintenance.";
            messageColor = AMS_RED;
        }
        else {
            aircraftList[index].recordFlight(
                inputRoute,
                safeToFloatAircraft(inputHours),
                safeToFloatAircraft(inputFuelUsed)
            );

            saveToFile();

            message = "Flight journey recorded successfully.";
            messageColor = AMS_GREEN;
            resetInputs();
        }
    }

    DrawText(message.c_str(), 320, 398, 20, messageColor);
}

// ================= DIAGNOSTICS PAGE =================

void AircraftModule::drawDiagnosticsPage(AircraftAssets assets) {
    drawHeader(assets, "Aircraft Diagnostics");

    textBox({ 60, 170, 320, 50 }, inputId, activeId, "Aircraft ID");

    if (button({ 410, 170, 190, 50 }, "RUN CHECK", AMS_BLUE, AMS_LIGHT_BLUE, AMS_WHITE)) {
        selectedAircraftIndex = findAircraft(inputId);

        if (selectedAircraftIndex == -1) {
            message = "Aircraft not found.";
            messageColor = AMS_RED;
        }
        else {
            message = "Diagnostics completed.";
            messageColor = AMS_GREEN;
        }
    }

    DrawText(message.c_str(), 630, 185, 20, messageColor);

    if (selectedAircraftIndex != -1) {
        Aircraft aircraft = aircraftList[selectedAircraftIndex];

        drawAircraftCard(assets, aircraft, { 60, 285, 1080, 120 });

        int y = 450;

        DrawText("Diagnostic Result", 60, y, 26, AMS_YELLOW);
        y += 45;

        if (aircraft.canFly()) {
            DrawText("Aircraft is safe and ready for flight.", 60, y, 22, AMS_GREEN);
        }
        else {
            DrawText("Aircraft is NOT safe for flight.", 60, y, 22, AMS_RED);
            y += 35;

            if (aircraft.getFuel() < 20) {
                DrawText("- Fuel is below safe limit.", 60, y, 18, AMS_WHITE);
                y += 28;
            }

            if (aircraft.getCondition() < 60) {
                DrawText("- Aircraft condition is weak.", 60, y, 18, AMS_WHITE);
                y += 28;
            }

            if (aircraft.getDamage() > 40) {
                DrawText("- Damage level is high.", 60, y, 18, AMS_WHITE);
                y += 28;
            }

            if (aircraft.getHoursSinceMaintenance() >= aircraft.getMaintenanceLimit()) {
                DrawText("- Maintenance limit reached.", 60, y, 18, AMS_WHITE);
                y += 28;
            }

            if (aircraft.getMaintenanceStatus()) {
                DrawText("- Aircraft is under maintenance.", 60, y, 18, AMS_WHITE);
            }
        }
    }
}

// ================= SEND MAINTENANCE PAGE =================

void AircraftModule::drawSendMaintenancePage(AircraftAssets assets) {
    drawHeader(assets, "Send Aircraft To Maintenance");

    textBox({ 60, 180, 320, 50 }, inputId, activeId, "Aircraft ID");
    textBox({ 420, 180, 400, 50 }, inputReason, activeReason, "Maintenance Reason");

    if (button({ 60, 310, 260, 55 }, "SEND MAINTENANCE", AMS_RED, { 255, 90, 100, 255 }, AMS_WHITE)) {
        int index = findAircraft(inputId);

        if (index == -1) {
            message = "Aircraft not found.";
            messageColor = AMS_RED;
        }
        else if (inputReason == "") {
            message = "Please enter maintenance reason.";
            messageColor = AMS_RED;
        }
        else {
            aircraftList[index].sendMaintenance(inputReason);
            saveToFile();

            message = "Aircraft sent to maintenance.";
            messageColor = AMS_GREEN;
            resetInputs();
        }
    }

    DrawText(message.c_str(), 360, 328, 20, messageColor);
}

// ================= COMPLETE MAINTENANCE PAGE =================

void AircraftModule::drawCompleteMaintenancePage(AircraftAssets assets) {
    drawHeader(assets, "Complete Maintenance");

    textBox({ 60, 180, 320, 50 }, inputId, activeId, "Aircraft ID");

    if (button({ 410, 180, 260, 50 }, "COMPLETE", AMS_GREEN, { 70, 220, 140, 255 }, AMS_WHITE)) {
        int index = findAircraft(inputId);

        if (index == -1) {
            message = "Aircraft not found.";
            messageColor = AMS_RED;
        }
        else {
            aircraftList[index].completeMaintenance();
            saveToFile();

            message = "Maintenance completed. Aircraft restored.";
            messageColor = AMS_GREEN;
            resetInputs();
        }
    }

    DrawText(message.c_str(), 60, 300, 22, messageColor);
}

// ================= HISTORY PAGE =================

void AircraftModule::drawHistoryPage(AircraftAssets assets) {
    drawHeader(assets, "Aircraft Flight History");

    textBox({ 60, 160, 320, 50 }, inputId, activeId, "Aircraft ID");

    if (button({ 410, 160, 170, 50 }, "LOAD", AMS_BLUE, AMS_LIGHT_BLUE, AMS_WHITE)) {
        selectedAircraftIndex = findAircraft(inputId);

        if (selectedAircraftIndex == -1) {
            message = "Aircraft not found.";
            messageColor = AMS_RED;
        }
        else {
            message = "History loaded.";
            messageColor = AMS_GREEN;
        }
    }

    DrawText(message.c_str(), 610, 175, 20, messageColor);

    if (selectedAircraftIndex != -1) {
        vector<string> history = aircraftList[selectedAircraftIndex].getHistory();

        DrawRectangleRounded({ 60, 250, 1080, 370 }, 0.12f, 12, AMS_CARD);
        DrawRectangleRoundedLinesEx({ 60, 250, 1080, 370 }, 0.12f, 12, 2.0f, AMS_BLUE);

        DrawText("Journey / Maintenance History", 90, 270, 24, AMS_YELLOW);

        if (history.empty()) {
            DrawText("No history found for this aircraft.", 90, 320, 22, AMS_WHITE);
        }
        else {
            int y = 320;

            for (int i = 0; i < (int)history.size() && i < 10; i++) {
                string line = to_string(i + 1) + ". " + history[i];
                DrawText(line.c_str(), 90, y, 18, AMS_WHITE);
                y += 30;
            }
        }
    }
}

// ================= DELETE PAGE =================

void AircraftModule::drawDeletePage(AircraftAssets assets) {
    drawHeader(assets, "Delete Aircraft");

    textBox({ 60, 170, 320, 50 }, inputId, activeId, "Aircraft ID");

    if (button({ 410, 170, 180, 50 }, "LOAD", AMS_BLUE, AMS_LIGHT_BLUE, AMS_WHITE)) {
        selectedAircraftIndex = findAircraft(inputId);

        if (selectedAircraftIndex == -1) {
            message = "Aircraft not found.";
            messageColor = AMS_RED;
        }
        else {
            message = "Aircraft loaded. Press delete to confirm.";
            messageColor = AMS_YELLOW;
        }
    }

    if (selectedAircraftIndex != -1) {
        drawAircraftCard(assets, aircraftList[selectedAircraftIndex], { 60, 280, 1080, 120 });

        if (button({ 60, 460, 230, 55 }, "DELETE AIRCRAFT", AMS_RED, { 255, 90, 100, 255 }, AMS_WHITE)) {
            aircraftList.erase(aircraftList.begin() + selectedAircraftIndex);
            saveToFile();

            selectedAircraftIndex = -1;
            message = "Aircraft deleted successfully.";
            messageColor = AMS_GREEN;
            resetInputs();
        }
    }

    DrawText(message.c_str(), 60, 560, 22, messageColor);
}

// ================= AIRCRAFT CARD =================

void AircraftModule::drawAircraftCard(AircraftAssets assets, Aircraft aircraft, Rectangle rect) {
    DrawRectangleRounded(rect, 0.12f, 12, AMS_CARD);
    DrawRectangleRoundedLinesEx(rect, 0.12f, 12, 2.0f, aircraft.canFly() ? AMS_BLUE : AMS_RED);

    Texture2D plane = getPlaneTexture(assets, aircraft.getType());

    if (plane.id > 0) {
        DrawTexturePro(
            plane,
            { 0, 0, (float)plane.width, (float)plane.height },
            { rect.x + 18, rect.y + 15, 110, rect.height - 30 },
            { 0, 0 },
            0,
            WHITE
        );
    }

    DrawText(aircraft.getId().c_str(), rect.x + 150, rect.y + 17, 24, AMS_YELLOW);
    DrawText(aircraft.getModel().c_str(), rect.x + 150, rect.y + 48, 18, AMS_WHITE);

    string typeLine = "Type: " + aircraft.getType();
    DrawText(typeLine.c_str(), rect.x + 150, rect.y + 72, 17, AMS_LIGHT_BLUE);

    string stats =
        "Capacity: " + to_string(aircraft.getCapacity()) +
        "   Fuel: " + to_string((int)aircraft.getFuel()) + "%" +
        "   Condition: " + to_string((int)aircraft.getCondition()) + "%" +
        "   Damage: " + to_string((int)aircraft.getDamage()) + "%";

    DrawText(stats.c_str(), rect.x + 390, rect.y + 25, 18, AMS_WHITE);

    string hours =
        "Total Hours: " + to_string((int)aircraft.getTotalFlightHours()) +
        "   Since Maintenance: " + to_string((int)aircraft.getHoursSinceMaintenance()) +
        "/" + to_string((int)aircraft.getMaintenanceLimit());

    DrawText(hours.c_str(), rect.x + 390, rect.y + 55, 18, AMS_WHITE);

    string status = "Status: " + aircraft.getStatusText();
    DrawText(status.c_str(), rect.x + 390, rect.y + 82, 18, aircraft.canFly() ? AMS_GREEN : AMS_RED);

    if (aircraft.getMaintenanceStatus() && assets.maintenance.id > 0) {
        DrawTexturePro(
            assets.maintenance,
            { 0, 0, (float)assets.maintenance.width, (float)assets.maintenance.height },
            { rect.x + rect.width - 105, rect.y + 18, 60, 60 },
            { 0, 0 },
            0,
            WHITE
        );
    }
    else if (!aircraft.canFly() && assets.technicalWarning.id > 0) {
        DrawTexturePro(
            assets.technicalWarning,
            { 0, 0, (float)assets.technicalWarning.width, (float)assets.technicalWarning.height },
            { rect.x + rect.width - 105, rect.y + 18, 60, 60 },
            { 0, 0 },
            0,
            WHITE
        );
    }
}

Texture2D AircraftModule::getPlaneTexture(AircraftAssets assets, string type) {
    if (type == "Passenger") {
        return assets.passengerPlane;
    }

    if (type == "Cargo") {
        return assets.cargoPlane;
    }

    if (type == "Private") {
        return assets.privatePlane;
    }

    return assets.passengerPlane;
}

// ================= HELPERS =================

void AircraftModule::resetInputs() {
    inputId = "";
    inputModel = "";
    inputCapacity = "";
    inputFuel = "";
    inputCondition = "";
    inputLimit = "";
    inputAmount = "";
    inputRoute = "";
    inputHours = "";
    inputFuelUsed = "";
    inputReason = "";

    activeId = false;
    activeModel = false;
    activeCapacity = false;
    activeFuel = false;
    activeCondition = false;
    activeLimit = false;
    activeAmount = false;
    activeRoute = false;
    activeHours = false;
    activeFuelUsed = false;
    activeReason = false;

    selectedAircraftIndex = -1;
}

int AircraftModule::findAircraft(string id) {
    for (int i = 0; i < (int)aircraftList.size(); i++) {
        if (aircraftList[i].getId() == id) {
            return i;
        }
    }

    return -1;
}

void AircraftModule::saveToFile() {
    ofstream file("aircraft.txt");

    for (int i = 0; i < (int)aircraftList.size(); i++) {
        file << aircraftList[i].serialize() << endl;
    }

    file.close();
}

void AircraftModule::loadFromFile() {
    aircraftList.clear();

    ifstream file("aircraft.txt");

    if (!file) {
        return;
    }

    string line;

    while (getline(file, line)) {
        if (line.length() > 0) {
            Aircraft aircraft;
            aircraft.deserialize(line);
            aircraftList.push_back(aircraft);
        }
    }

    file.close();
}