#ifndef PILOT_H
#define PILOT_H

#include "raylib.h"
#include <string>
#include <vector>
using namespace std;

// ===================== PILOT CLASS =====================

class Pilot {
private:
    string pilotID;
    string fullName;
    string rank;
    string licenseType;

    float totalFlightHours;
    float hoursSinceRest;
    float lastRestHours;

    bool available;

public:
    Pilot();

    Pilot(
        string pilotID,
        string fullName,
        string rank,
        string licenseType,
        float totalFlightHours,
        float hoursSinceRest,
        float lastRestHours
    );

    string getPilotID() const;
    string getFullName() const;
    string getRank() const;
    string getLicenseType() const;

    float getTotalFlightHours() const;
    float getHoursSinceRest() const;
    float getLastRestHours() const;

    bool isAvailable() const;

    void setFullName(string value);
    void setRank(string value);
    void setLicenseType(string value);

    void addFlightHours(float hours);
    void addRestHours(float hours);

    bool hasEnoughRest() const;
    bool licenseMatches(string aircraftType) const;
    bool canAssign(string aircraftType, float plannedHours) const;

    string getStatusText() const;

    string serialize() const;
    void deserialize(string line);
};

// ===================== PILOT MODULE CLASS =====================

class PilotModule {
private:
    vector<Pilot> pilots;

    enum Page {
        MENU,
        ADD,
        VIEW,
        SEARCH,
        UPDATE,
        RECORD_REST,
        ASSIGN_FLIGHT,
        REPORT
    };

    Page currentPage;

    string message;
    Color messageColor;

    string inputPilotID;
    string inputName;
    string inputRank;
    string inputLicense;

    string inputTotalHours;
    string inputHoursSinceRest;
    string inputRestHours;

    string inputFlightNumber;
    string inputAircraftType;
    string inputPlannedHours;

    bool activePilotID;
    bool activeName;
    bool activeRank;
    bool activeLicense;

    bool activeTotalHours;
    bool activeHoursSinceRest;
    bool activeRestHours;

    bool activeFlightNumber;
    bool activeAircraftType;
    bool activePlannedHours;

    int selectedPilotIndex;

public:
    PilotModule();

    bool Draw(Texture2D logo, Texture2D background, Texture2D crewIcon, Texture2D warningIcon);

    void loadFromFile();
    void saveToFile();

private:
    void drawBackground(Texture2D background);
    void drawHeader(Texture2D logo, const char* title);

    void drawMenu(Texture2D logo, Texture2D background, Texture2D crewIcon, Texture2D warningIcon);
    void drawAddPage(Texture2D logo, Texture2D background, Texture2D crewIcon);
    void drawViewPage(Texture2D logo, Texture2D background, Texture2D crewIcon, Texture2D warningIcon);
    void drawSearchPage(Texture2D logo, Texture2D background, Texture2D crewIcon, Texture2D warningIcon);
    void drawUpdatePage(Texture2D logo, Texture2D background);
    void drawRestPage(Texture2D logo, Texture2D background, Texture2D crewIcon);
    void drawAssignFlightPage(Texture2D logo, Texture2D background, Texture2D crewIcon, Texture2D warningIcon);
    void drawReportPage(Texture2D logo, Texture2D background);

    bool button(Rectangle rect, const char* text, Color color, Color hoverColor, Color textColor);
    void textBox(Rectangle rect, string& text, bool& active, const char* label);
    void drawIcon(Texture2D texture, Rectangle rect);
    void drawPilotCard(Pilot pilot, Rectangle rect, Texture2D crewIcon, Texture2D warningIcon);

    void resetInputs();
    int findPilot(string pilotID);

    float safeToFloat(string text);
};

#endif