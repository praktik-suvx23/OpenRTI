#ifndef TEMPSTRUCT_H
#define TEMPSTRUCT_H

#include <string>

struct ShooterInfo {
    std::wstring shipID;
    std::wstring team;
    int missilesAvailable;
};

struct TargetInfo {
    std::wstring shipID;
    std::wstring team;
    int maxMissilesRequired;
    int missilesCurrentlyTargeting;
};

#endif // TEMPSTRUCT_H