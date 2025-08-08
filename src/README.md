# OpenRTI Project

## Overview

OpenRTI uses the IEEE HLA standard for its structure and functionality. This program uses the rti1516e (1516 extended) standard, but RTI13 and rti1516 are also available.

This project is an implementation of the OpenRTI (Run-Time Infrastructure) for distributed simulations. It allows multiple simulation applications, known as federates, to communicate and coordinate their actions in a shared simulation environment called a federation. All federates use logicalTime and lookaheads to be able to get data and keep track of simulationTime vs RealTime.

## Project Setup

1. **Clone the Repository**
  ```bash
  git clone https://github.com/praktik-suvx23/OpenRTI.git
  cd OpenRTI
  ```

2. **Build the Project**
  Ensure you have the necessary build tools and dependencies installed (See previous README.md). Then, run the following commands:
  ```bash
  mkdir build
  cd build
  cmake ..
  make
  make install
  ```

## Running the Federates

3. **Run the RTI**
  To start your federate, use the following command in the build directory:
  ```bash
  rtinode
  ```
**Explanation:**
This starts that rti to listen on a specifik port (defaultPort:14321)

4. **Run AdminFederate**
  You must run one instance of AdminFederate in a terminal for the simulation to work:
  ```bash
  ./AdminFederate
  ```

After starting AdminFederate, **continue with steps 5 and 6**, and optionally step 7.

Once all required programs are running, follow the prompts in the AdminFederate terminal.  
If everything is set up correctly, you’ll start seeing data generated in each terminal.

- If you completed step 7 (visualization), the program will finish automatically.
- If not, you will need to manually close AdminFederate after the Ship and Missile terminals complete.

5. **Start the Ship Federates**
  To start a Ship federate, open a new terminal in the build directory and run:
  ```bash
  ./Ship
  ```

- You must start at least **two Ship federates**—one for each team (**Red** and **Blue**).
- Each instance will prompt you to select a team when it starts. Follow the on-screen instructions in each terminal and ensure that:
  - One Ship federate joins the **Red** team.
  - Another Ship federate joins the **Blue** team.
- *Tip:* You can launch additional Ship federates if desired, but you must have at least one per team for the simulation to run correctly.

6. **Run the MissileCreator**
  Open a **new terminal** in the build directory and run:
  ```bash
  ./MissileCreator
  ```
This federate listens for FireMissile interactions from Ship federates and launches a new Missile federate for each missile. MissileCreator does not simulate missiles directly.

7. **[Optional] Run Visualization Components (PyLink + ReceiveData.py)**

The components in the `VisualRepresentation/` folder provide post-simulation visualization of ship and missile data.  
These are **not required** for the simulation to function, but they offer useful insights after execution and help AdminFederate detect when data processing is complete.

#### One-Time Python Setup

Ensure Python 3 and required libraries are installed:
```bash
sudo apt install python3 python3-matplotlib python3-numpy python3-scipy
```

  #### How to Run

    1. In the `build/` directory, start PyLink just like other federates:
      ```bash
      ./PyLink
      ```

    2. In a separate terminal, navigate to the `VisualRepresentation/` folder and launch the Python visualization:
      ```bash
      python3 ReceiveData.py
      ```

8. **[Optional] Run the TextToTable**

  This program should be run **after the rest of the simulation have completed** to generate a summary and average values for the simulation. Could serve useful for debugging or performance checking.

  To run:
  ```bash
  ./TextToTable
  ```

---

##### Additional Information

Each federate or major component in this project has its own `README.md` file located in its respective folder. These files provide detailed explanations of folder contents, internal logic, and runtime behavior specific to that component.

- **AdminFederate/**  
  Contains a README describing how AdminFederate sets up the simulation, enforces synchronization points, communicates with Python components, and manages federation shutdown.

- **MissileFederate/**  
  Includes a README detailing how individual missiles are simulated, how they calculate flight paths, select targets, and interact with ships.

- **ShipFederate/**  
  Has a README explaining how ships are initialized, their attributes (like size and robot count), and how they send fire interactions to launch missiles.

- **MissileCreator/**  
  Contains a README describing how MissileCreator launches new Missile federates in response to ship fire requests.

- **VisualRepresentation/CommunicationWithPython/**  
  Contains a README for the PyLink federate, which subscribes to RTI data and sends it to Python for visualization via sockets.  
  **Note:** PyLink is required for `ReceiveData.py` to function. The README explains its purpose and implementation.

- **VisualRepresentation/**  
  Includes a README for the `ReceiveData.py` script, describing how it receives, logs, and visualizes missile and ship data post-simulation, and how it sends heartbeats to AdminFederate.

- **TextToTable**
  This file is optional but can be useful to get an easier overview of the run of the missiles and ships in the simulation.

- **include/**  
  This folder contains various helper files and utilities that support the entire Federation project. These files provide shared functions, classes, and definitions used by multiple components to simplify development and ensure consistency.  
  A README in this folder explains the purpose and usage of each file.

---

## How It Works

### Federate

A **federate** is a simulation application that communicates with other federates within a shared simulation. Each federate must join a federation to participate.

### Federation

A **federation** is a group of federates collaborating in a shared simulation environment. Communication and coordination between federates are managed by the RTI (Run-Time Infrastructure), which acts as the central middleware for interaction.

---

### Learn More About HLA

If you're new to HLA (High-Level Architecture) or want a refresher, check out these resources:

- [Wikipedia – High Level Architecture](https://en.wikipedia.org/wiki/High_Level_Architecture)  
  A solid overview of core HLA concepts.

- [Pitch Technologies HLA Tutorial](https://pitchtechnologies.com/hlatutorial/)  
  A beginner-friendly guide with hands-on examples.

  > It's recommended to read the Wikipedia article first to build a foundation before diving into the tutorial.

- Explore other projects:  
  Check out open-source HLA implementations like OpenRTI, Portico, or browse HLA-based repositories on GitHub for real-world examples and ideas.

Check out open-source HLA implementations like OpenRTI, Portico, or browse HLA-based repositories on GitHub for real-world examples and ideas.

> ⚠️ HLA can be a complex subject if you're new to distributed simulation. Take your time and explore these resources to build a solid understanding.

---

## Running the RTI

After installing the project, you can start the RTI using:
```bash
rtinode
```
This runs the RTI with default settings. By default, **OpenRTI uses port `14321`** unless another port is specified via configuration or command-line argument.

To start `rtinode` with a custom configuration file (e.g., specifying a port or other server options), run:
```bash
rtinode -c OpenRTI/configs/rtinode_config.xml
```
You can also explicitly set the port when launching:
```bash
rtinode -i:14321
```
The `rtinode` process acts as a central RTI instance that federates connect to—essentially functioning like a server waiting for client connections.

---

### How to Check RTI Port

To verify which port `rtinode` is currently using:
```bash
sudo lsof -i -P -n | grep rtinode
```
This will list the active network connections and the port `rtinode` is listening on.

---

### Useful Commands for Development

#### General

- `ps aux | grep "MyFederateStartCommand"` – Check if your federate is currently running.

#### GDB Debugging (if `gdb` is installed)

- `gdb ./MyFederateStartCommand` – Launch your federate in GDB for debugging.

##### Common GDB Commands:

- `run` – Start the program with the debugger.
- `break main.cpp:43` – Set a breakpoint at line 43 in `main.cpp`.
- `break main.cpp:myFunction` – Set a breakpoint at the start of `myFunction` in `main.cpp`.
- `next` – Step to the next line of code.
- `continue` – Resume execution until the next breakpoint.
- `backtrace` – Print the call stack (useful after a crash).
- `print myVariable` – Print the value of a variable or function return.

> These commands have been particularly useful in debugging and developing this project.