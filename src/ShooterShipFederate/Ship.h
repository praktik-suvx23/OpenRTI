#ifndef SHIP_H
#define SHIP_H

#include <string>
#include <RTI/RTI1516.h>

struct Ship {
    rti1516e::ObjectInstanceHandle objectInstanceHandle;
    std::wstring shipName;
    std::wstring shipTeam;
    std::pair<double, double> shipPosition;
    double shipSpeed;
    int numberOfMissiles;
    double HP;

    Ship(rti1516e::ObjectInstanceHandle objectInstanceHandle) 
    : objectInstanceHandle(objectInstanceHandle), 
    shipName(L""), 
    shipTeam(L""),
    shipPosition(std::make_pair(0.0, 0.0)), 
    shipSpeed(0), 
    numberOfMissiles(0), 
    HP(100.0) {}
};

struct EnemyShip {
    rti1516e::ObjectInstanceHandle objectInstanceHandle;
    std::wstring shipName;
    std::wstring shipTeam;
    std::pair<double, double> shipPosition;
    double shipSpeed;
    int numberOfMissiles;
    double HP;

    EnemyShip(rti1516e::ObjectInstanceHandle objectInstanceHandle) 
    : objectInstanceHandle(objectInstanceHandle), 
    shipName(L""), 
    shipTeam(L""),
    shipPosition(std::make_pair(0.0, 0.0)),
    shipSpeed(0), 
    numberOfMissiles(0), 
    HP(100.0) {}
};

#endif // SHIP_H