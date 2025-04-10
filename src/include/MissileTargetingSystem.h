#ifndef MISSILETARGETINGSYSTEM_H
#define MISSILETARGETINGSYSTEM_H
#include <string>

enum class MissileStatus {
    INACTIVE,
    ACTIVE,
    LOOKING_FOR_TARGET,
    TARGET_LOCKED,
    TARGET_LOST,
    TARGET_HIT,
    DESTROYED
};

struct MissileTargetingSystem {
    std::wstring missileID;
    std::wstring missileTeam;
    std::wstring targetID;
    u_int8_t targetSize;
    MissileStatus status;

    MissileTargetingSystem() :
    missileID(L""),
    missileTeam(L""),
    targetID(L""),
    targetSize(0),
    status(MissileStatus::INACTIVE) {}

    MissileTargetingSystem(const std::wstring& missileID, const std::wstring& missileTeam, const std::wstring& targetID, u_int8_t targetSize, MissileStatus status = MissileStatus::INACTIVE) :
    missileID(missileID),
    missileTeam(missileTeam),
    targetID(targetID),
    targetSize(targetSize),
    status(status) {} 
};

#endif // MISSILETARGETINGSYSTEM_H