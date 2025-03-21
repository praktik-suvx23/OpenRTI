#ifndef DECODEPOSITION_H
#define DECODEPOSITION_H

#include <utility>
#include <cstring>
#include <iostream>
#include <RTI/encoding/HLAfixedRecord.h>
#include <RTI/encoding/BasicDataElements.h>

// Function declarations
std::pair<double, double> decodePositionRec(const rti1516e::VariableLengthData& encodedData);
rti1516e::HLAfixedRecord encodePositionRec(const std::pair<double, double>& position);

#endif