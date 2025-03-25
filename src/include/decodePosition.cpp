#include "decodePosition.h"

std::pair<double, double> decodePositionRec(const rti1516e::VariableLengthData& encodedData) {
    // Function implementation (same as before)
    rti1516e::HLAfloat64BE xElement;
    rti1516e::HLAfloat64BE yElement;

    rti1516e::HLAfixedRecord decodedRecord;
    decodedRecord.appendElement(xElement);
    decodedRecord.appendElement(yElement);

    decodedRecord.decode(encodedData);

    double x = static_cast<const rti1516e::HLAfloat64BE&>(decodedRecord.get(0)).get();
    double y = static_cast<const rti1516e::HLAfloat64BE&>(decodedRecord.get(1)).get();

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
    encodedRecord.appendElement(rti1516e::HLAfloat64BE(position.first));
    encodedRecord.appendElement(rti1516e::HLAfloat64BE(position.second));
    return encodedRecord;
}