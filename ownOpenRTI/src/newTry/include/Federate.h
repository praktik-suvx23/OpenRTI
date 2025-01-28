#ifndef FEDERATE_H
#define FEDERATE_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAinteger64TimeFactory.h>
#include <memory>

class Federate {
public:
    Federate();
    ~Federate();

    void initialize(); 
    void run();        
    void finalize();   

private:
    void joinFederation();
    void resignFederation();

    int _state;
    std::unique_ptr<rti1516e::RTIambassador> _rtiAmbassador;
    std::unique_ptr<rti1516e::FederateAmbassador> _federateAmbassador;
};

#endif // FEDERATE_H