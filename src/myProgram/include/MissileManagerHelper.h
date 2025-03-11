#ifndef MISSILEMANAGERHELPER_H
#define MISSILEMANAGERHELPER_H

#include "../MissileManagerFederate/MissileManagerFederateAmbassador.h"

/*
    Was intended to be:
    Simple helper class to get and set the handles for the MissileManagerFederateAmbassador

    Not currently working / in use
    TODO: Implement this and clean up get/set methods in MissileManagerFederateAmbassador
*/
class MissileManagerHelper {
    public:
    // Getter Methods
    static rti1516e::InteractionClassHandle getFireMissileHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.fireMissileHandle;
    }

    static rti1516e::ParameterHandle getShooterIDParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.shooterIDParamHandle;
    }

    static rti1516e::ParameterHandle getTargetIDParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.targetIDParamHandle;
    }

    static rti1516e::ParameterHandle getShooterPositionParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.shooterPositionParamHandle;
    }

    static rti1516e::ParameterHandle getTargetPositionParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.targetPositionParamHandle;
    }

    static rti1516e::ParameterHandle getMissileCountParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.missileCountParamHandle;
    }

    static rti1516e::ParameterHandle getMissileTypeParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.missileTypeParamHandle;
    }

    static rti1516e::ParameterHandle getMaxDistanceParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.maxDistanceParamHandle;
    }

    static rti1516e::ParameterHandle getMissileSpeedParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.missileSpeedParamHandle;
    }

    static rti1516e::ParameterHandle getLockOnDistanceParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.lockOnDistanceParamHandle;
    }

    static rti1516e::ParameterHandle getFireTimeParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.fireTimeParamHandle;
    }

    // Setter Methods
    static void setFireMissileHandle(MissileManagerAmbassador& ambassador, rti1516e::InteractionClassHandle handle) {
        ambassador.fireMissileHandle = handle;
    }

    static void setShooterIDParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.shooterIDParamHandle = handle;
    }

    static void setTargetIDParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.targetIDParamHandle = handle;
    }

    static void setShooterPositionParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.shooterPositionParamHandle = handle;
    }

    static void setTargetPositionParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.targetPositionParamHandle = handle;
    }

    static void setMissileCountParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.missileCountParamHandle = handle;
    }

    static void setMissileTypeParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.missileTypeParamHandle = handle;
    }

    static void setMaxDistanceParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.maxDistanceParamHandle = handle;
    }

    static void setMissileSpeedParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.missileSpeedParamHandle = handle;
    }

    static void setLockOnDistanceParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.lockOnDistanceParamHandle = handle;
    }

    static void setFireTimeParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.fireTimeParamHandle = handle;
    }
};

#endif // MISSILEMANAGERHELPER_H