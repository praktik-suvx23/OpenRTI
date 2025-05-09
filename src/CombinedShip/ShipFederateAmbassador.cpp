#include "ShipFederateAmbassador.h"

MyShipFederateAmbassador::MyShipFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador) 
    : _rtiambassador(rtiAmbassador)  {
}

MyShipFederateAmbassador::~MyShipFederateAmbassador() {}

void MyShipFederateAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const &theObjectName) {
        std::wcout << L"[DEBUG] Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass <<
        ". Number of found objects: " << numberOfDiscoveredObjects << std::endl;

    numberOfDiscoveredObjects++;
    shipMap[theObject] = Ship(theObject);
}

void MyShipFederateAmbassador::reflectAttributeValues(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::AttributeHandleValueMap const &theAttributes,
    rti1516e::VariableLengthData const &theTag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType theType,
    rti1516e::LogicalTime const & theTime,
    rti1516e::OrderType receivedOrder,
    rti1516e::SupplementalReflectInfo theReflectInfo) {

    if (shipMap.find(theObject) == shipMap.end()) {
        std::wcerr << L"[ERROR] Object not found in ship map: " << theObject << std::endl;
        return;
    }

    auto& ship = shipMap[theObject];

    if (ship.shipName == L"") {
        const auto itShipFederateName = theAttributes.find(attributeHandleShipFederateName);
        if (itShipFederateName != theAttributes.end()) {
            rti1516e::HLAunicodeString attributeValueFederateName;
            attributeValueFederateName.decode(itShipFederateName->second);
            ship.shipName = attributeValueFederateName.get();
            std::wcout << L"[INFO] Updated target federate name: " << ship.shipName << std::endl;
        } else {
            std::wcerr << L"[INFO] Attribute handle for ship federate name not found" << std::endl;
        }
    }

    if (ship.shipTeam == L"") {
        const auto itShipTeam = theAttributes.find(attributeHandleShipTeam);
        if (itShipTeam != theAttributes.end()) {
            rti1516e::HLAunicodeString attributeValueTeam;
            attributeValueTeam.decode(itShipTeam->second);
            ship.shipTeam = attributeValueTeam.get();
            std::wcout << L"[INFO] Updated target ship team: " << ship.shipTeam << std::endl;
        } else {
            std::wcerr << L"[INFO] Attribute handle for ship team not found" << std::endl;
        }
    }

    const auto itShipPosition = theAttributes.find(attributeHandleShipPosition);
    if (itShipPosition != theAttributes.end()) {
        std::pair<double, double> tempShipPosition = decodePositionRec(itShipPosition->second);
        ship.shipPosition = tempShipPosition;
    } else {
        std::wcerr << L"[INFO] Attribute handle for ship position not found" << std::endl;
    }

    const auto itShipSpeed = theAttributes.find(attributeHandleShipSpeed);
    if (itShipSpeed != theAttributes.end()) {
        rti1516e::HLAfloat64BE attributeValueSpeed;
        attributeValueSpeed.decode(itShipSpeed->second);
        ship.shipSpeed = attributeValueSpeed.get();
    } else {
        std::wcerr << L"[INFO] Attribute handle for ship speed not found" << std::endl;
    }

    const auto itShipNumberOfMissiles = theAttributes.find(attributeHandleNumberOfMissiles);
    if (itShipNumberOfMissiles != theAttributes.end()) {
        rti1516e::HLAinteger32BE attributeValueNumberOfMissiles;
        attributeValueNumberOfMissiles.decode(itShipNumberOfMissiles->second);
        ship.shipNumberOfMissiles = attributeValueNumberOfMissiles.get();
    } else {
        std::wcerr << L"[INFO] Attribute handle for ship number of missiles not found" << std::endl;
    }

    const auto itShipMaxMissilesLockingThisShip = theAttributes.find(attributeHandleMaxMissilesLockingThisShip);
    if (itShipMaxMissilesLockingThisShip != theAttributes.end()) {
        rti1516e::HLAinteger32BE attributeValueMaxMissilesLockingThisShip;
        attributeValueMaxMissilesLockingThisShip.decode(itShipMaxMissilesLockingThisShip->second);
        ship.maxMissilesLocking = attributeValueMaxMissilesLockingThisShip.get();
    } else {
        std::wcerr << L"[INFO] Attribute handle for ship max missiles locking this ship not found" << std::endl;
    }

    const auto itShipCurrentMissilesLockingThisShip = theAttributes.find(attributeHandleCurrentMissilesLockingThisShip);
    if (itShipCurrentMissilesLockingThisShip != theAttributes.end()) {
        rti1516e::HLAinteger32BE attributeValueCurrentMissilesLockingThisShip;
        attributeValueCurrentMissilesLockingThisShip.decode(itShipCurrentMissilesLockingThisShip->second);
        ship.currentMissilesLocking = attributeValueCurrentMissilesLockingThisShip.get();
    } else {
        std::wcerr << L"[INFO] Attribute handle for ship current missiles locking this ship not found" << std::endl;
    }

    if(ship.shipTeam == L"Red") {
        if(std::find(redShipsVector.begin(), redShipsVector.end(), &ship) == redShipsVector.end()) {
            std::wcout << L"[INFO] Adding ship to red ships vector" << std::endl;
            redShipsVector.push_back(&ship);
        }
    } else if(ship.shipTeam == L"Blue") {
        if(std::find(blueShipsVector.begin(), blueShipsVector.end(), &ship) == blueShipsVector.end()) {
            std::wcout << L"[INFO] Adding ship to blue ships vector" << std::endl;
            blueShipsVector.push_back(&ship);
        }
    } else {
        std::wcerr << L"[ERROR] Unknown ship team: " << ship.shipTeam << std::endl;
    }

    std::wcout << L"[UPDATE] Ship: " << ship.shipName << L" received update." << std::endl;
}

//TSO (Time Stamp Order) 
//This function is called when an interaction is received with a time stamp
void MyShipFederateAmbassador::receiveInteraction(
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    const rti1516e::LogicalTime& theTime,
    rti1516e::OrderType receivedOrder,
    rti1516e::SupplementalReceiveInfo receiveInfo) {

    std::wcout << L"[DEBUG] Receive interaction called with time" << std::endl;

    if (interactionClassHandle == interactionClassTargetHit) {
        auto it = parameterValues.find(parameterHandleTargetHitID);
        if (it != parameterValues.end()) {
            rti1516e::VariableLengthData attributeValue = it->second;
            rti1516e::HLAunicodeString value;
            value.decode(attributeValue);
            std::wstring targetID = value.get();

            for (auto ownShip : ownShipsVector) {
                if (ownShip->shipName == targetID) {
                    ownShip->shipHP -= 50;
                    if (ownShip->shipHP <= 0) {
                        std::wcout << L"[DESTROYED] Own ship: " << ownShip->shipName << std::endl;
                        ownShipsVector.erase(std::remove(ownShipsVector.begin(), ownShipsVector.end(), ownShip), ownShipsVector.end());
                    } else {
                        std::wcout << L"[HIT] Own ship: " << ownShip->shipName << L", HP now: " << ownShip->shipHP << std::endl;
                    }
                }
            }

            if (teamStatus == ShipTeam::BLUE) {
                for (auto it = blueShipsVector.begin(); it != blueShipsVector.end(); ) {
                    Ship* ship = *it;

                    if (ship->shipName == targetID) {
                        ship->shipHP -= 50; // Apply damage

                        if (ship->shipHP <= 0) {
                            std::wcout << L"[DESTROYED] Ship: " << ship->shipName << std::endl;

                            // Remove ship from blueShipsVector list
                            it = blueShipsVector.erase(it);
                            // Optional: remove from shipMap if you want full deletion
                            // shipMap.erase(ship->objectInstanceHandle);

                            continue; // Skip incrementing
                        } else {
                            std::wcout << L"[HIT] Ship: " << ship->shipName << L", HP now: " << ship->shipHP << std::endl;
                        }
                    }

                    ++it;
                }
            } else if (teamStatus == ShipTeam::RED) {
                for (auto it = redShipsVector.begin(); it != redShipsVector.end(); ) {
                    Ship* ship = *it;

                    if (ship->shipName == targetID) {
                        ship->shipHP -= 50; // Apply damage

                        if (ship->shipHP <= 0) {
                            std::wcout << L"[DESTROYED] Ship: " << ship->shipName << std::endl;

                            // Remove ship from redShipsVector list
                            it = redShipsVector.erase(it);
                            // Optional: remove from shipMap if you want full deletion
                            // shipMap.erase(ship->objectInstanceHandle);

                            continue; // Skip incrementing
                        } else {
                            std::wcout << L"[HIT] Ship: " << ship->shipName << L", HP now: " << ship->shipHP << std::endl;
                        }
                    }

                    ++it;
                }
            }
        }
    }
    else if (interactionClassHandle == interactionClassFireMissile) {
        auto itShooterTeam = parameterValues.find(parameterHandleMissileTeam);
        auto itTargetID = parameterValues.find(parameterHandleTargetID);
        auto itNumberOfMissiles = parameterValues.find(parameterHandleNumberOfMissilesFired);
        if (itShooterTeam != parameterValues.end()
        && itTargetID != parameterValues.end()
        && itNumberOfMissiles != parameterValues.end()) {
            rti1516e::HLAunicodeString tempString;
            tempString.decode(itShooterTeam->second);
            std::wstring shooterTeam = tempString.get();
            tempString.decode(itTargetID->second);
            std::wstring targetID = tempString.get();
            rti1516e::HLAinteger32BE tempInt;
            tempInt.decode(itNumberOfMissiles->second);
            int numberOfMissilesFired = tempInt.get();
            
            auto& targetVector = getTargetShipVector(teamStatus, shooterTeam);

            std::wstring tempTeam = teamStatus == ShipTeam::BLUE ? L"Blue" : L"Red";
            logWmessage = L"[LOCK TARGET VALIDATION] My team: " + tempTeam + L", Shooter team: " + shooterTeam
                + L", Target team: " + targetVector[0]->shipTeam + L"\n";
            wstringToLog(logWmessage, teamStatus);
            
            applyMissileLock(targetVector, tempTeam, targetID, numberOfMissilesFired);
        } else {
            std::wcout << L"[ERROR] Missing parameters in FireMissile interaction" << std::endl;
        }
    }
    else if (interactionClassHandle == interactionClassConfirmRedHandshake || 
             interactionClassHandle == interactionClassConfirmBlueHandshake) {
        std::wcout << L"[DEBUG] ConfirmMissile interaction received" << std::endl;
        auto itParamShooterID = parameterValues.find(parameterHandleConfirmShooterID);
        auto itParamMissileNumberOfMissiles = parameterValues.find(parameterHandleConfirmMissileAmountFired);
        auto itParamTargetID = parameterValues.find(parameterHandleConfirmTargetID);
        auto itParamAllowFire = parameterValues.find(parameterHandleConfirmAllowFire);


        if (itParamShooterID != parameterValues.end() 
        && itParamMissileNumberOfMissiles != parameterValues.end()
        && itParamTargetID != parameterValues.end()
        && itParamAllowFire != parameterValues.end()) {
            rti1516e::HLAunicodeString tempString;
            rti1516e::HLAinteger32BE tempInt;
            rti1516e::HLAboolean tempBool;
            tempBool.decode(itParamAllowFire->second);
            if (tempBool.get() == false) {
                std::wcout << L"[DEBUG] ConfirmHancshake received, but allow fire is false." << std::endl;
                return;
            }
            tempString.decode(itParamShooterID->second);
            std::wstring shooterID = tempString.get();
            tempInt.decode(itParamMissileNumberOfMissiles->second);
            int missileAmount = tempInt.get();
            tempString.decode(itParamTargetID->second);
            std::wstring targetID = tempString.get();

            // Check if the ship is in the own ships vector
            for (auto ownShip : ownShipsVector) {
                if (ownShip->shipName == shooterID) {
                    ownShip->shipNumberOfMissiles -= missileAmount;
                    std::wcout << L"[INFO] ConfirmHancshake received. Found own ship: " << ownShip->shipName << L". Looking for target ship... ";
                    if (teamStatus == ShipTeam::BLUE) {
                        for (auto enemy : redShipsVector) {
                            if (enemy->shipName == targetID) {
                                enemy->currentMissilesLocking += missileAmount;
                                std::wcout << L"Successfully found enemy ship: " << enemy->shipName << std::endl;
                                fireOrders.emplace_back(ownShip, enemy, missileAmount);

                                logWmessage = L"[CONFIRMHANDSHAKE] " + ownShip->shipName + L" fired " + std::to_wstring(missileAmount) + L" missiles at " 
                                    + enemy->shipName;
                                wstringToLog(logWmessage, teamStatus);

                                if (enemy->currentMissilesLocking < 0) {
                                    std::wcout << L"[MAJOR ERROR] Current missiles locking is negative: " << enemy->currentMissilesLocking << std::endl;
                                    logWmessage = L"[MAJOR ERROR] Current missiles locking is negative: " + std::to_wstring(enemy->currentMissilesLocking);
                                    wstringToLog(logWmessage, teamStatus);
                                }

                                return;
                            }
                        }
                    } else if (teamStatus == ShipTeam::RED) {
                        for (auto enemy : blueShipsVector) {
                            if (enemy->shipName == targetID) {
                                enemy->currentMissilesLocking += missileAmount;
                                std::wcout << L"Successfully found enemy ship: " << enemy->shipName << std::endl;
                                fireOrders.emplace_back(ownShip, enemy, missileAmount);

                                logWmessage = L"[CONFIRMHANDSHAKE] " + ownShip->shipName + L" fired " + std::to_wstring(missileAmount) + L" missiles at " 
                                    + enemy->shipName;
                                wstringToLog(logWmessage, teamStatus);

                                if (enemy->currentMissilesLocking < 0) {
                                    std::wcout << L"[MAJOR ERROR] Current missiles locking is negative: " << enemy->currentMissilesLocking << std::endl;
                                    logWmessage = L"[MAJOR ERROR] Current missiles locking is negative: " + std::to_wstring(enemy->currentMissilesLocking);
                                    wstringToLog(logWmessage, teamStatus);
                                }

                                return;
                            }
                        }
                    }
                    std::wcout << L" Failed to find enemy ship: " << targetID << std::endl;
                    return;
                }
            }
            std::wcout << L"[DEBUG] ConfirmHancshake received. Did not find ship in ownVector: " << shooterID << std::endl;
        }
    }
}

// RO (Receive Order)
// This function is called when an interaction is received without a time stamp
void MyShipFederateAmbassador::receiveInteraction(
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    rti1516e::SupplementalReceiveInfo receiveInfo) {
    std::wcout << L"[DEBUG] Recieve interaction called without time" << std::endl;

    if (interactionClassHandle == setupSimulationHandle) {
        auto itBlueShips = parameterValues.find(blueShips);
        auto itRedShips = parameterValues.find(redShips);
        auto itTimeScaleFactor = parameterValues.find(timeScaleFactor);

        if (itBlueShips == parameterValues.end() 
        || itRedShips == parameterValues.end() 
        || itTimeScaleFactor == parameterValues.end()) {
            std::wcerr << L"Missing parameters in setup simulation interaction" << std::endl;
            return;
        }

        rti1516e::HLAinteger32BE paramValueBlueShips;
        paramValueBlueShips.decode(itBlueShips->second);
        std::wcout << L": Blue ships: " << paramValueBlueShips.get() << std::endl;
        std::wcout << federateName << std::endl;
        if (federateName.find(L"BlueShipFederate") ==  0) {
            std::wcout << L"Creating blue ships" << std::endl;
            createNewShips(paramValueBlueShips.get());
        }

        rti1516e::HLAinteger32BE paramValueRedShips;
        paramValueRedShips.decode(itRedShips->second);
        std::wcout << std::endl << L": Red ships: " << paramValueRedShips.get() << std::endl;
        std::wcout << federateName << std::endl;
        if (federateName.find(L"RedShipFederate") == 0) {
            std::wcout << L"Creating red ships" << std::endl;
            createNewShips(paramValueRedShips.get());
        }

        rti1516e::HLAfloat64BE paramValueTimeScaleFactor;
        paramValueTimeScaleFactor.decode(itTimeScaleFactor->second);
        std::wcout << std::endl << L": Time scale factor: " << paramValueTimeScaleFactor.get() << std::endl;
        timeScale = paramValueTimeScaleFactor.get();
    }
}

void MyShipFederateAmbassador::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag) 
{
    if (label == L"InitialSync") {
        std::wcout << L"[INFO - SyncPoint] Federate received synchronization announcement: InitialSync." << std::endl;
        syncLabel = label;
    }
    if (label == L"SimulationSetupComplete") {
        std::wcout << L"[INFO - SyncPoint] Federate synchronized at SimulationSetupComplete." << std::endl;
        syncLabel = label;
    }
    if (label.find(L"BlueShipFederate") == 0) {
        std::wcout << L"Federate synchronized at BlueTeamSync." << std::endl;
        blueSyncLabel = L"BlueShipFederate";
    }
    if (label.find(L"RedShipFederate") == 0) {
        std::wcout << L"Federate synchronized at RedTeamSync." << std::endl;
        redSyncLabel = L"RedShipFederate";
    }
    if (label == L"MissileReady") {
        std::wcout << L"[INFO - SyncPoint] Federate synchronized at MissileReady." << std::endl;
        syncLabel = label;
    }
    if (label == L"RedShipReady") {
        std::wcout << L"[INFO - SyncPoint] Federate synchronized at RedShipReady." << std::endl;
        syncLabel = label;
    }
    if (label == L"BlueShipReady") {
        std::wcout << L"[INFO - SyncPoint] Federate synchronized at BlueShipReady." << std::endl;
        syncLabel = label;
    }
    if (label == L"EveryoneReady") {
        std::wcout << L"[INFO - SyncPoint] Federate synchronized at EveryoneReady." << std::endl;
        syncLabel = label;
    }
    if (label == L"ReadyToExit") {
        std::wcout << L"[INFO - SyncPoint] Federate synchronized at ReadyToExit." << std::endl;
        syncLabel = label;
    }
    if (label == L"RedShipEmpty") {
        std::wcout << L"[INFO - SyncPoint] Federate synchronized at RedShipEmpty." << std::endl;
        syncLabel = label;
    }
    if (label == L"BlueShipEmpty") {
        std::wcout << L"[INFO - SyncPoint] Federate synchronized at BlueShipEmpty." << std::endl;
        syncLabel = label;
    }
}

bool MyShipFederateAmbassador::shipHandshake(std::vector<Ship*>& friendlyVector, std::vector<Ship*>& enemyVector, const std::wstring& shootingShipName, const std::wstring& shootingShipTeam, const std::wstring& targetShipName, int& missileAmount, const int& orderID) {
    std::wcout << L"[DEBUG] Ship handshake called" << std::endl;

    auto itShootingShip = std::find_if(friendlyVector.begin(), friendlyVector.end(), [&](Ship* ship) {
        return ship->shipName == shootingShipName;
    });

    auto itTargetShip = std::find_if(enemyVector.begin(), enemyVector.end(), [&](Ship* ship) {
        return ship->shipName == targetShipName;
    });

    if (itShootingShip != friendlyVector.end() && itTargetShip != enemyVector.end()) {
        Ship* shootingShip = *itShootingShip;
        Ship* targetShip = *itTargetShip;

        if (shootingShip->shipNumberOfMissiles >= missileAmount && targetShip->currentMissilesLocking < targetShip->maxMissilesLocking) {
            if (targetShip->currentMissilesLocking + missileAmount > targetShip->maxMissilesLocking) {
                missileAmount = targetShip->maxMissilesLocking - targetShip->currentMissilesLocking;
                if (missileAmount <= 0) {
                    std::wcerr << L"[ERROR] No missiles can be locked on target ship. missileAmount = " << missileAmount << std::endl;
                    return false;
                }
            }
            shootingShip->shipNumberOfMissiles -= missileAmount;
            targetShip->currentMissilesLocking += missileAmount;

            logWmessage = L"[SHIPHANDSHAKE] Ship: " + shootingShip->shipName + L" locked " + targetShip->shipName + L" with " + std::to_wstring(missileAmount) + L" missiles. Current missiles locking: "
                + std::to_wstring(targetShip->currentMissilesLocking) + L"/" + std::to_wstring(targetShip->maxMissilesLocking) + L". orderID: " + std::to_wstring(orderID) + L"\n";
            wstringToLog(logWmessage, teamStatus);
            return true;
        }
        std::wcout << L"[ERROR] Not enough missiles or target ship already locked." << std::endl;
        return false;
    } else {
        std::wstring errorMessage = L"[ERROR] ";
        if (itShootingShip == friendlyVector.end()) {
            errorMessage += L"Shooting ship not found: " + shootingShipName + L". ";
        }
        if (itTargetShip == enemyVector.end()) {
            errorMessage += L"Target ship not found: " + targetShipName + L". ";
        }
        std::wcerr << errorMessage << std::endl;
        return false;
    }
}

void MyShipFederateAmbassador::applyMissileLock(std::vector<Ship*>& shipVector, const std::wstring myTeam, const std::wstring& targetID, int numberOfMissilesFired) {
    for (auto ship : shipVector) {
        if (ship->shipName == targetID) {
            if ((ship->currentMissilesLocking + numberOfMissilesFired) > ship->maxMissilesLocking) {
                logWmessage = L"[MISSILE LOCKING-ERROR] My team: " + myTeam + L" found too many missiles locking on target " + targetID + L". Max: " + std::to_wstring(ship->maxMissilesLocking)
                + L", current: " + std::to_wstring(ship->currentMissilesLocking) + L"\n";
                wstringToLog(logWmessage, teamStatus);
                return;
            }
            ship->currentMissilesLocking += numberOfMissilesFired;
            logWmessage = L"[UPDATE MISSILE LOCKING] My team: " + myTeam + L" Found targetID: " + targetID + L", current missiles locking: "
            + std::to_wstring(ship->currentMissilesLocking) + L"/" + std::to_wstring(ship->maxMissilesLocking) + L"\n";
            wstringToLog(logWmessage, teamStatus);
            return;
        }
    }
    std::wcout << L"[ERROR] TargetID not found in ship vector: " << targetID << std::endl;
}

std::vector<Ship*>& MyShipFederateAmbassador::getTargetShipVector(ShipTeam teamStatus, const std::wstring& shooterTeam) {
    if (teamStatus == ShipTeam::BLUE)
        return (shooterTeam == L"Blue") ? redShipsVector : ownShipsVector;
    else
        return (shooterTeam == L"Red") ? blueShipsVector : ownShipsVector;
}

void MyShipFederateAmbassador::timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isRegulating = true;
    std::wcout << L"Time Regulation Enabled: " << theFederateTime << std::endl;
}

void MyShipFederateAmbassador::timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isConstrained = true;
    std::wcout << L"Time Constrained Enabled: " << theFederateTime << std::endl;
}

void MyShipFederateAmbassador::timeAdvanceGrant(const rti1516e::LogicalTime& theTime) {
    //std::wcout << L"[DEBUG] Time Advance Grant received: "
    //           << dynamic_cast<const rti1516e::HLAfloat64Time&>(theTime).getTime() << std::endl;

    isAdvancing = false;  // Allow simulation loop to continue
}

void MyShipFederateAmbassador::createNewShips(int amountOfShips) {
    try {
        if (amountOfShips <= 0) {
            std::wcerr << L"[ERROR] Invalid number of ships to create: " << amountOfShips << std::endl;
            return;
        }

        std::wcout << L"[INFO] Creating " << amountOfShips << L" ship(s)" << std::endl;

        int maxShipsPerRow = getOptimalShipsPerRow(amountOfShips);
        std::pair<double, double> baseShipPosition = {20.43829000, 15.62534000};

        for (int i = 0; i < amountOfShips; ++i) {
            int row = i / maxShipsPerRow;
            int column = i % maxShipsPerRow;

            rti1516e::ObjectInstanceHandle objectInstanceHandle = _rtiambassador->registerObjectInstance(objectClassHandleShip);

            // Add Ship to shipMap
            shipMap[objectInstanceHandle] = Ship(objectInstanceHandle);
            Ship* ship = &shipMap[objectInstanceHandle];

            // Assign ship name and team
            ownShipsVector.push_back(ship);
            if (federateName.find(L"BlueShipFederate") == 0) {
                ship->shipName = L"BlueShip_" + std::to_wstring(getpid()) + L"_" + std::to_wstring(shipCounter++);
                ship->shipTeam = L"Blue";
            } else if (federateName.find(L"RedShipFederate") == 0) {
                ship->shipName = L"RedShip_" + std::to_wstring(getpid()) + L"_" + std::to_wstring(shipCounter++);
                ship->shipTeam = L"Red";
            } else {
                std::wcerr << L"[ERROR] Unknown federate name: " << federateName << std::endl;
                continue;
            }

            // Assign position
            ship->shipPosition = generateDoubleShipPosition(baseShipPosition, ship->shipTeam, row, column);

            // Read any additional config (missiles etc.)
            readJsonFile(ship);

            // Construct position record
            rti1516e::HLAfixedRecord shipPositionRecord;
            shipPositionRecord.appendElement(rti1516e::HLAfloat64BE(ship->shipPosition.first));
            shipPositionRecord.appendElement(rti1516e::HLAfloat64BE(ship->shipPosition.second));

            std::wcout << L"[INFO] Registered ship: " << ship->shipName << std::endl;

            // Construct attribute map
            rti1516e::AttributeHandleValueMap attributes;
            attributes[attributeHandleShipFederateName] = rti1516e::HLAunicodeString(ship->shipName).encode();
            attributes[attributeHandleShipPosition] = shipPositionRecord.encode();
            attributes[attributeHandleShipSpeed] = rti1516e::HLAfloat64BE(getSpeed(10, 10, 25)).encode();
            attributes[attributeHandleNumberOfMissiles] = rti1516e::HLAinteger32BE(ship->shipNumberOfMissiles).encode();
            attributes[attributeHandleMaxMissilesLockingThisShip] = rti1516e::HLAinteger32BE(ship->maxMissilesLocking).encode();
            attributes[attributeHandleCurrentMissilesLockingThisShip] = rti1516e::HLAinteger32BE(ship->currentMissilesLocking).encode();
            // TODO: Add numberOfCanons if needed

            _rtiambassador->updateAttributeValues(objectInstanceHandle, attributes, rti1516e::VariableLengthData());
        }

        createShips = true;

    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[EXCEPTION] " << e.what() << std::endl;
    }
}

void MyShipFederateAmbassador::readJsonFile(Ship* ship) {
    JsonParser parser(JSON_PARSER_PATH);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1, 3);

    if (!parser.isFileOpen()) return;

    int i = dis(gen);
    parser.parseShipConfig("Ship" + std::to_string(i));

    // Determine which vector to update based on the federate name
    //Ship* ship = nullptr;
    //ship = ownShipsVector.back(); // Default to the last ship in ownShipsVector

    if (!ship) {
        std::wcerr << L"[ERROR] No ship found to update." << std::endl;
        return;
    }

    ship->shipSize = parser.getShipSize();
    ship->shipNumberOfMissiles = parser.getNumberOfMissiles();
    ship->shipNumberOfCanons = parser.getNumberOfCanons();

    std::wcout << std::endl
               << L"Ship size: " << ship->shipSize << L" for ship " << ship->shipName << std::endl
               << L"Number of missiles: " << ship->shipNumberOfMissiles << L" for ship " << ship->shipName << std::endl
               << L"Number of canons: " << ship->shipNumberOfCanons << L" for ship " << ship->shipName << std::endl;
}

void MyShipFederateAmbassador::addShip(rti1516e::ObjectInstanceHandle objectHandle) {
    // Create a new Ship object and add it to the shipMap (which owns the Ship)
    Ship newShip(objectHandle);
    shipMap[objectHandle] = std::move(newShip); // Emplace into map (copy or move)

    // Get a pointer to the stored Ship and add it to blueShipsVector
    Ship* shipPtr = &shipMap[objectHandle];
    blueShipsVector.push_back(shipPtr);
}


void MyShipFederateAmbassador::setFederateName(const std::wstring& name) {
    federateName = name;
}
std::wstring MyShipFederateAmbassador::getFederateName() const{
    return federateName;
}

ShipTeam MyShipFederateAmbassador::getTeamStatus() const {
    return teamStatus;
}
void MyShipFederateAmbassador::setTeamStatus(ShipTeam newStatus) {
    teamStatus = newStatus;
}

// Getter and setter for Object Class Ship and its attributes
rti1516e::ObjectClassHandle MyShipFederateAmbassador::getObjectClassHandleShip() const {
    return objectClassHandleShip;
}
void MyShipFederateAmbassador::setObjectClassHandleShip(rti1516e::ObjectClassHandle handle) {
    objectClassHandleShip = handle;
}

rti1516e::AttributeHandle MyShipFederateAmbassador::getAttributeHandleShipFederateName() const {
    return attributeHandleShipFederateName;
}
void MyShipFederateAmbassador::setAttributeHandleShipFederateName(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipFederateName = handle;
}

rti1516e::AttributeHandle MyShipFederateAmbassador::getAttributeHandleShipTeam() const {
    return attributeHandleShipTeam;
}
void MyShipFederateAmbassador::setAttributeHandleShipTeam(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipTeam = handle;
}

rti1516e::AttributeHandle MyShipFederateAmbassador::getAttributeHandleShipPosition() const {
    return attributeHandleShipPosition;
}
void MyShipFederateAmbassador::setAttributeHandleShipPosition(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipPosition = handle;
}

rti1516e::AttributeHandle MyShipFederateAmbassador::getAttributeHandleShipSpeed() const {
    return attributeHandleShipSpeed;
}
void MyShipFederateAmbassador::setAttributeHandleShipSpeed(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipSpeed = handle;
}

rti1516e::AttributeHandle MyShipFederateAmbassador::getAttributeHandleNumberOfMissiles() const {
    return attributeHandleNumberOfMissiles;
}
void MyShipFederateAmbassador::setAttributeHandleNumberOfMissiles(const rti1516e::AttributeHandle& handle) {
    attributeHandleNumberOfMissiles = handle;
}

rti1516e::AttributeHandle MyShipFederateAmbassador::getAttributeHandleMaxMissilesLockingThisShip() const {
    return attributeHandleMaxMissilesLockingThisShip;
}
void MyShipFederateAmbassador::setAttributeHandleMaxMissilesLockingThisShip(const rti1516e::AttributeHandle& handle) {
    attributeHandleMaxMissilesLockingThisShip = handle;
}

rti1516e::AttributeHandle MyShipFederateAmbassador::getAttributeHandleCurrentMissilesLockingThisShip() const {
    return attributeHandleCurrentMissilesLockingThisShip;
}
void MyShipFederateAmbassador::setAttributeHandleCurrentMissilesLockingThisShip(const rti1516e::AttributeHandle& handle) {
    attributeHandleCurrentMissilesLockingThisShip = handle;
}

// Getter and setter for interaction class SetupSimulation and its parameters
rti1516e::InteractionClassHandle MyShipFederateAmbassador::getSetupSimulationHandle() const {
    return setupSimulationHandle;
}
void MyShipFederateAmbassador::setSetupSimulationHandle(const rti1516e::InteractionClassHandle& handle) {
    setupSimulationHandle = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getBlueShipsParam() const {
    return blueShips;
}
void MyShipFederateAmbassador::setBlueShipsParam(const rti1516e::ParameterHandle& handle) {
    blueShips = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getRedShipsParam() const {
    return redShips;
}
void MyShipFederateAmbassador::setRedShipsParam(const rti1516e::ParameterHandle& handle) {
    redShips = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getTimeScaleFactorParam() const {
    return timeScaleFactor;
}
void MyShipFederateAmbassador::setTimeScaleFactorParam(const rti1516e::ParameterHandle& handle) {
    timeScaleFactor = handle;
}

// Getters and setters for ship attributes

std::wstring MyShipFederateAmbassador::getEnemyShipFederateName() const {
    return enemyShipFederateName;
}
void MyShipFederateAmbassador::setEnemyShipFederateName(const std::wstring& name) {
    enemyShipFederateName = name;
}

std::pair<double, double> MyShipFederateAmbassador::getEnemyShipPosition() const {
    return enemyShipPosition;
}
void MyShipFederateAmbassador::setEnemyShipPosition(const std::pair<double, double>& position) {
    enemyShipPosition = position;
}

double MyShipFederateAmbassador::getDistanceBetweenShips() const {
    return distanceBetweenShips;
}
void MyShipFederateAmbassador::setDistanceBetweenShips(const double& distance) {
    distanceBetweenShips = distance;
}

double MyShipFederateAmbassador::getBearing() const {
    return bearing;
}
void MyShipFederateAmbassador::setBearing(const double& input) {
    bearing = input;
}

bool MyShipFederateAmbassador::getIsFiring() const {
    return isFiring;
}
void MyShipFederateAmbassador::setIsFiring(const bool& firing) {
    isFiring = firing;
}

std::wstring MyShipFederateAmbassador::getSyncLabel() const {
    return syncLabel;
}
std::wstring MyShipFederateAmbassador::getRedSyncLabel() const {
    return redSyncLabel;
}
std::wstring MyShipFederateAmbassador::getBlueSyncLabel() const {
    return blueSyncLabel;
}

// Getter and setter for createShipsSyncPoint
bool MyShipFederateAmbassador::getCreateShips() const {
    return createShips;
}

// Getter and setter functions for interaction class FireMissile
rti1516e::InteractionClassHandle MyShipFederateAmbassador::getInteractionClassFireMissile() const {
    return interactionClassFireMissile;
}
void MyShipFederateAmbassador::setInteractionClassFireMissile(const rti1516e::InteractionClassHandle& handle) {
    interactionClassFireMissile = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamShooterID() const {
    return parameterHandleShooterID;
}
void MyShipFederateAmbassador::setParamShooterID(const rti1516e::ParameterHandle& handle) {
    parameterHandleShooterID = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamTargetID() const {
    return parameterHandleTargetID;
}
void MyShipFederateAmbassador::setParamTargetID(const rti1516e::ParameterHandle& handle) {
    parameterHandleTargetID = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamMissileTeam() const {
    return parameterHandleMissileTeam;
}
void MyShipFederateAmbassador::setParamMissileTeam(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileTeam = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamMissileStartPosition() const {
    return parameterHandleMissileStartPosition;
}
void MyShipFederateAmbassador::setParamMissileStartPosition(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileStartPosition = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamMissileTargetPosition() const {
    return parameterHandleMissileTargetPosition;
}
void MyShipFederateAmbassador::setParamMissileTargetPosition(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileTargetPosition = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamNumberOfMissilesFired() const {
    return parameterHandleNumberOfMissilesFired;
}
void MyShipFederateAmbassador::setParamNumberOfMissilesFired(const rti1516e::ParameterHandle& handle) {
    parameterHandleNumberOfMissilesFired = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamMissileSpeed() const {
    return parameterHandleMissileSpeed;
}
void MyShipFederateAmbassador::setParamMissileSpeed(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileSpeed = handle;
}

// Getter and setter functions for interaction class initiate handshake
rti1516e::InteractionClassHandle MyShipFederateAmbassador::getInteractionClassInitiateRedHandshake() const {
    return interactionClassInitiateRedHandshake;
}
void MyShipFederateAmbassador::setInteractionClassInitiateRedHandshake(const rti1516e::InteractionClassHandle& handle) {
    interactionClassInitiateRedHandshake = handle;
}

rti1516e::InteractionClassHandle MyShipFederateAmbassador::getInteractionClassInitiateBlueHandshake() const {
    return interactionClassInitiateBlueHandshake;
}
void MyShipFederateAmbassador::setInteractionClassInitiateBlueHandshake(const rti1516e::InteractionClassHandle& handle) {
    interactionClassInitiateBlueHandshake = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamInitiateShooterID() const {
    return parameterHandleInitiateShooterID;
}
void MyShipFederateAmbassador::setParamInitiateShooterID(const rti1516e::ParameterHandle& handle) {
    parameterHandleInitiateShooterID = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamInitiateTargetID() const {
    return parameterHandleInitiateTargetID;
}
void MyShipFederateAmbassador::setParamInitiateTargetID(const rti1516e::ParameterHandle& handle) {
    parameterHandleInitiateTargetID = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamInitiateMissileAmountFired() const {
    return parameterHandleInitiateMissileAmountFired;
}
void MyShipFederateAmbassador::setParamInitiateMissileAmountFired(const rti1516e::ParameterHandle& handle) {
    parameterHandleInitiateMissileAmountFired = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamInitiateMaxMissilesRequired() const {
    return parameterHandleMaxMissilesRequired;
}
void MyShipFederateAmbassador::setParamInitiateMaxMissilesRequired(const rti1516e::ParameterHandle& handle) {
    parameterHandleMaxMissilesRequired = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamInitiateMissilesCurrentlyTargeting() const {
    return parameterHandleCurrentlyTargeting;
}
void MyShipFederateAmbassador::setParamInitiateMissilesCurrentlyTargeting(const rti1516e::ParameterHandle& handle) {
    parameterHandleCurrentlyTargeting = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamInitiateDistanceToTarget() const {
    return parameterHandleDistanceToTarget;
}
void MyShipFederateAmbassador::setParamInitiateDistanceToTarget(const rti1516e::ParameterHandle& handle) {
    parameterHandleDistanceToTarget = handle;
}

// Getter and setter functions for interaction class ConfirmHandshake
rti1516e::InteractionClassHandle MyShipFederateAmbassador::getInteractionClassConfirmRedHandshake() const {
    return interactionClassConfirmRedHandshake;
}
void MyShipFederateAmbassador::setInteractionClassConfirmRedHandshake(const rti1516e::InteractionClassHandle& handle) {
    interactionClassConfirmRedHandshake = handle;
}

rti1516e::InteractionClassHandle MyShipFederateAmbassador::getInteractionClassConfirmBlueHandshake() const {
    return interactionClassConfirmBlueHandshake;
}
void MyShipFederateAmbassador::setInteractionClassConfirmBlueHandshake(const rti1516e::InteractionClassHandle& handle) {
    interactionClassConfirmBlueHandshake = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamConfirmShooterID() const {
    return parameterHandleConfirmShooterID;
}
void MyShipFederateAmbassador::setParamConfirmShooterID(const rti1516e::ParameterHandle& handle) {
    parameterHandleConfirmShooterID = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamConfirmTargetID() const {
    return parameterHandleConfirmTargetID;
}
void MyShipFederateAmbassador::setParamConfirmTargetID(const rti1516e::ParameterHandle& handle) {
    parameterHandleConfirmTargetID = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamConfirmMissileAmountFired() const {
    return parameterHandleConfirmMissileAmountFired;
}
void MyShipFederateAmbassador::setParamConfirmMissileAmountFired(const rti1516e::ParameterHandle& handle) {
    parameterHandleConfirmMissileAmountFired = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamConfirmAllowFire() const {
    return parameterHandleConfirmAllowFire;
}
void MyShipFederateAmbassador::setParamConfirmAllowFire(const rti1516e::ParameterHandle& handle) {
    parameterHandleConfirmAllowFire = handle;
}

// Getter and setter functions for interaction class targetHit
rti1516e::InteractionClassHandle MyShipFederateAmbassador::getInteractionClassTargetHit() const {
    return interactionClassTargetHit;
}
void MyShipFederateAmbassador::setInteractionClassTargetHit(const rti1516e::InteractionClassHandle& handle) {
    interactionClassTargetHit = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamTargetHitID() const {
    return parameterHandleTargetHitID;
}
void MyShipFederateAmbassador::setParamTargetHitID(const rti1516e::ParameterHandle& handle) {
    parameterHandleTargetHitID = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamTargetHitTeam() const {
    return parameterHandleTargetHitTeam;
}
void MyShipFederateAmbassador::setParamTargetHitTeam(const rti1516e::ParameterHandle& handle) {
    parameterHandleTargetHitTeam = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamTargetHitPosition() const {
    return parameterHandleTargetHitPosition;
}
void MyShipFederateAmbassador::setParamTargetHitPosition(const rti1516e::ParameterHandle& handle) {
    parameterHandleTargetHitPosition = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamTargetHitDestroyed() const {
    return parameterHandleTargetHitDestroyed;
}
void MyShipFederateAmbassador::setParamTargetHitDestroyed(const rti1516e::ParameterHandle& handle) {
    parameterHandleTargetHitDestroyed = handle;
}

// Getter and setter for time management
bool MyShipFederateAmbassador::getIsRegulating() const {
    return isRegulating;
}
bool MyShipFederateAmbassador::getIsConstrained() const {
    return isConstrained;
}
bool MyShipFederateAmbassador::getIsAdvancing() const {
    return isAdvancing;
}
void MyShipFederateAmbassador::setIsAdvancing(bool advancing) {
    isAdvancing = advancing;
}

// Getter and setter for shipMap
std::unordered_map<rti1516e::ObjectInstanceHandle, Ship>& MyShipFederateAmbassador::getShipMap() {
    return shipMap;
}
std::vector<Ship*>& MyShipFederateAmbassador::getBlueShips() {
    return blueShipsVector;
}
std::vector<Ship*>& MyShipFederateAmbassador::getRedShips() {
    return redShipsVector;
}
std::vector<Ship*>& MyShipFederateAmbassador::getOwnShips() {
    return ownShipsVector;
}

const std::map<Ship*, Ship*>& MyShipFederateAmbassador::getClosestEnemyShip() {
    return closestEnemyship;
}
void MyShipFederateAmbassador::setClosestEnemyShip(Ship* ship, Ship* target) {
    closestEnemyship[ship] = target;
}
void MyShipFederateAmbassador::clearClosestEnemyShip() {
    closestEnemyship.clear();
}

const std::vector<FireOrder>& MyShipFederateAmbassador::getFireOrders() const {
    return fireOrders;
}

void MyShipFederateAmbassador::clearFireOrders() {
    fireOrders.clear();
}