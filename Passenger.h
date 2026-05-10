#ifndef PASSENGER_H
#define PASSENGER_H

#include "raylib.h"
#include <string>
#include <vector>
using namespace std;

// ===================== PASSENGER ASSETS =====================

struct PassengerAssets {
    Texture2D passengerIcon;
    Texture2D ticket;
    Texture2D baggage;
    Texture2D meal;
    Texture2D seat;
};

// ===================== PASSENGER BAGGAGE CLASS =====================

class PassengerBaggage {
private:
    string bagTag;
    float weight;

public:
    PassengerBaggage();
    PassengerBaggage(string bagTag, float weight);

    string getBagTag() const;
    float getWeight() const;

    string serialize() const;
    void deserialize(string text);
};

// ===================== PASSENGER BOOKING CLASS =====================

class PassengerBooking {
private:
    string flightNumber;
    string route;
    string seatNumber;
    string mealPreference;
    string ticketStatus;

public:
    PassengerBooking();
    PassengerBooking(
        string flightNumber,
        string route,
        string seatNumber,
        string mealPreference,
        string ticketStatus
    );

    string getFlightNumber() const;
    string getRoute() const;
    string getSeatNumber() const;
    string getMealPreference() const;
    string getTicketStatus() const;

    string serialize() const;
    void deserialize(string text);
};

// ===================== PASSENGER RECORD CLASS =====================

class PassengerRecord {
private:
    string passengerID;
    string fullName;
    string passportNumber;
    string nationality;
    string phoneNumber;
    string email;

    vector<PassengerBooking> bookings;
    vector<PassengerBaggage> baggageList;

public:
    PassengerRecord();
    PassengerRecord(
        string passengerID,
        string fullName,
        string passportNumber,
        string nationality,
        string phoneNumber,
        string email
    );

    string getPassengerID() const;
    string getFullName() const;
    string getPassportNumber() const;
    string getNationality() const;
    string getPhoneNumber() const;
    string getEmail() const;

    vector<PassengerBooking> getBookings() const;
    vector<PassengerBaggage> getBaggageList() const;

    void setFullName(string value);
    void setPassportNumber(string value);
    void setNationality(string value);
    void setPhoneNumber(string value);
    void setEmail(string value);

    void addBooking(PassengerBooking booking);
    void addBaggage(PassengerBaggage baggage);

    string serialize() const;
    void deserialize(string line);
};

// ===================== PASSENGER MODULE CLASS =====================

class PassengerModule {
private:
    vector<PassengerRecord> passengers;

    enum Page {
        MENU,
        ADD,
        VIEW,
        SEARCH,
        UPDATE,
        BOOK_FLIGHT,
        ADD_BAGGAGE,
        REPORT
    };

    Page currentPage;

    string message;
    Color messageColor;

    string inputID;
    string inputName;
    string inputPassport;
    string inputNationality;
    string inputPhone;
    string inputEmail;

    string inputFlight;
    string inputRoute;
    string inputSeat;
    string inputMeal;
    string inputTicketStatus;

    string inputBagTag;
    string inputBagWeight;

    bool activeID;
    bool activeName;
    bool activePassport;
    bool activeNationality;
    bool activePhone;
    bool activeEmail;

    bool activeFlight;
    bool activeRoute;
    bool activeSeat;
    bool activeMeal;
    bool activeTicketStatus;

    bool activeBagTag;
    bool activeBagWeight;

    int selectedPassengerIndex;

public:
    PassengerModule();

    bool Draw(PassengerAssets assets, Texture2D logo, Texture2D background);

    void loadFromFile();
    void saveToFile();

private:
    void drawBackground(Texture2D background);
    void drawHeader(Texture2D logo, const char* title);

    void drawMenu(PassengerAssets assets, Texture2D logo, Texture2D background);
    void drawAddPage(PassengerAssets assets, Texture2D logo, Texture2D background);
    void drawViewPage(PassengerAssets assets, Texture2D logo, Texture2D background);
    void drawSearchPage(PassengerAssets assets, Texture2D logo, Texture2D background);
    void drawUpdatePage(PassengerAssets assets, Texture2D logo, Texture2D background);
    void drawBookFlightPage(PassengerAssets assets, Texture2D logo, Texture2D background);
    void drawAddBaggagePage(PassengerAssets assets, Texture2D logo, Texture2D background);
    void drawReportPage(PassengerAssets assets, Texture2D logo, Texture2D background);

    bool button(Rectangle rect, const char* text, Color color, Color hoverColor, Color textColor);
    void textBox(Rectangle rect, string& text, bool& active, const char* label);
    void drawIcon(Texture2D texture, Rectangle rect);
    void drawPassengerCard(PassengerAssets assets, PassengerRecord passenger, Rectangle rect);

    void resetInputs();
    int findPassenger(string passengerID);
};

#endif