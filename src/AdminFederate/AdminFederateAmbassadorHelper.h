#ifndef ADMINFEDERATEAMBASSADORHELPER_H
#define ADMINFEDERATEAMBASSADORHELPER_H

#include "AdminFederateAmbassador.h"

class AmbassadorGetter {
public:
    static std::wstring getSyncLabel(AdminFederateAmbassador& ambassador) {
        return ambassador.syncLabel;}
    
    static rti1516e::InteractionClassHandle getSetupSimulationHandle(AdminFederateAmbassador& ambassador) {
        return ambassador.setupSimulationHandle;}
    static rti1516e::ParameterHandle getBlueShipsParam(AdminFederateAmbassador& ambassador) {
        return ambassador.blueShipsParam;}
    static rti1516e::ParameterHandle getRedShipsParam(AdminFederateAmbassador& ambassador) {
        return ambassador.redShipsParam;}
    static rti1516e::ParameterHandle getTimeScaleFactorParam(AdminFederateAmbassador& ambassador) {
        return ambassador.timeScaleFactorParam;}
};

class AmbassadorSetter {
public:
    static void setSyncLabel(AdminFederateAmbassador& ambassador, std::wstring label) {
        ambassador.syncLabel = label;}

    static void setSetupSimulationHandle(AdminFederateAmbassador& ambassador, rti1516e::InteractionClassHandle handle) {
        ambassador.setupSimulationHandle = handle;}
    static void setBlueShipsParam(AdminFederateAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.blueShipsParam = handle;}
    static void setRedShipsParam(AdminFederateAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.redShipsParam = handle;}
    static void setTimeScaleFactorParam(AdminFederateAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.timeScaleFactorParam = handle;}
};
#endif