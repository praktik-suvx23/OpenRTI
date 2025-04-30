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
    size_t maxMissilesLocking;
    size_t currentMissilesLocking;

    // Default constructor
    Ship() 
    : objectInstanceHandle(), 
      shipName(L""), 
      shipTeam(L""),
      shipPosition(std::make_pair(0.0, 0.0)), 
      shipSpeed(0), 
      shipSize(0),
      shipNumberOfMissiles(0),
      shipNumberOfCanons(0), 
      shipHP(100.0),
      maxMissilesLocking(2), 
      currentMissilesLocking(0) {}

    // Constructor with ObjectInstanceHandle
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
      maxMissilesLocking(2), 
      currentMissilesLocking(0) {}
};

struct FireOrder {
  Ship* shooterShip;
  Ship* targetShip;
  int missileAmount;
  int orderID;

  FireOrder(Ship* shooter, Ship* target, int amount, int id)
      : shooterShip(shooter), targetShip(target), missileAmount(amount), orderID(id) {}
};

#endif // SHIP_H