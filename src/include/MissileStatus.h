#ifndef MISSILESTATUS_H
#define MISSILESTATUS_H

#include <RTI/RTI1516.h>
#include <string>

enum class MissileStatus {
    INACTIVE,
    ACTIVE,
    HIGHT_ACHIEVED,
    LOOKING_FOR_TARGET,
    TARGET_LOCKED,
    TARGET_LOST,
    TARGET_HIT,
    DESTROYED
};

// Struct for communication between ship and missile. WIP
struct MissileCommunication {
    rti1516e::ObjectInstanceHandle shipID{};
    rti1516e::ObjectInstanceHandle targetID{};
    rti1516e::ObjectInstanceHandle missileID{};
    size_t numberOfMissilesFired{};
    MissileStatus status = MissileStatus::INACTIVE;
    MissileStatus lastStatus = MissileStatus::INACTIVE;

    MissileCommunication(
        rti1516e::ObjectInstanceHandle shipID,
        rti1516e::ObjectInstanceHandle targetID,
        rti1516e::ObjectInstanceHandle missileID,
        size_t numberOfMissilesFired = 0,
        MissileStatus status = MissileStatus::INACTIVE,
        MissileStatus lastStatus = MissileStatus::INACTIVE)
        : shipID(shipID), targetID(targetID), missileID(missileID), 
        numberOfMissilesFired(numberOfMissilesFired), status(status), lastStatus(status) {}
};

#endif