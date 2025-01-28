#ifndef MY_FEDERATE_AMBASSADOR_H
#define MY_FEDERATE_AMBASSADOR_H
#include <memory>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/RTIambassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include <iostream>

class MyFederateAmbassador : public rti1516e::NullFederateAmbassador {
public:
    MyFederateAmbassador()
        : currentPositionValue(0.0), currentSpeedValue(0.0), valuesUpdated(false) {}

    void reflectAttributeValues(rti1516e::ObjectInstanceHandle objectInstanceHandle,
                                const rti1516e::AttributeHandleValueMap& attributeValues,
                                const rti1516e::VariableLengthData& tag,
                                rti1516e::OrderType sentOrder,
                                rti1516e::TransportationType transportationType,
                                const rti1516e::LogicalTime& time,
                                rti1516e::OrderType receivedOrder,
                                rti1516e::MessageRetractionHandle retractionHandle,
                                rti1516e::SupplementalReflectInfo reflectInfo) override {
        std::unique_lock<std::mutex> lock(mutex);
        for (const auto& attribute : attributeValues) {
            if (attribute.first == positionHandle) {
                std::memcpy(&currentPositionValue, attribute.second.data(), sizeof(currentPositionValue));
                std::cout << "Received Position Value: " << currentPositionValue << std::endl;
            } else if (attribute.first == speedHandle) {
                std::memcpy(&currentSpeedValue, attribute.second.data(), sizeof(currentSpeedValue));
                std::cout << "Received Speed Value: " << currentSpeedValue << std::endl;
            }
        }
        valuesUpdated = true;
        cv.notify_all();
    }

    double currentPositionValue;
    double currentSpeedValue;
    rti1516e::AttributeHandle positionHandle;
    rti1516e::AttributeHandle speedHandle;
    std::mutex mutex;
    std::condition_variable cv;
    bool valuesUpdated;
};

#endif // MY_FEDERATE_AMBASSADOR_H