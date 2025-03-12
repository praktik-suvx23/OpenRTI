# Robot FOM Documentation

This document provides an overview of the Robot Federation Object Model (FOM) defined in `robot.xml`. The FOM is structured for a Proof-of-Concept (POC) missile and ship simulation system built using the High-Level Architecture (HLA) standard. More specifically, it follows the IEEE 1516-2010 standard (also known as HLA Evolved) and is intended to be used with OpenRTI, an open-source Run-Time Infrastructure (RTI) implementation.

# What is a Federation Object Model (FOM)?

In HLA, a **FOM** defines the **data exchange rules** between different simulation components (called federates) participating in a distributed simulation (federation). It specifies the **objects, interactions, data types, and synchronization points** that federates use to communicate.

This FOM is designed to model a scenario where **ships deploy and control missiles** while interacting with other federates in the system.

## Model Identification
The metadata for this FOM is as follows:
*** TODO: Make this our own instead of using template & clean up ***

- **Name**: Robot FOM
- **Type**: FOM
- **Version**: 1.0
- **Modification Date**: 2025-02-06
- **Security Classification**: Unclassified
- **Purpose**: Tutorial FOM
- **Application Domain**: Engineering
- **Description**: FOM for the Getting Started with HLA Evolved kit.
- **Use Limitation**: None
- **Point of Contact**:
  - **Type**: Primary author
  - **Name**: John Doe
  - **Organization**: Example Technologies
  - **Telephone**: +1 123 456 7890
  - **Email**: info@example.com

## Timestamp
HLA requires a mechanism to track logical time across federates to ensure consistent event processing. This FOM uses:
- **Logical Time Implementation Name**: HLAfloat64Time

## Objects
### HLAobjectRoot
All object classes in HLA are derived from HLAobjectRoot, which serves as the base class. The following object classes are included in this FOM:
*** TODO: Add objects in use here with description. Remove unused objects ***

#### Ship

- **Sharing**: PublishSubscribe
- **Attributes**:
  - **ShipID** (*HLAunicodeString*): Unique identifier for each ship. Static.
  - **ShipTeam** (*HLAunicodeString*): Defines team alignment. Static.
  - **Position** (*PositionRec*): Current position of the ship. Updated conditionally when changed.
  - **Speed** (*HLAfloat32BE*): Speed of the ship. Updated conditionally when changed.
  - **Angle** (*HLAfloat32BE*): Orientation of the ship. Updated conditionally when changed.
  - **NumberOfRobots** (*HLAinteger32BE*): Number of robots on board. Static. *** TODO: Static? The number of robots should change when being used. Take a look at this ***

## Interactions
### HLAinteractionRoot
Interactions allow federates to exchange discrete events. Unlike object attributes, interactions **are instantaneous and do not persist.**
*** TODO: Add interactions in use here with description. Remove unused interactions ***

#### FireRobot
This interaction models the action of launching a robot from a ship.
*** TODO: This might change / get improved later ***

- **Sharing**: PublishSubscribe
- **Transportation**: HLAreliable (Ensures guaranteed delivery of the interaction)
- **Order**: Timestamp (Interactions are timestamped for time management)
- **Semantics**: Represents an event where a robot is deployed and assigned a target.
- **Parameters**:
  - **Fire** (*HLAinteger32BE*): Unique identifier of the robot being fired.
  - **Target** (*HLAunicodeString*): Unique identifier of the ship being targeted.
  - **ShooterPosition** (*PositionRec*): Position where the robot was deployed.

## Synchronizations
Synchronization points ensure federates reach a common state before continuing execution.
### InitialSync
This ensures all federates start from a consistent initial state.
- **Capability**: Achieve
- **Semantics**: Achieved when all federates are ready to proceed

## RTI Configuration Switches
These settings control RTI behavior and federation execution:

- **autoProvide**: true (Attributes are automatically updated when requested)
- **conveyRegionDesignatorSets**: false (Region-based filtering disabled)
- **conveyProducingFederate**: false (Federate origin of updates not conveyed)
- **attributeScopeAdvisory**: false (No advisory messages for attribute scope changes)
- **attributeRelevanceAdvisory**: false (No advisory messages for relevance changes)
- **objectClassRelevanceAdvisory**: false
- **interactionRelevanceAdvisory**: false
- **serviceReporting**: false (No reporting on RTI service usage)
- **exceptionReporting**: false (No reporting on RTI exceptions)
- **delaySubscriptionEvaluation**: false
- **automaticResignAction**: CancelThenDeleteThenDivest (Defines how a federate resigns from the federation)

## Data Types
*** TODO: Control that all of these are in use. ***
### Simple Data Types

- **FuelInt32** (*HLAinteger32BE*): Integer that describes a fuel volume.
- **AngleFloat64** (*HLAfloat64BE*): Decimal degree data type used for Lat/Long positioning.
- **ScaleFactorFloat32** (*HLAfloat32BE*): Ratio between two values.

### Enumerated Data Types

- **FuelTypeEnum32** (*HLAinteger32BE*):
  - **AviationGasoline**: 1
  - **JetFuel**: 2
  - **EthanolFlexibleFuel**: 3
  - **NaturalGas**: 4

### Fixed Record Data Types

- **PositionRec** (*HLAfixedRecord*): Position on Earth described as Lat/Long using WGS-84.
  - **Lat** (*AngleFloat64*): Latitude degree (-90 to 90)
  - **Long** (*AngleFloat64*): Longitude degree (-180 to 180)