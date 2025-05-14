#include "../CombinedShip/Ship.h"
#include "../MissileFederate/structMissile.h"
#include <cstring>
#include <codecvt>
#include <locale>

void serializeMissile(const Missile& missile, char* buffer) {
    memset(buffer, 0, 232);  // Ensure buffer is clean

    // Convert wstring to UTF-16 (little-endian)
    std::wstring_convert<std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>> converter;
    std::string utf16_id = converter.to_bytes(missile.id);
    std::string utf16_team = converter.to_bytes(missile.team);

    // Copy into buffer (make sure we don't exceed 100 bytes)
    memcpy(buffer, utf16_id.c_str(), std::min(utf16_id.size(), (size_t)100));
    memcpy(buffer + 100, utf16_team.c_str(), std::min(utf16_team.size(), (size_t)100));

    // Copy position, speed, and altitude as little-endian doubles
    double* data_ptr = reinterpret_cast<double*>(buffer + 200);
    data_ptr[0] = missile.position.first;
    data_ptr[1] = missile.position.second;
    data_ptr[2] = missile.speed;
    data_ptr[3] = missile.altitude;
}

void serializeShip(const Ship& ship, char* buffer) {
    memset(buffer, 0, 256);  // Ensure buffer is clean

    // Convert wstring to UTF-16 (little-endian)
    std::wstring_convert<std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>> converter;
    std::string utf16_name = converter.to_bytes(ship.shipName);
    std::string utf16_team = converter.to_bytes(ship.shipTeam);
   
    // Copy into buffer (make sure we don't exceed 100 bytes for each field)
    memcpy(buffer, utf16_name.c_str(), std::min(utf16_name.size(), (size_t)100));
    memcpy(buffer + 100, utf16_team.c_str(), std::min(utf16_team.size(), (size_t)100));

    // Copy position, speed, size, and HP as little-endian doubles
    double* data_ptr = reinterpret_cast<double*>(buffer + 200);
    data_ptr[0] = ship.shipPosition.first;  // X-coordinate of position
    data_ptr[1] = ship.shipPosition.second; // Y-coordinate of position
    data_ptr[2] = ship.shipSpeed;           // Ship speed
    data_ptr[3] = ship.shipSize;            // Ship size
    data_ptr[4] = ship.shipHP;              // Ship HP
}