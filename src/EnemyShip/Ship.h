#ifndef SHIP_H
#define SHIP_H

#include <string>
#include <RTI/RTI1516.h>

struct Ship {
    rti1516e::ObjectInstanceHandle objectInstanceHandle;
    std::wstring shipName;
    std::wstring shipPosition;
    double shipSpeed;
    int numberOfMissiles;
    double HP;
};

#endif // SHIP_H