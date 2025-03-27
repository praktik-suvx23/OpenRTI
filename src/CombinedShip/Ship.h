#ifndef SHIP_H
#define SHIP_H

#include <string>
#include <RTI/RTI1516.h>

struct Ship {
    rti1516e::ObjectInstanceHandle objectInstanceHandle;
    std::wstring shipName;
    std::pair<double, double> shipPosition;
    double shipSpeed;
    double shipSize;
    int numberOfMissiles;
    int numberOfCanons;
    double HP;

    Ship(rti1516e::ObjectInstanceHandle objectInstanceHandle) 
    : objectInstanceHandle(objectInstanceHandle), 
    shipName(L""), 
    shipPosition(std::make_pair(0.0, 0.0)), 
    shipSpeed(0), 
    shipSize(0),
    numberOfMissiles(0),
    numberOfCanons(0), 
    HP(100.0) {}
};

struct EnemyShip {
    rti1516e::ObjectInstanceHandle objectInstanceHandle;
    std::wstring shipName;
    std::pair<double, double> shipPosition;
    double shipSpeed;
    double shipSize;
    int numberOfMissiles;
    int numberOfCanons;
    double HP;

    EnemyShip(rti1516e::ObjectInstanceHandle objectInstanceHandle) 
    : objectInstanceHandle(objectInstanceHandle), 
    shipName(L""), 
    shipPosition(std::make_pair(0.0, 0.0)),
    shipSpeed(0), 
    shipSize(0),
    numberOfMissiles(0), 
    numberOfCanons(0),
    HP(100.0) {}
};

#endif // SHIP_H