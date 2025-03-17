#ifndef ADMINFEDERATEAMBASSADOR_H
#define ADMINFEDERATEAMBASSADOR_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <iostream>

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
};

#endif