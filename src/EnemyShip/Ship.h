#ifndef SHIP_H
#define SHIP_H

#include <string>
#include <RTI/RTI1516.h>

struct Ship {
    rti1516e::ObjectInstanceHandle objectInstanceHandle;
    std::wstring shipName;
    std::wstring shipPosition;
    double shipSpeed;
    int numberOfMissiles;
    double HP;

    Ship(rti1516e::ObjectInstanceHandle objectInstanceHandle) 
    : objectInstanceHandle(objectInstanceHandle), 
    shipName(L""), 
    shipPosition(L""), 
    shipSpeed(0), 
    numberOfMissiles(0), 
    HP(100.0) {}
};

struct EnemyShip {
    rti1516e::ObjectInstanceHandle objectInstanceHandle;
    std::wstring shipName;
    std::wstring shipPosition;
    double shipSpeed;
    int numberOfMissiles;
    double HP;

    EnemyShip(rti1516e::ObjectInstanceHandle objectInstanceHandle) 
    : objectInstanceHandle(objectInstanceHandle), 
    shipName(L""), 
    shipPosition(L""), 
    shipSpeed(0), 
    numberOfMissiles(0), 
    HP(100.0) {}
};

#endif // SHIP_H