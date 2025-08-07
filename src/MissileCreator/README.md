# MissileCreator Federate

**MissileCreator** is a C++ federate built using the IEEE HLA (High-Level Architecture) standard. Its primary role is to act as an asynchronous missile launcher: it listens for fire requests from ship federates and dynamically spawns new missile instances during the simulation.

Unlike other federates, MissileCreator does **not** simulate missile behavior itself. Its sole purpose is to detect fire interactions and launch new processes that run MissileFederate instances via `./MissileHandler`.

---

## Purpose

- Listens for missile creation interactions from ShipFederate.
- Launches a new MissileFederate process (via `./MissileHandler`) for each missile requested.
- Ensures each missile runs in its own independent process.
- Converts and forwards RTI data necessary to initialize missiles.
- Synchronizes with the rest of the federation for coordinated startup and time management.

---

## File Overview

### MissileCreator.cpp / MissileCreator.h

- **Core application logic:**
  - RTI setup:
    - `createRTIAmbassador()`
    - `connectToRTI()`
    - `initializeFederation()`
    - `joinFederation()`
  - Handle and interaction setup:
    - `initializeHandles()`
    - `subscribeInteractions()`
  - **Main loop:**
    - Waits at the synchronization point until the simulation begins.
    - Delegates missile-launching behavior to the Ambassador.

### MissileCreatorFederateAmbassador.cpp / MissileCreatorFederateAmbassador.h

- Handles all RTI communication and contains the core missile-launching logic:
  - `announceSynchronizationPoint()` — Synchronizes with the federation before simulation starts.
  - `receiveInteraction()` — Detects fire interactions sent by ShipFederate.
  - `StartMissile()` — Core logic to create and launch new missile processes:
    - Forks a new process for each missile, separating it from the parent federate.
    - Prepares arguments (converts RTI parameters to usable C++ values).
    - Launches the missile using `execv()` to run `./MissileHandler`, which starts a new MissileFederate.

---

## Runtime Behavior

- MissileCreator runs quietly in the background during the simulation.
- Reacts only when a ship initiates a fire request.
- For every fire request received:
  - A new MissileFederate is launched in its own process, allowing multiple missiles to be simulated concurrently.
- Waits for simulation end by monitoring the synchronization point.

---

## Notes

- **MissileCreator does not simulate missiles** — this is entirely delegated to the MissileFederate instances it launches.
- This federate **must be running** for missiles to be spawned when ships fire.
- All inter-process communication and RTI setup for each missile happens inside MissileFederate after being launched by MissileCreator.

---

**Summary:**  
MissileCreator is the bridge between ship fire requests and missile simulation. It ensures that each missile is launched as a separate federate, enabling scalable and concurrent missile simulation within