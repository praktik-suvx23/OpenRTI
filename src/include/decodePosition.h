#ifndef DECODEPOSITION_H
#define DECODEPOSITION_H

#include <utility>
#include <cstring>
#include <RTI/VariableLengthData.h>

/*
    Helper function to decode PositionRec from the FOM.xml data type
*/

std::pair<double, double> decodePositionRec(const rti1516e::VariableLengthData& encodedData) {
    if (encodedData.size() != 16) {  // Each HLAfloat64BE is 8 bytes, so total should be 16 bytes
        std::wcerr << L"[ERROR] Invalid encoded PositionRec size: " << encodedData.size() << std::endl;
        return {0.0, 0.0};  // Return default if size is incorrect
    }

    double lat = 0.0, lon = 0.0;
    const char* dataPtr = static_cast<const char*>(encodedData.data()); // Cast to const char*

    // Extract bytes directly
    std::memcpy(&lat, dataPtr, 8);
    std::memcpy(&lon, dataPtr + 8, 8);

    // Convert from big-endian to host byte order
    rti1516e::HLAfloat64BE latBE, lonBE;
    latBE.set(lat);
    lonBE.set(lon);

    return {latBE.get(), lonBE.get()};  
}

rti1516e::VariableLengthData encodePositionRec(const std::pair<double, double>& position) {
    rti1516e::VariableLengthData encodedData;

    // Extract latitude and longitude from the pair
    double lat = position.first;
    double lon = position.second;

    // Create a buffer to hold both latitude and longitude (16 bytes total)
    char buffer[16];
    std::memcpy(buffer, &lat, sizeof(double));        // Copy latitude into the buffer
    std::memcpy(buffer + sizeof(double), &lon, sizeof(double)); // Copy longitude into the buffer

    // Set the buffer as the data for VariableLengthData
    encodedData.setData(buffer, sizeof(buffer));

    return encodedData;
}

#endif