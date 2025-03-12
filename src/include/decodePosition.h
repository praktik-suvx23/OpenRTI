#ifndef DECODEPOSITION_H
#define DECODEPOSITION_H

#include <utility>
#include <RTI/encoding/HLAfixedRecord.h>

/*
    Helper function to decode PisotionRec from the FOM.xml data type
*/

std::pair<double, double> decodePositionRec(const rti1516e::VariableLengthData& encodedData) {
    rti1516e::HLAfixedRecord positionRecord;
    rti1516e::HLAfloat64BE lat, lon;

    // Append elements in the order defined in the FOM
    positionRecord.appendElement(lat);
    positionRecord.appendElement(lon);

    // Decode the provided data
    positionRecord.decode(encodedData);

    return {lat.get(), lon.get()};
}

#endif