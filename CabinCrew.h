#ifndef CABIN_CREW_H
#define CABIN_CREW_H

#include "raylib.h"
#include <string>
#include <vector>
using namespace std;

class CabinCrew {
private:
    string crewID;
    string fullName;
    string role;
    string languages;

    float totalDutyHours;
    float hoursSinceRest;
    float lastRestHours;

    bool available;

public:
    CabinCrew();

    CabinCrew(
        string crewID,
        string fullName,
        string role,
        string languages,
        float totalDutyHours,
        float hoursSinceRest,
        float lastRestHours
    );

    string getCrewID() const;
    string getFullName() const;
    string getRole() const;
    string getLanguages() const;

    float getTotalDutyHours() const;
    float getHoursSinceRest() const;
    float getLastRestHours() const;

    bool isAvailable() const;

    void setFullName(string value);
    void setRole(string value);
    void setLanguages(string value);

    void addDutyHours(float hours);
    void addRestHours(float hours);

    bool hasEnoughRest() const;
    bool speaksLanguage(string language) const;

    string getStatusText() const;

    string serialize() const;
    void deserialize(string line);
};

class CabinCrewModule {
private:
    vector<CabinCrew> crewList;

    enum Page {
        MENU,
        ADD,
        VIEW,
        SEARCH,
        UPDATE,
        RECORD_REST,
        ASSIGN_FLIGHT,
        EMERGENCY,
        REPORT
    };

    Page currentPage;

    string message;
    Color messageColor;

    string inputCrewID;
    string inputName;
    string inputRole;
    string inputLanguages;

    string inputTotalHours;
    string inputRestHours;

    string inputFlightNumber;
    string inputRequiredStaff;
    string inputRequiredLanguage;
    string inputPlannedHours;

    string inputEmergencyType;
    string inputEmergencyNote;

    bool activeCrewID;
    bool activeName;
    bool activeRole;
    bool activeLanguages;

    bool activeTotalHours;
    bool activeRestHours;

    bool activeFlightNumber;
    bool activeRequiredStaff;
    bool activeRequiredLanguage;
    bool activePlannedHours;

    bool activeEmergencyType;
    bool activeEmergencyNote;

    int selectedCrewIndex;

public:
    CabinCrewModule();

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
    void drawEmergencyPage(Texture2D logo, Texture2D background, Texture2D crewIcon, Texture2D warningIcon);
    void drawReportPage(Texture2D logo, Texture2D background);

    bool button(Rectangle rect, const char* text, Color color, Color hoverColor, Color textColor);
    void textBox(Rectangle rect, string& text, bool& active, const char* label);
    void drawIcon(Texture2D texture, Rectangle rect);
    void drawCrewCard(CabinCrew crew, Rectangle rect, Texture2D crewIcon, Texture2D warningIcon);

    void resetInputs();
    int findCrew(string crewID);

    int safeToInt(string text);
    float safeToFloat(string text);

    int countAvailableCrew(string requiredLanguage);
    vector<int> getAvailableCrewIndexes(string requiredLanguage, int requiredCount);
};

#endif