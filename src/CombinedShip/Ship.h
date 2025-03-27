#ifndef SHIP_H
#define SHIP_H

#include <array>
#include <string>
#include <RTI/RTI1516.h>

struct BlueShip {
    rti1516e::ObjectInstanceHandle objectInstanceHandle;
    std::wstring blueShipName;
    std::wstring blueShipTeam;
    std::pair<double, double> blueShipPosition;
    double blueShipSpeed;
    double blueShipSize;
    int blueShipNumberOfMissiles;
    int blueShipNumberOfCanons;
    double blueShipHP;
    std::array<double, 3> closestRedShipDistance;
    std::array<int, 3> closestRedShipIndex;
    std::array<int, 3> missilesFiredAtRedShip;

    BlueShip(rti1516e::ObjectInstanceHandle objectInstanceHandle) 
    : objectInstanceHandle(objectInstanceHandle), 
    blueShipName(L""), 
    blueShipTeam(L""),
    blueShipPosition(std::make_pair(0.0, 0.0)), 
    blueShipSpeed(0), 
    blueShipSize(0),
    blueShipNumberOfMissiles(0),
    blueShipNumberOfCanons(0), 
    blueShipHP(100.0),
    closestRedShipDistance{-1, -1, -1},
    closestRedShipIndex{-1, -1, -1},
    missilesFiredAtRedShip{-1, -1, -1} {}
};

struct RedShip {
    rti1516e::ObjectInstanceHandle objectInstanceHandle;
    std::wstring redShipName;
    std::wstring redShipTeam;
    std::pair<double, double> redShipPosition;
    double redShipSpeed;
    double redShipSize;
    int redShipNumberOfMissiles;
    int redShipNumberOfCanons;
    double redShipHP;
    std::array<double, 3> closestBlueShipDistance;
    std::array<int, 3> closestBlueShipIndex;
    std::array<int, 3> missilesFiredAtBlueShip;

    RedShip(rti1516e::ObjectInstanceHandle objectInstanceHandle) 
    : objectInstanceHandle(objectInstanceHandle), 
    redShipName(L""), 
    redShipTeam(L""),
    redShipPosition(std::make_pair(0.0, 0.0)),
    redShipSpeed(0), 
    redShipSize(0),
    redShipNumberOfMissiles(0), 
    redShipNumberOfCanons(0),
    redShipHP(100.0),
    closestBlueShipDistance{-1, -1, -1},
    closestBlueShipIndex{-1, -1, -1},
    missilesFiredAtBlueShip{-1, -1, -1} {}
};

#endif // SHIP_H