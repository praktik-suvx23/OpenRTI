#include "../../MissileManagerFederate/structMissile.h"
#include <cstring>

void serializeMissile(const Missile& missile, char* buffer) {
    std::memset(buffer, 0, 50 * sizeof(wchar_t));
    std::wcsncpy(reinterpret_cast<wchar_t*>(buffer), missile.structMissileID.c_str(), 49);
    buffer += 50 * sizeof(wchar_t);

    std::memset(buffer, 0, 50 * sizeof(wchar_t));
    std::wcsncpy(reinterpret_cast<wchar_t*>(buffer), missile.structMissileTeam.c_str(), 49);
    buffer += 50 * sizeof(wchar_t);

    std::memcpy(buffer, &missile.structMissilePosition.first, sizeof(double));
    buffer += sizeof(double);
    std::memcpy(buffer, &missile.structMissilePosition.second, sizeof(double));
    buffer += sizeof(double);

    std::memcpy(buffer, &missile.structMissileSpeed, sizeof(double));
    buffer += sizeof(double);

    std::memcpy(buffer, &missile.structMissileAltitude, sizeof(double));
    buffer += sizeof(double);

    std::memcpy(buffer, &missile.structMissileHeightAchieved, sizeof(bool));
}

Missile deserializeMissile(const char* buffer) {
    Missile missile = Missile(rti1516e::ObjectInstanceHandle());

    wchar_t idBuffer[50];
    std::wcsncpy(idBuffer, reinterpret_cast<const wchar_t*>(buffer), 49);
    missile.structMissileID = idBuffer;
    buffer += 50 * sizeof(wchar_t);

    wchar_t teamBuffer[50];
    std::wcsncpy(teamBuffer, reinterpret_cast<const wchar_t*>(buffer), 49);
    missile.structMissileTeam = teamBuffer;
    buffer += 50 * sizeof(wchar_t);

    std::memcpy(&missile.structMissilePosition.first, buffer, sizeof(double));
    buffer += sizeof(double);
    std::memcpy(&missile.structMissilePosition.second, buffer, sizeof(double));
    buffer += sizeof(double);

    std::memcpy(&missile.structMissileSpeed, buffer, sizeof(double));
    buffer += sizeof(double);

    std::memcpy(&missile.structMissileAltitude, buffer, sizeof(double));
    buffer += sizeof(double);

    std::memcpy(&missile.structMissileHeightAchieved, buffer, sizeof(bool));

    return missile;
}