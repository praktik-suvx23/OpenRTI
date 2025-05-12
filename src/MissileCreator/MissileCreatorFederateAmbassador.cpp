#include "MissileCreatorFederateAmbassador.h"


MissileCreatorFederateAmbassador::MissileCreatorFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador) : _rtiAmbassador(rtiAmbassador) {
}
MissileCreatorFederateAmbassador::~MissileCreatorFederateAmbassador() {
}

void MissileCreatorFederateAmbassador::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag) {
    if (label == L"InitialSync") {
        std::wcout << L"[INFO - SyncPoint] Federate received synchronization announcement: InitialSync." << std::endl;
        syncLabel = label;
    }
    if (label == L"SimulationSetupComplete") {
        std::wcout << L"[INFO - SyncPoint] Federate synchronized at SimulationSetupComplete." << std::endl;
        syncLabel = label;
    }
}
void MissileCreatorFederateAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const& theObjectName) {
    //std::wcout << L"Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;
}

void MissileCreatorFederateAmbassador::receiveInteraction(    //Receive interaction without time
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    rti1516e::SupplementalReceiveInfo receiveInfo) {
    std::wcout << L"[DEBUG] Received interaction: " << interactionClassHandle << std::endl;

    if (interactionClassHandle == interactionClassFireMissile) {
        // Handle the interaction
        std::wcout << L"[INFO] FireMissile interaction received." << std::endl;
        //Somehow know where to start missileFederates based on simulation load.
        //Send start missile interaction to the missile federate
        auto itShooterID = parameterValues.find(parameterHandleShooterID);
        auto itMissileTeam = parameterValues.find(parameterHandleMissileTeam);
        auto itMissileStartPosition = parameterValues.find(parameterHandleMissileStartPosition);
        auto itMissileTargetPosition = parameterValues.find(parameterHandleMissileTargetPosition);
        auto itNumberOfMissilesFired = parameterValues.find(parameterHandleNumberOfMissilesFired);

        if (itShooterID != parameterValues.end() 
        &&  itMissileTeam != parameterValues.end() 
        &&  itMissileStartPosition != parameterValues.end() 
        &&  itMissileTargetPosition != parameterValues.end() 
        &&  itNumberOfMissilesFired != parameterValues.end()) {

            rti1516e::HLAunicodeString tempValue;
            tempValue.decode(itShooterID->second);
            std::wstring shooterID = tempValue.get();

            tempValue.decode(itMissileTeam->second);
            std::wstring missileTeam = tempValue.get();

            auto startPosition = decodePositionRec(itMissileStartPosition->second);
            auto targetPosition = decodePositionRec(itMissileTargetPosition->second);

            double missileDirection = calculateInitialBearingDouble(
                startPosition.first,
                startPosition.second,
                targetPosition.first,
                targetPosition.second
            );

            rti1516e::HLAinteger32BE numberOfMissilesFired;
            numberOfMissilesFired.decode(itNumberOfMissilesFired->second);

            // Eventually add so that it send the interaction to the least overloaded missile federate

            StartMissile(
                shooterID,
                missileTeam,
                startPosition,
                targetPosition,
                numberOfMissilesFired.get(),
                missileDirection //Target bearing, this should be calculated based on the start and target position
            ); 
                       
            std::wcout << L"[INFO] StartMissile interaction sent." << std::endl;
        }
    }
}

void MissileCreatorFederateAmbassador::reflectAttributeValues(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::AttributeHandleValueMap const& theAttributes,
    rti1516e::VariableLengthData const& theTag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType theType,
    rti1516e::SupplementalReflectInfo theReflectInfo) {
    // Handle attribute values
}

std::string wstringToString(const std::wstring& wstr) {
    std::string str(wstr.begin(), wstr.end());
    return str;
}

std::string PairToString(const std::pair<double, double>& pair) {
    return std::to_string(pair.first) + "," + std::to_string(pair.second);
}


void MissileCreatorFederateAmbassador::StartMissile( //Make this to const pointers instead to save process time and memory
    const std::wstring& shooterID,
    const std::wstring& missileTeam,
    const std::pair<double, double>& missileStartPosition,
    const std::pair<double, double>& missileTargetPosition,
    const int& numberOfMissilesFired,
    const double& initialBearing) {
    // Send the start missile interaction to the missile federate
    // This function should be implemented to send the interaction with the required parameters
    // using the RTIambassador.
    try {
        for (int i = 0; i < numberOfMissilesFired; ++i) {
            // Create a new process for each missile, input correct logic here
                pid_t pid = fork();
            if (pid < 0) { 
                std::wcerr << L"[ERROR] Fork failed." << std::endl;
                return;
            }
            else if (pid == 0) { // Child process
                std::vector<std::string> argStrings;
                argStrings.push_back("MissileFederate");
                argStrings.push_back(wstringToString(shooterID));
                argStrings.push_back(wstringToString(missileTeam));
                argStrings.push_back(PairToString(missileStartPosition));
                argStrings.push_back(PairToString(missileTargetPosition));
                argStrings.push_back(std::to_string(initialBearing));

                std::vector<char*> args;
                for (auto& s : argStrings) args.push_back(&s[0]);
                args.push_back(nullptr);

                execv("./MissileHandler", args.data());
                // If execv fails
                perror("execv failed");
                exit(1);
            }
        }
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] sendStartMissileInteraction - Exception: " << e.what() << std::endl;
    }
}

//--- Get set methods Below ---
std::wstring MissileCreatorFederateAmbassador::getSyncLabel() const {
    return syncLabel;
}


// Get set fire missile interaction
void MissileCreatorFederateAmbassador::setInteractioClassFireMissile(rti1516e::InteractionClassHandle interactionClassHandle) {
    interactionClassFireMissile = interactionClassHandle;
}
rti1516e::InteractionClassHandle MissileCreatorFederateAmbassador::getInteractioClassFireMissile() const {
    return interactionClassFireMissile;
}

void MissileCreatorFederateAmbassador::setParamShooterID(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleShooterID = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamShooterID() const {
    return parameterHandleShooterID;
}

void MissileCreatorFederateAmbassador::setParamMissileTeam(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleMissileTeam = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamMissileTeam() const {
    return parameterHandleMissileTeam;
}

void MissileCreatorFederateAmbassador::setParamMissileStartPosition(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleMissileStartPosition = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamMissileStartPosition() const {
    return parameterHandleMissileStartPosition;
}

void MissileCreatorFederateAmbassador::setParamMissileTargetPosition(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleMissileTargetPosition = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamMissileTargetPosition() const {
    return parameterHandleMissileTargetPosition;
}

void MissileCreatorFederateAmbassador::setParamNumberOfMissilesFired(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleNumberOfMissilesFired = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamNumberOfMissilesFired() const {
    return parameterHandleNumberOfMissilesFired;
}
void MissileCreatorFederateAmbassador::setParamMissileSpeed(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleMissileSpeed = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamMissileSpeed() const {
    return parameterHandleMissileSpeed;
}
//------

//get set create missile interaction
void MissileCreatorFederateAmbassador::setInteractioClassCreateMissile(rti1516e::InteractionClassHandle interactionClassHandle) {
    interactionClassCreateMissile = interactionClassHandle;
}
rti1516e::InteractionClassHandle MissileCreatorFederateAmbassador::getInteractioClassCreateMissile() const {
    return interactionClassCreateMissile;
}

void MissileCreatorFederateAmbassador::setParamCreateMissileID(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleCreateMissileID = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamCreateMissileID() const {
    return parameterHandleCreateMissileID;
}

void MissileCreatorFederateAmbassador::setParamCreateMissileTeam(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleCreateMissileTeam = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamCreateMissileTeam() const {
    return parameterHandleCreateMissileTeam;
}

void MissileCreatorFederateAmbassador::setParamCreateMissilePosition(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleCreateMissilePosition = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamCreateMissilePosition() const {
    return parameterHandleCreateMissilePosition;
}

void MissileCreatorFederateAmbassador::setParamCreateMissileTargetPosition(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleCreateMissileTargetPosition = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamCreateMissileTargetPosition() const {
    return parameterHandleCreateMissileTargetPosition;
}

void MissileCreatorFederateAmbassador::setParamCreateMissileNumberOfMissilesFired(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleCreateMissileNumberOfMissilesFired = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamCreateMissileNumberOfMissilesFired() const {
    return parameterHandleCreateMissileNumberOfMissilesFired;
}

void MissileCreatorFederateAmbassador::setParamCreateMissileBearing(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleCreateMissileBearing = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamCreateMissileBearing() const {
    return parameterHandleCreateMissileBearing;
}
//------
//--- End of get set methods ---
