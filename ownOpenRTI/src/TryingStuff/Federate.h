#ifndef testFederate_H
#define testFederate_H

#include <RTI/RTI1516.h>
#include <RTI/time/HLAfloat64Interval.h>
#include <RTI/time/HLAfloat64Time.h>
#include <string>
#include <vector>

class testFederate {
    public:

        // fom handles //
        ObjectClassHandle vehicleClassHandle;
        AttributeHandle positionHandle;
        AttributeHandle speedHandle;

        testFederate();
        virtual ~testFederate();
        void runFederate(std::wstring federateName);

    private:
        void initializeHandles();
}