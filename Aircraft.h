#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include "raylib.h"
#include <string>
#include <vector>
using namespace std;

struct AircraftAssets {
    Texture2D logo;
    Texture2D passengerPlane;
    Texture2D cargoPlane;
    Texture2D privatePlane;
    Texture2D background;
    Texture2D maintenance;
    Texture2D technicalWarning;
    Texture2D crewIcon;
};
class Aircraft {
private:
    string id;
    string model;
    string type;
    int capacity;
    float fuel;
    float condition;
    float damage;
    float totalFlightHours;
    float hoursSinceMaintenance;
    float maintenanceLimit;
    bool inMaintenance;
    vector<string> flightHistory;

public:
    Aircraft();

    Aircraft(string id, string model, string type, int capacity, float fuel,
             float condition, float maintenanceLimit);

    string getId() const;
    string getModel() const;
    string getType() const;
    int getCapacity() const;
    float getFuel() const;
    float getCondition() const;
    float getDamage() const;
    float getTotalFlightHours() const;
    float getHoursSinceMaintenance() const;
    float getMaintenanceLimit() const;
    bool getMaintenanceStatus() const;

    void setModel(string newModel);
    void setCapacity(int newCapacity);
    void setFuel(float newFuel);
    void setCondition(float newCondition);
    void setMaintenanceLimit(float newLimit);

    bool canFly() const;
    string getStatusText() const;

    void refuel(float amount);
    void recordFlight(string route, float hours, float fuelUsed);
    void sendMaintenance(string reason);
    void completeMaintenance();

    vector<string> getHistory() const;

    string serialize() const;
    void deserialize(string line);
};

class AircraftModule {
private:
    vector<Aircraft> aircraftList;

    enum Page {
        MENU,
        ADD,
        VIEW,
        SEARCH,
        UPDATE,
        REFUEL,
        RECORD_FLIGHT,
        DIAGNOSTICS,
        SEND_MAINTENANCE,
        COMPLETE_MAINTENANCE,
        HISTORY,
        DELETE_AIRCRAFT
    };

    Page currentPage;

    string message;
    Color messageColor;

    string inputId;
    string inputModel;
    string inputCapacity;
    string inputFuel;
    string inputCondition;
    string inputLimit;
    string inputAmount;
    string inputRoute;
    string inputHours;
    string inputFuelUsed;
    string inputReason;

    int selectedType;

    bool activeId;
    bool activeModel;
    bool activeCapacity;
    bool activeFuel;
    bool activeCondition;
    bool activeLimit;
    bool activeAmount;
    bool activeRoute;
    bool activeHours;
    bool activeFuelUsed;
    bool activeReason;

    int selectedAircraftIndex;

public:
    AircraftModule();

    bool Draw(AircraftAssets assets);

    void loadFromFile();
    void saveToFile();

private:
    void drawBackground(AircraftAssets assets);
    void drawHeader(AircraftAssets assets, const char* title);
    void drawMenu(AircraftAssets assets);
    void drawAddPage(AircraftAssets assets);
    void drawViewPage(AircraftAssets assets);
    void drawSearchPage(AircraftAssets assets);
    void drawUpdatePage(AircraftAssets assets);
    void drawRefuelPage(AircraftAssets assets);
    void drawRecordFlightPage(AircraftAssets assets);
    void drawDiagnosticsPage(AircraftAssets assets);
    void drawSendMaintenancePage(AircraftAssets assets);
    void drawCompleteMaintenancePage(AircraftAssets assets);
    void drawHistoryPage(AircraftAssets assets);
    void drawDeletePage(AircraftAssets assets);

    bool button(Rectangle rect, const char* text, Color color, Color hoverColor, Color textColor);
    void textBox(Rectangle rect, string& text, bool& active, const char* label);
    void resetInputs();
    int findAircraft(string id);
    void drawAircraftCard(AircraftAssets assets, Aircraft aircraft, Rectangle rect);
    Texture2D getPlaneTexture(AircraftAssets assets, string type);
};

#endif