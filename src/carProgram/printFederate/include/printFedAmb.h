#ifndef MASTFEDAMB_H
#define MASTFEDAMB_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/encoding/EncodingExceptions.h>
#include <RTI/encoding/DataElement.h>
#include "../../include/MyPositionDecoder.h"
#include "../../include/MyFloat32Decoder.h"
#include "../../include/ObjectInstanceHandleHash.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_map>

class mastFedAmb : public rti1516e::NullFederateAmbassador {
public:
    mastFedAmb(rti1516e::RTIambassador* rtiAmbassador);
    ~mastFedAmb();

    

    std::wstring syncLabel = L"";

private:
    rti1516e::RTIambassador* _rtiAmbassador;
};

#endif