#include "MyShipFederateAmbassador.h"

MyShipFederateAmbassador::MyShipFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador) {}

MyShipFederateAmbassador::~MyShipFederateAmbassador() {}

void MyShipFederateAmbassador::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag)
{
    if (label == L"InitialSync") {
        std::wcout << L"Publisher Federate received synchronization announcement: InitialSync." << std::endl;
        syncLabel = label;
    }
}

std::wstring MyShipFederateAmbassador::getSyncLabel() const {
    return syncLabel;
}

std::wstring MyShipFederateAmbassador::getFederateName() const {
    return federateName;
}

void MyShipFederateAmbassador::setFederateName(std::wstring name) {
    federateName = name;
}