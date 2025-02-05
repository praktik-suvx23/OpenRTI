#pragma once
#include "RTI/VariableLengthData.h"
#include <stdexcept>

class MyPositionDecoder {
public:
    void decode(const rti1516e::VariableLengthData& data, float& position) {
        if (data.size() == sizeof(float)) {
            position = *reinterpret_cast<const float*>(data.data());
        } else {
            throw std::runtime_error("Invalid data size for position attribute");
        }
    }
};