#ifndef STRUCTMISSILE_H
#define STRUCTMISSILE_H

#include <RTI/RTI1516.h>
#include <string>

// This should be based on the FOM: MissileFlight ..?

struct Missile {
    rti1516e::ObjectInstanceHandle objectInstanceHandle;
    std::wstring structMissileID;
    std::wstring structMissileTeam;
    std::pair<double, double> structMissilePosition;
    std::pair<double, double> structInitialTargetPosition;
    double structMissileAltitude;
    double structMissileSpeed;
    std::chrono::time_point<std::chrono::high_resolution_clock> structMissileStartTime;
    double structInitialBearing;
    double structMissileDistanceToTarget;

    Missile(rti1516e::ObjectInstanceHandle objectInstanceHandle)
        : objectInstanceHandle(objectInstanceHandle),
        structMissileID(L""),
        structMissileTeam(L""),
        structMissilePosition(std::make_pair(0.0, 0.0)),
        structInitialTargetPosition(std::make_pair(0.0, 0.0)),
        structMissileAltitude(0.0),
        structMissileSpeed(0.0),
        structMissileStartTime(std::chrono::high_resolution_clock::now()),
        structInitialBearing(0.0),
        structMissileDistanceToTarget(0.0) {}
};

struct Ship {
    rti1516e::ObjectInstanceHandle objectInstanceHandle;
    std::wstring structShipFederateName;
    std::wstring structShipTeam;
    std::pair<double, double> structShipPosition;
    double structShipSpeed;

    Ship(rti1516e::ObjectInstanceHandle objectInstanceHandle)
        : objectInstanceHandle(objectInstanceHandle),
        structShipFederateName(L""),
        structShipTeam(L""),
        structShipPosition(std::make_pair(0.0, 0.0)),
        structShipSpeed(0.0) {}
};
#endif