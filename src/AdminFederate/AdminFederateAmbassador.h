#ifndef ADMINFEDERATEAMBASSADOR_H
#define ADMINFEDERATEAMBASSADOR_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/encoding/EncodingExceptions.h>
#include <RTI/encoding/DataElement.h>

#include <RTI/LogicalTimeFactory.h>
#include <RTI/LogicalTimeInterval.h>
#include <RTI/LogicalTime.h>

#include <RTI/time/HLAfloat64Interval.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAfloat64TimeFactory.h>
#include <iostream>
#include <thread>

class AmbassadorGetter;
class AmbassadorSetter;

class AdminFederateAmbassador : public rti1516e::NullFederateAmbassador {
    friend class AmbassadorGetter;
    friend class AmbassadorSetter;

    rti1516e::RTIambassador* _rtiAmbassador;

    std::wstring syncLabel = L"";

    rti1516e::InteractionClassHandle setupSimulationHandle; // Interaction class handle for setupSimulation interaction
    rti1516e::ParameterHandle blueShipsParam;               // Parameter handle for blueShips parameter
    rti1516e::ParameterHandle redShipsParam;                // Parameter handle for redShips parameter
    rti1516e::ParameterHandle timeScaleFactorParam;         // Parameter handle for timeScaleFactor parameter
public:
    AdminFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador);
    ~AdminFederateAmbassador();

    void announceSynchronizationPoint (
         std::wstring  const & label,
         rti1516e::VariableLengthData const & theUserSuppliedTag) override;

    bool isRegulating = false;
    bool isConstrained = false;
    bool isAdvancing = false;

    void timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeAdvanceGrant(const rti1516e::LogicalTime& theTime) override;
};

#endif