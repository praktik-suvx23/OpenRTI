#ifndef FEDERATEAMBASSADOR_H
#define FEDERATEAMBASSADOR_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAinteger64TimeFactory.h>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <string>
#include <sstream>

class FederateAmbassador : public rti1516e::NullFederateAmbassador {
public:
    FederateAmbassador();
    ~FederateAmbassador();

    void receiveInteraction(rti1516e::InteractionClassHandle theInteraction,
                            rti1516e::ParameterHandleValueMap const &theParameters,
                            rti1516e::VariableLengthData const &theUserSuppliedTag,
                            rti1516e::OrderType sentOrder,
                            rti1516e::TransportationType theType,
                            rti1516e::LogicalTime const &theTime,
                            rti1516e::OrderType receivedOrder,
                            rti1516e::MessageRetractionHandle theHandle,
                            rti1516e::SupplementalReceiveInfo theReceiveInfo) override;

    void reflectAttributeValues(rti1516e::ObjectInstanceHandle objectInstanceHandle,
                                const rti1516e::AttributeHandleValueMap& attributeValues,
                                const rti1516e::VariableLengthData& tag,
                                rti1516e::OrderType sentOrder,
                                rti1516e::TransportationType transportationType,
                                const rti1516e::LogicalTime& time,
                                rti1516e::OrderType receivedOrder,
                                rti1516e::MessageRetractionHandle retractionHandle,
                                rti1516e::SupplementalReflectInfo reflectInfo) override;

    void receiveAttributeValue(const rti1516e::ObjectInstanceHandle& objectInstanceHandle,
                               const rti1516e::AttributeHandleSet& attributes,
                               const rti1516e::LogicalTime& time);

    void setVehiclePosition(double position);
    double getVehiclePosition() const;

    void setVehicleSpeed(double speed);
    double getVehicleSpeed() const;

    bool isUpdateTimeout() const;

    void waitForUpdate();

private:
    int _callbackState;
    double _vehiclePosition;
    double _vehicleSpeed;

    std::chrono::steady_clock::time_point lastUpdateTime;

    double _lookahead; // Maybe use later. Tyingstuff uses it for time regulation

    mutable std::mutex mutex;
    std::condition_variable cv;
    bool valuesUpdated = false;
    bool receivedNewData = false;
    rti1516e::AttributeHandle positionHandle;
    rti1516e::AttributeHandle speedHandle;
    double currentPositionValue;
    double currentSpeedValue;

    void notifyAttributeUpdate();
};

#endif