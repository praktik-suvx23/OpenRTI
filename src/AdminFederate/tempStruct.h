#ifndef TEMPSTRUCT_H
#define TEMPSTRUCT_H

#include <string>

enum class Team {
    UNSIGNED,
    Blue,
    Red
};

struct ConfirmHandshake {
    std::wstring shooterID;
    int32_t missilesLoaded;
    bool shooterReceivedOrder;
    std::wstring targetID;
    int32_t missilesLocked;
};

struct InitialHandshake {
    std::wstring shooterID;
    int32_t missilesLoaded;
    std::wstring targetID;
    int32_t maxMissilesRequired;
    int32_t missilesCurrentlyTargeting;
    int32_t distanceToTarget;
};

#endif // TEMPSTRUCT_H