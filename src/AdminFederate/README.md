# AdminFederate

**AdminFederate** is a C++ program built using the IEEE HLA (High-Level Architecture) standard. It acts as the central administrative unit in a federated simulation, responsible for initializing simulation parameters, enforcing synchronization, and coordinating communication between federates and external Python components.

## Purpose

- Sets up key simulation parameters (e.g., ship count, time factor).
- Enforces a synchronization point to delay simulation progress until setup is complete.
- Maintains communication with external Python scripts (such as `ReceiveData.py`) via sockets, due to OpenRTI’s lack of native Python support.
- Monitors simulation state and coordinates shutdown when appropriate.

## File Overview

- **structAdmin.h**  
  Defines enums and structs for data management within AdminFederate.

- **AdminFederateFunctions.h**  
  Contains utility functions for setup and operation.

- **AdminFederateAmbassador.cpp/.h**  
  Handles RTI (Run-Time Infrastructure) interactions:
  - Receiving interactions (`receiveInteraction` with and without LogicalTime)
  - Managing synchronization and time progression (`announceSynchronizationPoint`, `timeRegulationEnabled`, `timeConstrainedEnabled`, `timeAdvanceGrant`)
  - Declares and initializes various `rti1516e::InteractionClassHandle` and `ParameterHandle` objects.

- **AdminFederate.cpp/.h**  
  Core logic of AdminFederate:
  - Creating and connecting to the RTI (`createRTIambassador`, `connectToRTI`, `initializeFederation`, `joinFederation`)
  - Blocking simulation progress at a sync point until admin setup (via console) is complete.
  - Communicating with `ReceiveData.py` via socket to maintain simulation lifecycle awareness.

## Runtime Behavior

### Initialization Phase

- Connects to the RTI and sets up the federation.
- Prompts the user to input parameters (e.g., ship count, time factor).
- Holds other federates at a synchronization point until setup is finalized.

### Execution Phase

- Listens for simulation data in the RTI.
- Receives signals via socket from `ReceiveData.py` to confirm ongoing simulation activity.
- Shuts down when no more data is received (heartbeat lost).

---

**Note:**  
AdminFederate does not directly simulate ships or missiles. Its main role is to coordinate setup, synchronization, and lifecycle management for the entire federation.