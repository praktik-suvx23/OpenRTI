#include "../include/pubFedAmb.h"
#include "../include/pubFederate.h"

pubFedAmb::pubFedAmb(rti1516e::RTIambassador* rtiAmbassador) : _rtiAmbassador(rtiAmbassador) {}

pubFedAmb::~pubFedAmb() {}

void pubFedAmb::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag)
{
    if (label == L"InitialSync") {
        std::wcout << L"Publisher Federate received synchronization announcement: InitialSync." << std::endl;
        syncLabel = label;
    }

    if (label == L"ShutdownSync") {
        std::wcout << L"Publisher Federate received synchronization announcement: ShutdownSync." << std::endl;
        syncLabel = label; 
    }
}