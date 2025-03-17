#ifndef MISSILEMANAGERHELPER_H
#define MISSILEMANAGERHELPER_H

#include "../MissileManagerFederateAmbassador.h"

/*
    IDEA:
    This is suppose to be a class that helps with the get/set methods for the MissileManagerFederateAmbassador.
    I belive the code got too bloated and hard to read with all the get/set methods in the MissileManagerFederateAmbassador.
    That is why I created this helper class to make it easier to read and understand the code.

    TODO:
    - Discuss if this is a good idea
    - One big class that does everything or split it up in smaller classes?
    - Placement of the helper class and name. New include folder in 'MissileManagerFederate'?
    - Move all used get/set methods from MissileManagerFederateAmbassador to this class
    - Clean up the get/set methods in MissileManagerFederateAmbassador
    - Make something similar for the Ships as well

    IMPORTANT:
    - Write a comment for each get/set group to explain where they are used to 
        make it easier to determine if they are still in use or not.
*/
class MissileManagerGetter {
    public:
    /* 1. Getter Methods for interaction class and parameters: HLAinteractionRoot.FireMissile */
    static rti1516e::InteractionClassHandle getFireMissileHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.fireMissileHandle;}
    static rti1516e::ParameterHandle getShooterIDParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.shooterIDParamHandle;}
    static rti1516e::ParameterHandle getTargetIDParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.targetIDParamHandle;}
    static rti1516e::ParameterHandle getShooterPositionParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.shooterPositionParamHandle;}
    static rti1516e::ParameterHandle getTargetPositionParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.targetPositionParamHandle;}
    static rti1516e::ParameterHandle getMissileCountParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.missileCountParamHandle;}
    static rti1516e::ParameterHandle getMissileTypeParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.missileTypeParamHandle;}
    static rti1516e::ParameterHandle getMaxDistanceParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.maxDistanceParamHandle;}
    static rti1516e::ParameterHandle getMissileSpeedParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.missileSpeedParamHandle;}
    static rti1516e::ParameterHandle getLockOnDistanceParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.lockOnDistanceParamHandle;}
    static rti1516e::ParameterHandle getFireTimeParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.fireTimeParamHandle;}

    /* 2. Getter Methods for object class and attributes: HLAobjectRoot.Ship */
    static rti1516e::ObjectClassHandle getShipClassHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.shipClassHandle;}
    static rti1516e::AttributeHandle getAttributeHandleShipID(MissileManagerAmbassador& ambassador) {
        return ambassador.attributeHandleShipID;}
    static rti1516e::AttributeHandle getAttributeHandleShipTeam(MissileManagerAmbassador& ambassador) {
        return ambassador.attributeHandleShipTeam;}
    static rti1516e::AttributeHandle getAttributeHandleShipPosition(MissileManagerAmbassador& ambassador) {
        return ambassador.attributeHandleShipPosition;}
    static rti1516e::AttributeHandle getAttributeHandleShipSpeed(MissileManagerAmbassador& ambassador) {
        return ambassador.attributeHandleShipSpeed;}
    static rti1516e::AttributeHandle getAttributeHandleShipSize(MissileManagerAmbassador& ambassador) {
        return ambassador.attributeHandleShipSize;}
    static rti1516e::AttributeHandle getAttributeHandleShipAngle(MissileManagerAmbassador& ambassador) {
        return ambassador.attributeHandleShipAngle;}
    static rti1516e::AttributeHandle getAttributeHandleNumberOfMissiles(MissileManagerAmbassador& ambassador) {
        return ambassador.attributeHandleNumberOfMissiles;}

    /* 3. Getter Methods for object class and attributes: HLAobjectRoot.Missile */
    static rti1516e::ObjectClassHandle getMissileClassHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.missileClassHandle;}
    static rti1516e::AttributeHandle getAttributeHandleMissileID(MissileManagerAmbassador& ambassador) {
        return ambassador.attributeHandleMissileID;}
    static rti1516e::AttributeHandle getAttributeHandleSpeed(MissileManagerAmbassador& ambassador) {
        return ambassador.attributeHandleSpeed;}
    static rti1516e::AttributeHandle getAttributeHandleFuelLevel(MissileManagerAmbassador& ambassador) {
        return ambassador.attributeHandleFuelLevel;}    
    static rti1516e::AttributeHandle getAttributeHandlePosition(MissileManagerAmbassador& ambassador) {
        return ambassador.attributeHandlePosition;}
    static rti1516e::AttributeHandle getAttributeHandleAltitude(MissileManagerAmbassador& ambassador) {
        return ambassador.attributeHandleAltitude;}
    static rti1516e::AttributeHandle getAttributeHandleTargetID(MissileManagerAmbassador& ambassador) {
        return ambassador.attributeHandleTargetID;}
    static rti1516e::AttributeHandle getAttributeHandleMissileTeam(MissileManagerAmbassador& ambassador) {
        return ambassador.attributeHandleMissileTeam;}

    // 4. Generic getter methods for general attributes
    
    // 4.1 Getter methods for receiveInteraction variables
    /*
        Currently only setFlagActiveMissile, setShooterID, setTargetID, setShooterPosition and setTargetPosition are used.
        The rest of the methods are placeholders for future use.

        setFireTime - might not be necessary.
        Might need a ship size, so missile know what kind of ship to look for when locking on.
        Might need X number of set cordinates for a path to follow.
    */
    static bool getFlagActiveMissile(MissileManagerAmbassador& ambassador) {
        return ambassador.flagActiveMissile;}
    static std::wstring getShooterID(MissileManagerAmbassador& ambassador) {
        return ambassador.shooterID;}
    static std::wstring getTargetID(MissileManagerAmbassador& ambassador) {
        return ambassador.targetID;}
    static std::wstring getMissileType(MissileManagerAmbassador& ambassador) {
        return ambassador.missileType;}
    static double getMissileSpeed(MissileManagerAmbassador& ambassador) {
        return ambassador.missileSpeed;}
    static double getMaxDistance(MissileManagerAmbassador& ambassador) {  
        return ambassador.maxDistance;}
    static double getLockOnDistance(MissileManagerAmbassador& ambassador) {
        return ambassador.lockOnDistance;}
    static double getFireTime(MissileManagerAmbassador& ambassador) {
        return ambassador.fireTime;}
    static int getMissileCount(MissileManagerAmbassador& ambassador) {
        return ambassador.missileCount;}
    static std::pair<double, double> getShooterPosition(MissileManagerAmbassador& ambassador) {
        return ambassador.shooterPosition;}
    static std::pair<double, double> getTargetPosition(MissileManagerAmbassador& ambassador) {
        return ambassador.targetPosition;}
};

class MissileManagerSetter{
    public:
    /* 1. Setter Methods for interaction class and parameters: HLAinteractionRoot.FireMissile */
    static void setFireMissileHandle(MissileManagerAmbassador& ambassador, rti1516e::InteractionClassHandle handle) {
        ambassador.fireMissileHandle = handle;}
    static void setShooterIDParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.shooterIDParamHandle = handle;}
    static void setTargetIDParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.targetIDParamHandle = handle;}
    static void setShooterPositionParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.shooterPositionParamHandle = handle;}
    static void setTargetPositionParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.targetPositionParamHandle = handle;}
    static void setMissileCountParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.missileCountParamHandle = handle;}
    static void setMissileTypeParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.missileTypeParamHandle = handle;}
    static void setMaxDistanceParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.maxDistanceParamHandle = handle;}
    static void setMissileSpeedParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.missileSpeedParamHandle = handle;}
    static void setLockOnDistanceParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.lockOnDistanceParamHandle = handle;}
    static void setFireTimeParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.fireTimeParamHandle = handle;}

    /* 2. Setter Methods for object class and attributes: HLAobjectRoot.Ship */
    static void setShipClassHandle(MissileManagerAmbassador& ambassador, rti1516e::ObjectClassHandle handle) {
        ambassador.shipClassHandle = handle;}
    static void setAttributeHandleShipID(MissileManagerAmbassador& ambassador, rti1516e::AttributeHandle handle) {
        ambassador.attributeHandleShipID = handle;}
    static void setAttributeHandleShipTeam(MissileManagerAmbassador& ambassador, rti1516e::AttributeHandle handle) {
        ambassador.attributeHandleShipTeam = handle;}
    static void setAttributeHandleShipPosition(MissileManagerAmbassador& ambassador, rti1516e::AttributeHandle handle) {
        ambassador.attributeHandleShipPosition = handle;}
    static void setAttributeHandleShipSpeed(MissileManagerAmbassador& ambassador, rti1516e::AttributeHandle handle) {
        ambassador.attributeHandleShipSpeed = handle;}
    static void setAttributeHandleShipSize(MissileManagerAmbassador& ambassador, rti1516e::AttributeHandle handle) {
        ambassador.attributeHandleShipSize = handle;}
    static void setAttributeHandleShipAngle(MissileManagerAmbassador& ambassador, rti1516e::AttributeHandle handle) {
        ambassador.attributeHandleShipAngle = handle;}
    static void setAttributeHandleNumberOfMissiles(MissileManagerAmbassador& ambassador, rti1516e::AttributeHandle handle) {
        ambassador.attributeHandleNumberOfMissiles = handle;}

    /* 3. Setter Methods for object class and attributes: HLAobjectRoot.Missile */
    static void setMissileClassHandle(MissileManagerAmbassador& ambassador, rti1516e::ObjectClassHandle handle) {
        ambassador.missileClassHandle = handle;}
    static void setAttributeHandleMissileID(MissileManagerAmbassador& ambassador, rti1516e::AttributeHandle handle) {
        ambassador.attributeHandleMissileID = handle;}
    static void setAttributeHandleSpeed(MissileManagerAmbassador& ambassador, rti1516e::AttributeHandle handle) {
        ambassador.attributeHandleSpeed = handle;}
    static void setAttributeHandleFuelLevel(MissileManagerAmbassador& ambassador, rti1516e::AttributeHandle handle) {
        ambassador.attributeHandleFuelLevel = handle;}
    static void setAttributeHandlePosition(MissileManagerAmbassador& ambassador, rti1516e::AttributeHandle handle) {
        ambassador.attributeHandlePosition = handle;}
    static void setAttributeHandleAltitude(MissileManagerAmbassador& ambassador, rti1516e::AttributeHandle handle) {
        ambassador.attributeHandleAltitude = handle;}
    static void setAttributeHandleTargetID(MissileManagerAmbassador& ambassador, rti1516e::AttributeHandle handle) {
        ambassador.attributeHandleTargetID = handle;}
    static void setAttributeHandleMissileTeam(MissileManagerAmbassador& ambassador, rti1516e::AttributeHandle handle) {
        ambassador.attributeHandleMissileTeam = handle;}

    // 4. Generic setter methods for general attributes

    // 4.1 Setter methods for receiveInteraction variables
    /*
        Currently only setFlagActiveMissile, setShooterID, setTargetID, setShooterPosition and setTargetPosition are used.
        The rest of the methods are placeholders for future use.

        setFireTime - might not be necessary.
        Might need a ship size, so missile know what kind of ship to look for when locking on.
        Might need X number of set cordinates for a path to follow.
    */
    static void setFlagActiveMissile(MissileManagerAmbassador& ambassador, bool flag) {
        ambassador.flagActiveMissile = flag;}
    static void setShooterID(MissileManagerAmbassador& ambassador, std::wstring id) {
        ambassador.shooterID = id;}
    static void setTargetID(MissileManagerAmbassador& ambassador, std::wstring id) {
        ambassador.targetID = id;}
    static void setMissileType(MissileManagerAmbassador& ambassador, std::wstring type) {
        ambassador.missileType = type;}
    static void setMissileSpeed(MissileManagerAmbassador& ambassador, double speed) {
        ambassador.missileSpeed = speed;}
    static void setMaxDistance(MissileManagerAmbassador& ambassador, double distance) {
        ambassador.maxDistance = distance;}
    static void setLockOnDistance(MissileManagerAmbassador& ambassador, double distance) {
        ambassador.lockOnDistance = distance;}
    static void setFireTime(MissileManagerAmbassador& ambassador, double time) {
        ambassador.fireTime = time;}
    static void setMissileCount(MissileManagerAmbassador& ambassador, int count) {
        ambassador.missileCount = count;}
    static void setShooterPosition(MissileManagerAmbassador& ambassador, std::pair<double, double> position) {
        ambassador.shooterPosition = position;}
    static void setTargetPosition(MissileManagerAmbassador& ambassador, std::pair<double, double> position) {
        ambassador.targetPosition = position;}
};

#endif // MISSILEMANAGERHELPER_H