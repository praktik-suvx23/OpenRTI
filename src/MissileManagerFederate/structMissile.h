#ifndef STRUCTMISSILE_H
#define STRUCTMISSILE_H

#include <RTI/RTI1516.h>
#include <string>

struct Missile {
    rti1516e::ObjectInstanceHandle objectInstanceHandle;
    std::wstring structMissileID;
    std::wstring structMissileTeam;
    std::pair<double, double> structMissilePosition;
    double structMissileAltitude;
    double structMissileSpeed;

    Missile(rti1516e::ObjectInstanceHandle objectInstanceHandle)
        : objectInstanceHandle(objectInstanceHandle),
        structMissileID(L""),
        structMissileTeam(L""),
        structMissilePosition(std::make_pair(0.0, 0.0)),
        structMissileAltitude(0.0),
        structMissileSpeed(0.0) {}
};

#endif