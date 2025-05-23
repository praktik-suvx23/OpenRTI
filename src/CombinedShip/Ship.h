#ifndef SHIP_H
#define SHIP_H

#include <array>
#include <string>
#include <RTI/RTI1516.h>

enum ShipTeam {
  UNSIGNED = 0,
  BLUE = 1,
  RED = 2
};

enum orderStatus {
  ORDER_UNASSIGNED = 0,
  ORDER_INITIATED = 1,
  ORDER_IN_PROGRESS = 2,
  ORDER_CANCELLED = 3,
  ORDER_CONFIRMED = 4,
  ORDER_COMPLETED = 5
};

struct Ship {
    rti1516e::ObjectInstanceHandle objectInstanceHandle;
    std::wstring shipName;
    std::wstring shipTeam;
    std::pair<double, double> shipPosition;
    double shipSpeed;
    double bearing;
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
      bearing(0),
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
      bearing(0),
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
  orderStatus status;

  FireOrder(Ship* shooter, Ship* target, int amount, orderStatus stat = ORDER_UNASSIGNED)
      : shooterShip(shooter), targetShip(target), missileAmount(amount), 
        status(stat) {}
};

#endif // SHIP_H