#pragma once
#include <RTI/VariableLengthData.h>
#include <stdexcept>

class MyFloat32Decoder {
public:
    void decode(const rti1516e::VariableLengthData& data, float& value) {
        if (data.size() == sizeof(float)) {
            value = *reinterpret_cast<const float*>(data.data());
        } else {
            throw std::runtime_error("Invalid data size for float32 attribute");
        }
    }
};