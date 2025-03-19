#ifndef DECODEPOSITION_H
#define DECODEPOSITION_H

#include <utility>
#include <cstring>

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

#endif