#include <iostream>
#include <RTI/TIME/HLAfloat64Time.h>
#include "testFedAmb.h"

testFedAmb::testFedAmb(){
    this->isRegulating = false;
    this->isConstrained = false;
    this->federateTime = 0.0;
    this->federateLookahead = 1.0;
}