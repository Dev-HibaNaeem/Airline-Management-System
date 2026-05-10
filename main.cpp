#include "raylib.h"
#include "Aircraft.h"
#include "Flight.h"
#include "Passenger.h"
#include "Pilot.h"
#include "CabinCrew.h"
#include "Airport.h"

#include <iostream>
#include <string>
#include <vector>
using namespace std;

// ===================== SCREEN SETTINGS =====================

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 750;

// ===================== THEME COLORS =====================

Color NAVY_BLUE = { 8, 28, 58, 255 };
Color DARK_NAVY = { 4, 18, 38, 255 };
Color BLUE_THEME = { 25, 118, 210, 255 };
Color LIGHT_BLUE = { 83, 169, 255, 255 };
Color YELLOW_THEME = { 255, 193, 7, 255 };
Color WHITE_THEME = { 245, 248, 255, 255 };
Color CARD_COLOR = { 16, 42, 82, 255 };
Color CARD_HOVER = { 28, 73, 130, 255 };
Color RED_THEME = { 230, 57, 70, 255 };
Color GREEN_THEME = { 46, 204, 113, 255 };
Color INPUT_BG = { 242, 246, 252, 255 };

// ===================== RAYLIB BORDER FIX =====================

void DrawRoundedBorder(Rectangle rect, float roundness, int segments, float thickness, Color color) {
    DrawRectangleRoundedLines(rect, roundness, segments, color);
}

// ===================== APP STATES =====================

enum ScreenState {
    WELCOME,
    LOGIN,
    DASHBOARD,
    MODULE_DETAIL
};

ScreenState currentScreen = WELCOME;

// ===================== MODULE STRUCTURE =====================

struct Module {
    string title;
    string subtitle;
    string description;
};

vector<Module> modules = {
    {
        "Aircraft",
        "Fleet Management",
        "Maintain aircraft records, aircraft type, capacity, fuel status, diagnostics, and airworthiness."
    },
    {
        "Flight",
        "Flight Operations",
        "Manage routes, timings, gates, live status, delays, aircraft, pilot, cabin crew, and passenger relations."
    },
    {
        "Passenger",
        "Passenger Records",
        "Store passenger information, flight bookings, meal choices, ticket status, and passenger details."
    },
    {
        "Pilot",
        "Cockpit Crew",
        "Manage pilot records, licenses, ranks, ratings, flight hours, rest hours, and flight assignments."
    },
    {
        "Cabin Crew",
        "Cabin Operations",
        "Manage cabin crew profiles, duty hours, languages, training status, rest hours, and flight assignments."
    },
    {
        "Airport",
        "Airport Operations",
        "Manage gates, runways, ready flights, airport assignments, arrivals, and departures."
    },
    {
        "Cargo",
        "Cargo Management",
        "Manage cargo records, cargo weight, sender details, destination, flight connection, and delivery status."
    }
};

int selectedModule = -1;

// ===================== MODULE OBJECTS =====================

AircraftModule aircraftModule;
FlightModule flightModule;
PassengerModule passengerModule;
PilotModule pilotModule;
CabinCrewModule cabinCrewModule;
AirportModule airportModule;

// ===================== LOGIN DATA =====================

string username = "";
string password = "";
bool usernameActive = false;
bool passwordActive = false;
string loginMessage = "";
Color loginMessageColor = WHITE_THEME;

// ===================== PLANE ANIMATION =====================

float planeX1 = -250;
float planeX2 = SCREEN_WIDTH + 250;

// ===================== TEXT HELPER =====================

void DrawTextCentered(const char* text, float centerX, float y, int fontSize, Color color) {
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, (int)(centerX - textWidth / 2), (int)y, fontSize, color);
}

// ===================== BUTTON FUNCTION =====================

bool DrawButton(Rectangle rect, const char* text, Color normalColor, Color hoverColor, Color textColor) {
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, rect);

    DrawRectangleRounded(rect, 0.22f, 14, hover ? hoverColor : normalColor);
    DrawRoundedBorder(rect, 0.22f, 14, 2.0f, YELLOW_THEME);

    int fontSize = 21;
    int textWidth = MeasureText(text, fontSize);

    DrawText(
        text,
        (int)(rect.x + rect.width / 2 - textWidth / 2),
        (int)(rect.y + rect.height / 2 - fontSize / 2),
        fontSize,
        textColor
    );

    return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// ===================== TEXTBOX FUNCTION =====================

void DrawTextBox(Rectangle box, string& text, bool& active, bool isPassword) {
    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        active = CheckCollisionPointRec(mouse, box);
    }

    Color borderColor = active ? YELLOW_THEME : BLUE_THEME;

    DrawRectangleRounded(box, 0.15f, 10, INPUT_BG);
    DrawRoundedBorder(box, 0.15f, 10, 2.0f, borderColor);

    string displayText = text;

    if (isPassword) {
        displayText = "";

        for (int i = 0; i < (int)text.length(); i++) {
            displayText += "*";
        }
    }

    DrawText(displayText.c_str(), (int)box.x + 16, (int)box.y + 15, 22, DARK_NAVY);

    if (active) {
        int key = GetCharPressed();

        while (key > 0) {
            if ((key >= 32) && (key <= 125) && text.length() < 24) {
                text += (char)key;
            }

            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && text.length() > 0) {
            text.pop_back();
        }
    }
}

// ===================== PLANE SHAPE BACKGROUND =====================

void DrawPlaneShape(float x, float y, float scale, Color color) {
    DrawTriangle(
        { x, y },
        { x - 80 * scale, y + 25 * scale },
        { x - 80 * scale, y - 25 * scale },
        color
    );

    DrawRectangle(
        (int)(x - 115 * scale),
        (int)(y - 10 * scale),
        (int)(55 * scale),
        (int)(20 * scale),
        color
    );

    DrawTriangle(
        { x - 90 * scale, y },
        { x - 145 * scale, y - 38 * scale },
        { x - 130 * scale, y },
        color
    );

    DrawTriangle(
        { x - 90 * scale, y },
        { x - 145 * scale, y + 38 * scale },
        { x - 130 * scale, y },
        color
    );

    DrawTriangle(
        { x - 115 * scale, y - 10 * scale },
        { x - 155 * scale, y - 35 * scale },
        { x - 140 * scale, y - 5 * scale },
        color
    );
}

// ===================== BACKGROUND =====================

void DrawMovingPlaneBackground(Texture2D background) {
    ClearBackground(DARK_NAVY);

    if (background.id > 0) {
        DrawTexturePro(
            background,
            { 0, 0, (float)background.width, (float)background.height },
            { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT },
            { 0, 0 },
            0,
            Fade(WHITE, 0.35f)
        );
    }

    DrawRectangleGradientV(
        0,
        0,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        Fade(NAVY_BLUE, 0.92f),
        Fade(DARK_NAVY, 0.97f)
    );

    for (int i = 0; i < 42; i++) {
        int x = (i * 97) % SCREEN_WIDTH;
        int y = (i * 53) % SCREEN_HEIGHT;
        DrawCircle(x, y, 2, Fade(WHITE_THEME, 0.35f));
    }

    planeX1 += 1.15f;

    if (planeX1 > SCREEN_WIDTH + 250) {
        planeX1 = -250;
    }

    planeX2 -= 0.85f;

    if (planeX2 < -250) {
        planeX2 = SCREEN_WIDTH + 250;
    }

    DrawPlaneShape(planeX1, 170, 1.1f, Fade(LIGHT_BLUE, 0.35f));
    DrawPlaneShape(planeX2, 520, 0.9f, Fade(YELLOW_THEME, 0.30f));

    DrawCircleGradient(950, 130, 180, Fade(BLUE_THEME, 0.22f), Fade(BLUE_THEME, 0.0f));
    DrawCircleGradient(180, 620, 230, Fade(YELLOW_THEME, 0.10f), Fade(YELLOW_THEME, 0.0f));
}

// ===================== LOGO =====================

void DrawLogo(Texture2D logo) {
    if (logo.id > 0) {
        Rectangle source = { 0, 0, (float)logo.width, (float)logo.height };
        Rectangle dest = { 26, 18, 72, 72 };
        Vector2 origin = { 0, 0 };

        DrawTexturePro(logo, source, dest, origin, 0, WHITE);
        DrawRoundedBorder({ 20, 12, 84, 84 }, 0.10f, 8, 2.0f, YELLOW_THEME);
    }
    else {
        DrawRectangleRounded({ 20, 12, 84, 84 }, 0.10f, 8, WHITE_THEME);
        DrawRoundedBorder({ 20, 12, 84, 84 }, 0.10f, 8, 2.0f, YELLOW_THEME);
        DrawText("AMS", 42, 45, 20, DARK_NAVY);
    }
}

// ===================== WELCOME SCREEN =====================

void DrawWelcomeScreen(AircraftAssets aircraftAssets) {
    DrawMovingPlaneBackground(aircraftAssets.background);
    DrawLogo(aircraftAssets.logo);

    DrawTextCentered("AIRLINE", SCREEN_WIDTH / 2, 130, 68, WHITE_THEME);
    DrawTextCentered("MANAGEMENT SYSTEM", SCREEN_WIDTH / 2, 215, 58, YELLOW_THEME);

    Rectangle welcomeBox = { 210, 335, 780, 180 };

    DrawRectangleRounded(welcomeBox, 0.12f, 18, Fade(CARD_COLOR, 0.93f));
    DrawRoundedBorder(welcomeBox, 0.12f, 18, 2.0f, YELLOW_THEME);

    DrawTextCentered("Integrated Airline Operations Console", SCREEN_WIDTH / 2, 375, 30, WHITE_THEME);

    DrawTextCentered(
        "Connected aircraft, flight, passenger, pilot, cabin crew, airport, and cargo records",
        SCREEN_WIDTH / 2,
        430,
        18,
        Fade(WHITE_THEME, 0.88f)
    );

    DrawTextCentered(
        "through file handling and automatic ID generation.",
        SCREEN_WIDTH / 2,
        462,
        18,
        Fade(WHITE_THEME, 0.88f)
    );

    if (DrawButton({ 470, 585, 260, 60 }, "START SYSTEM", BLUE_THEME, LIGHT_BLUE, WHITE_THEME)) {
        currentScreen = LOGIN;
    }
}

// ===================== LOGIN SCREEN =====================

void DrawLoginScreen(AircraftAssets aircraftAssets) {
    DrawMovingPlaneBackground(aircraftAssets.background);
    DrawLogo(aircraftAssets.logo);

    Rectangle loginBox = { 355, 120, 490, 500 };

    DrawRectangleRounded(loginBox, 0.08f, 15, Fade(CARD_COLOR, 0.96f));
    DrawRoundedBorder(loginBox, 0.08f, 15, 2.0f, YELLOW_THEME);

    DrawTextCentered("ADMIN LOGIN", SCREEN_WIDTH / 2, 165, 34, YELLOW_THEME);
    DrawTextCentered("Sign in to access the management dashboard", SCREEN_WIDTH / 2, 215, 18, Fade(WHITE_THEME, 0.86f));

    DrawText("Username", 420, 285, 20, WHITE_THEME);
    DrawTextBox({ 420, 315, 360, 55 }, username, usernameActive, false);

    DrawText("Password", 420, 390, 20, WHITE_THEME);
    DrawTextBox({ 420, 420, 360, 55 }, password, passwordActive, true);

    if (DrawButton({ 470, 510, 260, 55 }, "LOGIN", BLUE_THEME, LIGHT_BLUE, WHITE_THEME)) {
        if (username == "admin" && password == "1234") {
            loginMessage = "Access granted. Loading dashboard...";
            loginMessageColor = GREEN_THEME;
            currentScreen = DASHBOARD;
        }
        else {
            loginMessage = "Access denied. Please check your credentials.";
            loginMessageColor = RED_THEME;
        }
    }

    DrawTextCentered(loginMessage.c_str(), SCREEN_WIDTH / 2, 580, 18, loginMessageColor);
}

// ===================== TOP BAR =====================

void DrawTopBar(Texture2D logo, const char* title) {
    DrawRectangle(0, 0, SCREEN_WIDTH, 96, NAVY_BLUE);
    DrawRectangle(0, 93, SCREEN_WIDTH, 3, YELLOW_THEME);

    DrawLogo(logo);

    DrawText(title, 125, 33, 31, WHITE_THEME);

    if (DrawButton({ 1010, 25, 150, 46 }, "LOGOUT", RED_THEME, { 255, 90, 100, 255 }, WHITE_THEME)) {
        username = "";
        password = "";
        loginMessage = "";
        currentScreen = LOGIN;
    }
}

// ===================== DASHBOARD SCREEN =====================

void DrawDashboard(AircraftAssets aircraftAssets) {
    ClearBackground(DARK_NAVY);

    DrawRectangleGradientV(
        0,
        0,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        DARK_NAVY,
        NAVY_BLUE
    );

    DrawTopBar(aircraftAssets.logo, "Admin Dashboard");

    DrawText("Management Modules", 40, 125, 29, YELLOW_THEME);
    DrawText("Active airline project modules are shown here.", 40, 162, 18, Fade(WHITE_THEME, 0.78f));

    int startX = 70;
    int startY = 220;
    int cardW = 320;
    int cardH = 120;
    int gapX = 45;
    int gapY = 35;

    for (int i = 0; i < (int)modules.size(); i++) {
        int row = i / 3;
        int col = i % 3;

        Rectangle card = {
            (float)(startX + col * (cardW + gapX)),
            (float)(startY + row * (cardH + gapY)),
            (float)cardW,
            (float)cardH
        };

        Vector2 mouse = GetMousePosition();
        bool hover = CheckCollisionPointRec(mouse, card);

        DrawRectangleRounded(card, 0.14f, 12, hover ? CARD_HOVER : CARD_COLOR);
        DrawRoundedBorder(card, 0.14f, 12, 2.0f, hover ? YELLOW_THEME : BLUE_THEME);

        string number = to_string(i + 1);

        DrawCircle(card.x + 32, card.y + 34, 19, YELLOW_THEME);
        DrawText(number.c_str(), (int)card.x + 24, (int)card.y + 22, 18, DARK_NAVY);

        DrawText(modules[i].title.c_str(), (int)card.x + 65, (int)card.y + 22, 22, WHITE_THEME);
        DrawText(modules[i].subtitle.c_str(), (int)card.x + 65, (int)card.y + 55, 16, LIGHT_BLUE);
        DrawText("Open Management Panel", (int)card.x + 65, (int)card.y + 84, 15, Fade(WHITE_THEME, 0.75f));

        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selectedModule = i;
            currentScreen = MODULE_DETAIL;
        }
    }
}

// ===================== COMING SOON MODULE =====================

void DrawComingSoonModule(AircraftAssets aircraftAssets, string title, string subtitle) {
    ClearBackground(DARK_NAVY);

    DrawRectangleGradientV(
        0,
        0,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        DARK_NAVY,
        NAVY_BLUE
    );

    DrawTopBar(aircraftAssets.logo, title.c_str());

    Rectangle box = { 230, 210, 740, 300 };

    DrawRectangleRounded(box, 0.12f, 18, Fade(CARD_COLOR, 0.95f));
    DrawRoundedBorder(box, 0.12f, 18, 2.0f, YELLOW_THEME);

    DrawTextCentered(title.c_str(), SCREEN_WIDTH / 2, 265, 42, YELLOW_THEME);
    DrawTextCentered(subtitle.c_str(), SCREEN_WIDTH / 2, 335, 22, WHITE_THEME);

    DrawTextCentered(
        "This dashboard module is added successfully.",
        SCREEN_WIDTH / 2,
        390,
        20,
        Fade(WHITE_THEME, 0.85f)
    );

    DrawTextCentered(
        "Its .h and .cpp files can be created later for full functionality.",
        SCREEN_WIDTH / 2,
        425,
        18,
        Fade(WHITE_THEME, 0.75f)
    );

    if (DrawButton({ 470, 560, 260, 55 }, "BACK DASHBOARD", YELLOW_THEME, { 255, 210, 70, 255 }, DARK_NAVY)) {
        currentScreen = DASHBOARD;
    }
}

// ===================== MODULE DETAIL SCREEN =====================

void DrawModuleDetail(
    AircraftAssets aircraftAssets,
    PassengerAssets passengerAssets,
    Texture2D gateIcon
) {
    // Card 1: Aircraft
    if (selectedModule == 0) {
        bool backToDashboard = aircraftModule.Draw(aircraftAssets);

        if (backToDashboard) {
            currentScreen = DASHBOARD;
        }

        return;
    }

    // Card 2: Flight
    if (selectedModule == 1) {
        bool backToDashboard = flightModule.Draw(
            aircraftAssets.logo,
            aircraftAssets.background,
            aircraftAssets.passengerPlane,
            aircraftAssets.technicalWarning
        );

        if (backToDashboard) {
            currentScreen = DASHBOARD;
        }

        return;
    }

    // Card 3: Passenger
    if (selectedModule == 2) {
        bool backToDashboard = passengerModule.Draw(
            passengerAssets,
            aircraftAssets.logo,
            aircraftAssets.background
        );

        if (backToDashboard) {
            currentScreen = DASHBOARD;
        }

        return;
    }

    // Card 4: Pilot
    if (selectedModule == 3) {
        bool backToDashboard = pilotModule.Draw(
            aircraftAssets.logo,
            aircraftAssets.background,
            aircraftAssets.crewIcon,
            aircraftAssets.technicalWarning
        );

        if (backToDashboard) {
            currentScreen = DASHBOARD;
        }

        return;
    }

    // Card 5: Cabin Crew
    if (selectedModule == 4) {
        bool backToDashboard = cabinCrewModule.Draw(
            aircraftAssets.logo,
            aircraftAssets.background,
            aircraftAssets.crewIcon,
            aircraftAssets.technicalWarning
        );

        if (backToDashboard) {
            currentScreen = DASHBOARD;
        }

        return;
    }

    // Card 6: Airport
    if (selectedModule == 5) {
        bool backToDashboard = airportModule.Draw(
            aircraftAssets.logo,
            aircraftAssets.background,
            gateIcon,
            aircraftAssets.passengerPlane
        );

        if (backToDashboard) {
            currentScreen = DASHBOARD;
        }

        return;
    }

    // Card 7: Cargo
    if (selectedModule == 6) {
        DrawComingSoonModule(
            aircraftAssets,
            "Cargo Management",
            "Cargo records, weight, sender details, destination, and delivery status."
        );

        return;
    }

    currentScreen = DASHBOARD;
}

// ===================== SAFE UNLOAD FUNCTION =====================

void SafeUnloadTexture(Texture2D texture) {
    if (texture.id > 0) {
        UnloadTexture(texture);
    }
}

// ===================== MAIN FUNCTION =====================

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Airline Management System");
    SetTargetFPS(60);

    // ===================== LOAD AIRCRAFT / COMMON ASSETS =====================

    AircraftAssets aircraftAssets;

    aircraftAssets.logo = LoadTexture("assets/logo.png");
    aircraftAssets.passengerPlane = LoadTexture("assets/passenger_plane.png");
    aircraftAssets.cargoPlane = LoadTexture("assets/cargo_plane.png");
    aircraftAssets.privatePlane = LoadTexture("assets/private_plane.png");
    aircraftAssets.background = LoadTexture("assets/background.png");
    aircraftAssets.maintenance = LoadTexture("assets/maintainance.png");
    aircraftAssets.technicalWarning = LoadTexture("assets/technical_warning.png");
    aircraftAssets.crewIcon = LoadTexture("assets/crew_icon.png");

    // ===================== LOAD PASSENGER ASSETS =====================

    PassengerAssets passengerAssets;

    passengerAssets.passengerIcon = LoadTexture("assets/passenger_icon.png");
    passengerAssets.ticket = LoadTexture("assets/ticket.png");
    passengerAssets.baggage = LoadTexture("assets/baggage.png");
    passengerAssets.meal = LoadTexture("assets/meal.png");
    passengerAssets.seat = LoadTexture("assets/seat.png");

    // ===================== LOAD AIRPORT ASSET =====================

    Texture2D gateIcon = LoadTexture("assets/gate_icon.png");

    // ===================== MAIN LOOP =====================

    while (!WindowShouldClose()) {
        BeginDrawing();

        if (currentScreen == WELCOME) {
            DrawWelcomeScreen(aircraftAssets);
        }
        else if (currentScreen == LOGIN) {
            DrawLoginScreen(aircraftAssets);
        }
        else if (currentScreen == DASHBOARD) {
            DrawDashboard(aircraftAssets);
        }
        else if (currentScreen == MODULE_DETAIL) {
            DrawModuleDetail(aircraftAssets, passengerAssets, gateIcon);
        }

        EndDrawing();
    }

    // ===================== UNLOAD AIRCRAFT / COMMON ASSETS =====================

    SafeUnloadTexture(aircraftAssets.logo);
    SafeUnloadTexture(aircraftAssets.passengerPlane);
    SafeUnloadTexture(aircraftAssets.cargoPlane);
    SafeUnloadTexture(aircraftAssets.privatePlane);
    SafeUnloadTexture(aircraftAssets.background);
    SafeUnloadTexture(aircraftAssets.maintenance);
    SafeUnloadTexture(aircraftAssets.technicalWarning);
    SafeUnloadTexture(aircraftAssets.crewIcon);

    // ===================== UNLOAD PASSENGER ASSETS =====================

    SafeUnloadTexture(passengerAssets.passengerIcon);
    SafeUnloadTexture(passengerAssets.ticket);
    SafeUnloadTexture(passengerAssets.baggage);
    SafeUnloadTexture(passengerAssets.meal);
    SafeUnloadTexture(passengerAssets.seat);

    // ===================== UNLOAD AIRPORT ASSET =====================

    SafeUnloadTexture(gateIcon);

    CloseWindow();

    return 0;
}