#include "../include/mastFederate.h"
#include "../include/mastFedAmb.h"
#include <iostream>
#include <cstring>

mastFedAmb::mastFedAmb(rti1516e::RTIambassador* rtiAmbassador) : _rtiAmbassador(rtiAmbassador) {}

mastFedAmb::~mastFedAmb() {}

void mastFedAmb::announceSynchronizationPoint (
     std::wstring  const & label,
     rti1516e::VariableLengthData const & theUserSuppliedTag)
{
    if (label == L"InitialSync") {
        std::wcout << L"Master Federate synchronized at InitialSync." << std::endl;
        syncPointRegistered = true;
    }
}