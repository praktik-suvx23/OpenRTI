# ShipFederate Federate

The **ShipFederate** federate is a core component of this HLA-based simulation. It is responsible for creating, managing, and simulating ships during runtime. This federate handles ship creation, missile firing, damage simulation, and the lifecycle of each ship in the scenario.

ShipFederate interacts with the RTI to:
- Receive data from other federates
- Publish its own simulation data
- Maintain ship logic during simulation steps

---

## File Overview

### Ship.h
- Defines enums and structs for ship-specific data.
- Manages ship state, missile tracking, and related attributes.

### ShipFederateAmbassador.cpp/.h
- Acts as the bridge between ShipFederate and the RTI.
- **HLA RTI Callbacks:**
  - `discoverObjectInstance`
  - `reflectAttributeValues`
  - `receiveInteraction` (with and without LogicalTime)
  - `announceSynchronizationPoint`
  - `timeRegulationEnabled`
  - `timeConstrainedEnabled`
  - `timeAdvanceGrant`
- **Ship-specific logic:**
  - `createNewShips`: Instantiates ships as specified by AdminFederate.
  - `applyMissileLock`: Applies missile locks to target ships.
- **RTI Handle Management:**
  - Declares and initializes `InteractionClassHandle`, `ParameterHandle`, `ObjectInstanceHandle`, `AttributeHandle`, and other RTI variables.

### ShipFederate.cpp/.h
- Main logic and entry point for the ShipFederate federate.
- **Federation Setup:**
  - `createRTIAmbassador()`
  - `connectToRTI()`
  - `initializeFederation()`
  - `joinFederation()`
- **Handle Management:**
  - Initializes interaction and object handles (via ShipFederateAmbassador).
  - Publishes and subscribes to attributes and interactions.
- **Simulation Loop:**
  - Simulates ship behavior per time step.
  - Assigns missile launch orders to ships.
  - Handles missile impacts and removes destroyed ships.
  - Detects victory/loss state (exits if no enemy ships remain).

---

## Runtime Behavior

### Federation Initialization
- Connects to the RTI and joins the federation.
- Retrieves simulation configuration (e.g., number of ships) via AdminFederate interactions.

### Ship Management
- Creates ships and publishes their state to the RTI.
- Listens for enemy ship states and missile locks via RTI updates.

### Combat Simulation
- Launches missiles based on orders.
- Updates ship state as damage is received.
- Ends simulation for this federate once its ships are destroyed or all enemies are eliminated.

---

**Note:**  
ShipFederate does not simulate missiles directly; it issues missile launch orders and updates ship states based on missile impacts. Missile simulation is handled by separate