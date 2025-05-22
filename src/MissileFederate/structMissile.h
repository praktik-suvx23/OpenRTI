#ifndef STRUCTMISSILE_H
#define STRUCTMISSILE_H

#include <RTI/RTI1516.h>
#include <string>

struct Missile {
    // Core identifiers
    rti1516e::ObjectInstanceHandle objectInstanceHandle;
    std::wstring id;
    std::wstring team;

    // Flight data
    std::pair<double, double> position;
    double altitude;
    double speed;
    double bearing;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    double distanceToTarget;
    double groundDistanceToTarget;

    // Targeting data
    std::wstring targetID;
    std::wstring initialTargetID;
    std::pair<double, double> initialTargetPosition;
    double initialDistanceToTarget;

    // Status flags
    bool heightAchieved;
    bool targetFound;
    bool lookingForTarget;
    bool targetDestroyed;

    Missile()
        : id(L""),
        team(L""),
        position({0.0, 0.0}),
        altitude(0.0),
        speed(0.0),
        bearing(0.0),
        startTime(std::chrono::high_resolution_clock::now()),
        distanceToTarget(0.0),
        groundDistanceToTarget(0.0),
        targetID(L""),
        initialTargetID(L""),
        initialTargetPosition({0.0, 0.0}),
        initialDistanceToTarget(0.0),
        heightAchieved(false),
        targetFound(false),
        lookingForTarget(true),
        targetDestroyed(false) {}
};

struct TargetShips {
    rti1516e::ObjectInstanceHandle objectInstanceHandle;
    std::wstring structShipID;
    std::wstring structShipTeam;
    double structShipSize;
    int numberOfMissilesTargeting;

    TargetShips(rti1516e::ObjectInstanceHandle objectInstanceHandle)
        : objectInstanceHandle(objectInstanceHandle),
        structShipID(L""),
        structShipTeam(L""), 
        structShipSize(0.0),
        numberOfMissilesTargeting(0) {}
};

#endif