#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAinteger64TimeFactory.h>
#include <memory>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

class MyFederateAmbassador : public rti1516e::NullFederateAmbassador {
public:
    void discoverObjectInstance(const rti1516e::ObjectClassHandle& objectHandle, 
                            const rti1516e::FederateHandle& federateHandle, 
                            const char* objectName);
};

int main() {
    try {
        rti1516e::RTIambassadorFactory factory;
        std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador = factory.createRTIambassador();
        
        MyFederateAmbassador ambassador;
        rtiAmbassador->connect(ambassador, rti1516e::HLA_EVOKED);

        // Federation and federate setup
        rtiAmbassador->createFederationExecution(L"MyFederation", L"FOM.xml");
        rtiAmbassador->joinFederationExecution(L"MyFederate", L"MyFederation");

        std::cout << "Federate joined the federation." << std::endl;

        // Run simulation loop or additional logic here...

        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        rtiAmbassador->destroyFederationExecution(L"MyFederation");
    } catch (const rti1516e::Exception& e) {
        std::cerr << "Exception: " << std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(e.what()) << std::endl;
    }
    return 0;
}
