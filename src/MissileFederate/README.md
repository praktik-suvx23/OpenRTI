# MissileFederate

**MissileFederate** is a C++ federate in an HLA-based simulation, designed to model the lifecycle and behavior of a single missile. Built using the IEEE HLA (High-Level Architecture) standard (rti1516e), multiple MissileFederate instances run concurrently to simulate real-time missile launches and impacts.

---

## Purpose

- Simulates the full lifecycle of a missile: launch, flight, target acquisition, and impact.
- Calculates trajectory, speed, altitude, and dynamically tracks and switches targets if needed.
- Interacts with ships and other federates via RTI.
- Logs missile creation, flight, and hit events for analysis.
- Coordinates with other missiles to prevent over-targeting (overkill) on a single ship.
- **Note:** MissileFederate instances are launched by the MissileCreator component in response to ship fire events.

---

## Targeting Behavior

- Missiles initially fly toward their assigned target’s position at launch.
- When within ~1500 meters of the target, the missile checks for nearby available targets.
- If the current target is over-allocated (too many missiles assigned), the missile may switch to a new target based on proximity and availability.
- Coordination logic ensures no more than a set number of missiles are assigned to any single ship.

---

## File Overview

### structMissile.h
- Defines core data structures for missile state, parameters, and tracking.

### MissileFederateAmbassador.cpp/.h
- Handles RTI interactions:
  - `announceSynchronizationPoint`: manages synchronization points during startup.
  - `discoverObjectInstance`: detects new objects (e.g., ships) in the federation.
  - `reflectAttributeValues`: receives updated attributes from subscribed object classes.
  - `receiveInteraction`: receives interactions and simulation time.
- Declares and initializes RTI handles:
  - `InteractionClassHandle`, `ParameterHandle`
  - `ObjectClassHandle`, `AttributeHandle`
  - Other RTI-related variables.

### MissileFederate.cpp/.h
- Main missile simulation logic:
  - RTI setup and connection (`createRTIAmbassador`, `connectToRTI`, `initializeFederation`, `joinFederation`)
  - Handle initialization (`initializeHandles`, `subscribeAttributes`, `publishAttributes`, `subscribeInteractions`, `publishInteractions`)
  - Main simulation loop:
    - Calculates flight path and target logic.
    - Detects when a missile reaches a target.
    - Sends interaction to the appropriate ship to register a hit.
    - Logs relevant missile data to text files.

---

## Runtime Behavior

### Initialization Phase

- Connects to the RTI and joins the federation.
- Initializes RTI handles and subscribes/publishes required attributes and interactions.
- Waits at a synchronization point until federation setup is complete.

### Execution Phase

- Begins missile trajectory toward the initial target.
- Monitors range and re-evaluates target if necessary.
- Sends interaction to the target ship upon impact.
- Logs data at creation and termination (hit).
- Shuts down when the missile reaches a target or its lifecycle ends.

---

## Notes

- Each MissileFederate represents one missile.
- Missiles are independent processes, forked dynamically based on firing events.
- Coordination logic prevents overkill on single targets by sharing targeting state across active missiles.
- Once launched, each MissileFederate operates autonomously and does not require external input beyond initial launch parameters.

---

## Logging

- `../log/finalData.txt`: Logs results such as missile ID, hit target ID, flight time, and simulation time.
- `../log/loggingMissile.txt`: Logs missile creation events, including