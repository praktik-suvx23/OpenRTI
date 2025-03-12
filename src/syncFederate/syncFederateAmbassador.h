#ifndef SYNCFEDERATEAMBASSADOR_H
#define SYNCFEDERATEAMBASSADOR_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <iostream>

class syncFederateAmbassador : public rti1516e::NullFederateAmbassador {
public:
    syncFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador);
    ~syncFederateAmbassador();

    void announceSynchronizationPoint (
         std::wstring  const & label,
         rti1516e::VariableLengthData const & theUserSuppliedTag) override;

    std::wstring syncLabel = L"";

private:
    rti1516e::RTIambassador* _rtiAmbassador;
};

#endif