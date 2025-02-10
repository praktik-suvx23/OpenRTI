#include "../include/scenarioFederate.h"
#include "../include/scenarioFedAmb.h"
#include <iostream>
#include <cstring>

scenarioFedAmb::scenarioFedAmb(rti1516e::RTIambassador* rtiAmbassador) : _rtiAmbassador(rtiAmbassador) {}

scenarioFedAmb::~scenarioFedAmb() {}

void scenarioFedAmb::announceSynchronizationPoint (
     std::wstring  const & label,
     rti1516e::VariableLengthData const & theUserSuppliedTag)
{
    if (label == L"InitialSync") {
        std::wcout << L"Master Federate synchronized at InitialSync." << std::endl;
        syncLabel = label;
    }

    if (label == L"ShutdownSync") {
        std::wcout << L"Master Federate synchronized at ShutdownSync." << std::endl;
        syncLabel = label;
    }
}
