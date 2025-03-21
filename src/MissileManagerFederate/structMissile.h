#ifndef STRUCTMISSILE_H
#define STRUCTMISSILE_H

#include <RTI/RTI1516.h>
#include <string>

struct Missile {
    rti1516e::ObjectInstanceHandle objectInstanceHandle;
    std::wstring missileID;
    std::wstring missileTeam;
    std::pair<double, double> missilePosition;
    double missileAltitude;
    double missileSpeed;
    std::wstring lockOnTargetID;
    bool hitTarget;
    bool destroyed;

    Missile(rti1516e::ObjectInstanceHandle objectInstanceHandle)
        : objectInstanceHandle(objectInstanceHandle),
        missileID(L""),
        missileTeam(L""),
        missilePosition(std::make_pair(0.0, 0.0)),
        missileAltitude(0.0),
        missileSpeed(0.0),
        lockOnTargetID(L""),
        hitTarget(false),
        destroyed(false) {}
};

#endif