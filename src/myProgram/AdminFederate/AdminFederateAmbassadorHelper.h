#ifndef ADMINFEDERATEAMBASSADORHELPER_H
#define ADMINFEDERATEAMBASSADORHELPER_H

#include "AdminFederateAmbassador.h"

class AmbassadorGetter {
public:
    static std::wstring getSyncLabel(AdminFederateAmbassador& ambassador) {
        return ambassador.syncLabel;}
};

class AmbassadorSetter {
public:
    static void setSyncLabel(AdminFederateAmbassador& ambassador, std::wstring label) {
        ambassador.syncLabel = label;}
};
#endif