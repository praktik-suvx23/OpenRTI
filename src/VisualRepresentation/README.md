# ReceiveData – 3D Data Visualization in Python

**ReceiveData.py** is a Python script designed to visualize the final state of ships and missiles in an HLA-based simulation. Due to OpenRTI's lack of native Python support, it communicates with the simulation via a socket-based bridge (the PyLink federate), receiving simulation data for analysis and display.

> **Note:**  
> This is a temporary solution to OpenRTI’s limitations. While functional, it does not provide real-time visualization and may introduce latency.

---

## Script Overview

**ReceiveData.py** is a core part of the simulation pipeline, responsible for:

- **Socket-Based Data Reception:**  
  Receives missile and ship data from the C++ PyLink federate via sockets.

- **Deferred Visualization:**  
  Processes and displays the final state only after all data is received.  
  *No live or real-time visualization.*

- **Heartbeat Mechanism:**  
  Sends periodic heartbeats to AdminFederate to indicate ongoing data processing:
  ```python
  def heartbeat_sender(admin_ip='127.0.0.1', admin_port=12348)
  ```
  When heartbeats stop, AdminFederate infers simulation completion.

- **Data Parsing and Validation:**  
  Ensures received data is correctly structured before visualization.

- **Debug Logging (Optional):**  
  Can log received data and errors for debugging and validation.

---

## Known Limitations

- **No Live Visualization:**  
  Visualization occurs only after all data is received; no streaming or real-time rendering.

- **Performance Bottleneck:**  
  Socket communication and serialization can slow down long or data-heavy simulations.

- **Temporary Architecture:**  
  This setup is a workaround. Future improvements may include RTI-compatible Python bindings or direct C++ visualization.

---

## Supporting Components

### SendData.cpp

- **Reliable Transmission:**  
  Prepares and transmits data to destinations like ReceiveData.py via sockets.
- **High-Throughput Optimized:**  
  Supports efficient data streaming for high-speed requirements.

### SerializeData.cpp

- **Flexible Format Support:**  
  Converts complex C++ data structures into formats like JSON or binary.
- **Robust Error Handling:**  
  Catches and reports formatting or parsing errors during serialization.

---

**Summary:**  
ReceiveData.py, together with PyLink, enables Python-based visualization for OpenRTI simulations. This architecture is temporary and may be replaced by more integrated solutions.

---

## Additional Information

There is another README file located in the `CommunicationWithPython` folder. This README describes the **PyLink** federation, which is required for `ReceiveData.py` to function.  
PyLink acts as a bridge between the C++ simulation and the Python visualization script, forwarding simulation data via sockets.  
For details on PyLink's purpose and implementation, refer to `CommunicationWithPython/README.md`.