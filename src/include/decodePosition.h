#ifndef DECODEPOSITION_H
#define DECODEPOSITION_H

#include <utility>
#include <cstring>
#include <RTI/LogicalTimeFactory.h>
#include <RTI/LogicalTime.h>
#include <RTI/VariableLengthData.h>
#include <RTI/Handle.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/encoding/EncodingExceptions.h>
#include <RTI/encoding/DataElement.h>
#include <RTI/encoding/HLAfixedRecord.h>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <cmath>

/*
    Helper function to decode PositionRec from the FOM.xml data type
*/

std::pair<double, double> decodePositionRec(const rti1516e::VariableLengthData& encodedData) {
    // Create HLAfixedRecord and add actual HLAfloat64BE elements
    rti1516e::HLAfloat64BE xElement;
    rti1516e::HLAfloat64BE yElement;

    rti1516e::HLAfixedRecord decodedRecord;
    decodedRecord.appendElement(xElement);
    decodedRecord.appendElement(yElement);

    // Decode the data
    decodedRecord.decode(encodedData);

    // Retrieve values
    double x = static_cast<const rti1516e::HLAfloat64BE&>(decodedRecord.get(0)).get();
    double y = static_cast<const rti1516e::HLAfloat64BE&>(decodedRecord.get(1)).get();

    // Validate latitude and longitude
    if (x < -90.0 || x > 90.0) {
        std::wcerr << L"[ERROR] Invalid latitude value: " << x << std::endl;
        return {0.0, 0.0};
    }

    if (y < -180.0 || y > 180.0) {
        std::wcerr << L"[ERROR] Invalid longitude value: " << y << std::endl;
        return {0.0, 0.0};
    }

    return {x, y};
}

rti1516e::HLAfixedRecord encodePositionRec(const std::pair<double, double>& position) {
    rti1516e::HLAfixedRecord encodedRecord;
    encodedRecord.appendElement(rti1516e::HLAfloat64BE(position.first));  // Latitude or X
    encodedRecord.appendElement(rti1516e::HLAfloat64BE(position.second)); // Longitude or Y
    return encodedRecord;
}

#endif