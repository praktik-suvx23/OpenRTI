#ifndef SHIP_H
#define SHIP_H

#include <array>
#include <string>
#include <RTI/RTI1516.h>

struct Ship {
    rti1516e::ObjectInstanceHandle objectInstanceHandle;
    std::wstring shipName;
    std::wstring shipTeam;
    std::pair<double, double> shipPosition;
    double shipSpeed;
    double shipSize;
    int shipNumberOfMissiles;
    int shipNumberOfCanons;
    double shipHP;
    std::array<double, 3> closestShipDistance;
    std::array<int, 3> closestShipIndex;
    std::array<int, 3> missilesFiredAtShip;

    Ship(rti1516e::ObjectInstanceHandle objectInstanceHandle) 
    : objectInstanceHandle(objectInstanceHandle), 
    shipName(L""), 
    shipTeam(L""),
    shipPosition(std::make_pair(0.0, 0.0)), 
    shipSpeed(0), 
    shipSize(0),
    shipNumberOfMissiles(0),
    shipNumberOfCanons(0), 
    shipHP(100.0),
    closestShipDistance{-1, -1, -1},
    closestShipIndex{-1, -1, -1},
    missilesFiredAtShip{-1, -1, -1} {}
};

#endif // SHIP_H