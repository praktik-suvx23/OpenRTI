# PyLink

**PyLink** is a lightweight C++ federate in an HLA (High-Level Architecture) simulation. It acts as a communication bridge between the RTI (Run-Time Infrastructure) and the external Python visualization component, `ReceiveData.py`.

Due to OpenRTI’s lack of native Python support, PyLink was introduced as a practical workaround to forward simulation data from C++ to Python via sockets. While this design is considered temporary, PyLink currently fulfills all functional requirements imposed by `ReceiveData.py` and provides fine-grained control over what simulation data gets transmitted.

> ⚠️ **Note:** PyLink may eventually be replaced if direct Python RTI integration becomes available. For now, it is essential for the visualization pipeline. The main performance constraint is on the Python side, not PyLink itself.

---

## Purpose

- Subscribes to key simulation data (ship and missile attributes) via RTI.
- Transfers selected data to the Python script `ReceiveData.py` over sockets.
- Sends only every 10th update to minimize socket latency and reduce load on the Python receiver.
- Logs each transmission to `../../log/loggingPyLink.txt` for traceability and debugging.

---

## File Overview

### PyLinkAmbassador.cpp/.h
- Handles RTI interactions:
  - `announceSynchronizationPoint()` — Synchronizes with the federation.
  - `discoverObjectInstance()` — Detects new simulation entities (ships, missiles, etc.).
  - `reflectAttributeValues()` — Receives updated attributes from subscribed object classes.
- Manages RTI handles:
  - `ObjectClassHandle`, `AttributeHandle`
  - Other RTI-related variables for data subscription.

### PyLink.cpp/.h
- Main application logic:
  - RTI setup (`createRTIAmbassador`, `connectToRTI`, `initializeFederation`, `joinFederation`)
  - Handle management (`initializeHandles`, `subscribeAttributes`)
  - Communication (`socketsSetup` — initializes socket connection with `ReceiveData.py`)
- Main loop:
  - Waits for attribute updates from the RTI.
  - Forwards data to `ReceiveData.py` every 10th update.
  - Writes all transmitted data to `../../log/loggingPyLink.txt`.

---

## Runtime Behavior

### Initialization Phase

- Connects to the RTI and joins the simulation federation.
- Synchronizes with other federates at a sync point.
- Subscribes to relevant object attributes (ships and missiles).
- Sets up socket communication with the external Python receiver.

### Execution Phase

- Continuously listens for simulation data updates.
- Forwards data to the Python component with controlled frequency (1 in 10).
- Logs all transmissions for traceability and debugging.

## RTI Data Subscription

PyLink subscribes to the following attributes:
- **Ships:** `FederateName`, `ShipTeam`, `Position`, `Speed`, `ShipSize`
- **Missiles:** `MissileID`, `MissileTeam`, `Position`, `Altitude`, `Speed`

## Socket Communication

- Data is sent to Python (`ReceiveData.py`) via three sockets:
  - Red ships: `REDSHIP_PORT`
  - Blue ships: `BLUESHIP_PORT`
  - Missiles: `MISSILE_PORT`

---

## Notes

- PyLink is not a performance bottleneck; it exists to reduce the load on the Python-side receiver (`ReceiveData.py`), which currently lacks direct RTI integration.
- Acts as a necessary bridge due to the limitations of OpenRTI and the temporary visualization pipeline.
- The log file (`../../log/loggingPyLink.txt`) can grow large during long simulations; it is intended mainly for debugging