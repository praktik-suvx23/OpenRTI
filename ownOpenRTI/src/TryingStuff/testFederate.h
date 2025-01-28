#ifndef testFederate_H
#define testFederate_H

#include "testFedAmb.h"

#include <RTI/RTI1516.h>
#include <RTI/time/HLAfloat64Interval.h>
#include <RTI/time/HLAfloat64Time.h>
#include <string>
#include <vector>

#define READY_TO_RUN L"ReadyToRun"

class testFederate {
    public:
        rti1516e::RTIambassador* rtiAmbassador;
        testFedAmb* fedAmbassador;

        // fom handles //
        ObjectClassHandle vehicleClassHandle;
        AttributeHandle positionHandle;
        AttributeHandle speedHandle;

        InteractionClassHandle updateInteractionHandle;
        ParameterHandle positionParameterHandle;

        testFederate();
        virtual ~testFederate();
        void runFederate(const std::wstring& federateName);

    private:
        void initializeHandles();
        void waitForUser();
        void enableTimePolicy();
        void publishAndSubscribe();
        rti1516e::ObjectInstanceHandle registerObject();
        void updateAttributeValues(rti1516e::ObjectInstanceHandle objectHandle);
        void sendInteraction();
        void advanceTime(double timestep);
};

#endif