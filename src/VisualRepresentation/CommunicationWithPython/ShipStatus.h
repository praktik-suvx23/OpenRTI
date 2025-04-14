#ifndef SHIPSTATUS_H
#define SHIPSTATUS_H

#include <string>

/* Might not be needed. */

struct ShipStatus {
    std::wstring shipID;
    std::wstring team;
    std::pair<double, double> position;
    double speed;
    double size;
    double hp;

    ShipStatus() :
    shipID(L""),
    team(L""),
    position(std::make_pair(0.0, 0.0)),
    speed(0.0),
    size(0.0),
    hp(100.0) {}
};

#endif // SHIPSTATUS_H

