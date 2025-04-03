#include "../CombinedShip/Ship.h"
#include "../MissileManagerFederate/structMissile.h"
#include <cstring>
#include <codecvt>
#include <locale>

void serializeMissile(const Missile& missile, char* buffer) {
    memset(buffer, 0, 232);  // Ensure buffer is clean

    // Convert wstring to UTF-16 (little-endian)
    std::wstring_convert<std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>> converter;
    std::string utf16_id = converter.to_bytes(missile.structMissileID);
    std::string utf16_team = converter.to_bytes(missile.structMissileTeam);

    // Copy into buffer (make sure we don't exceed 100 bytes)
    memcpy(buffer, utf16_id.c_str(), std::min(utf16_id.size(), (size_t)100));
    memcpy(buffer + 100, utf16_team.c_str(), std::min(utf16_team.size(), (size_t)100));

    // Copy position, speed, and altitude as little-endian doubles
    double* data_ptr = reinterpret_cast<double*>(buffer + 200);
    data_ptr[0] = missile.structMissilePosition.first;
    data_ptr[1] = missile.structMissilePosition.second;
    data_ptr[2] = missile.structMissileSpeed;
    data_ptr[3] = missile.structMissileAltitude;
}

void serializeShip(const Ship& ship, char* buffer) {
    // Clear the buffer
    std::memset(buffer, 0, 256); // Adjust size as needed

    // Serialize shipName (UTF-16 Little Endian, max 100 bytes)
    std::wstring shipName = ship.shipName.substr(0, 50); // Limit to 50 characters
    std::memcpy(buffer, shipName.c_str(), shipName.size() * sizeof(wchar_t));

    // Serialize shipTeam (UTF-16 Little Endian, max 100 bytes)
    std::wstring shipTeam = ship.shipTeam.substr(0, 50); // Limit to 50 characters
    std::memcpy(buffer + 100, shipTeam.c_str(), shipTeam.size() * sizeof(wchar_t));

    // Serialize shipPosition (16 bytes: 2 doubles)
    std::memcpy(buffer + 200, &ship.shipPosition.first, sizeof(double));
    std::memcpy(buffer + 208, &ship.shipPosition.second, sizeof(double));

    // Serialize shipSpeed (8 bytes: 1 double)
    std::memcpy(buffer + 216, &ship.shipSpeed, sizeof(double));

    // Serialize shipSize (8 bytes: 1 double)
    std::memcpy(buffer + 224, &ship.shipSize, sizeof(double));

    // Serialize shipHP (8 bytes: 1 double)
    std::memcpy(buffer + 232, &ship.shipHP, sizeof(double));
}

Missile deserializeMissile(const char* buffer) {
    Missile missile = Missile(rti1516e::ObjectInstanceHandle());

    // Deserialize structMissileID
    wchar_t idBuffer[50];
    std::memcpy(idBuffer, buffer, 50 * sizeof(wchar_t));
    missile.structMissileID = idBuffer;
    buffer += 50 * sizeof(wchar_t);

    // Deserialize structMissileTeam
    wchar_t teamBuffer[50];
    std::memcpy(teamBuffer, buffer, 50 * sizeof(wchar_t));
    missile.structMissileTeam = teamBuffer;
    buffer += 50 * sizeof(wchar_t);

    // Deserialize structMissilePosition
    std::memcpy(&missile.structMissilePosition.first, buffer, sizeof(double));
    buffer += sizeof(double);
    std::memcpy(&missile.structMissilePosition.second, buffer, sizeof(double));
    buffer += sizeof(double);

    // Deserialize structMissileSpeed
    std::memcpy(&missile.structMissileSpeed, buffer, sizeof(double));
    buffer += sizeof(double);

    // Deserialize structMissileAltitude
    std::memcpy(&missile.structMissileAltitude, buffer, sizeof(double));
    buffer += sizeof(double);

    return missile;
}