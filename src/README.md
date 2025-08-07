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
    This is a simple help federate that makes sure every other federate launches at the same time. This federate require a manual input in the terminal for the rest of the program to launch. Also requires input on how many Ships should be on Blue ships and Red ships sides of combat
    ```bash
    ./AdminFederate
    ```

5. **Run the Ships**
    To start your federate, use the following command in a new terminal in the build directory:
    ```bash
    ./Ship
    ```
    Ship wants you to enter 1(Blue) or 2(Red) on startup to choose what team you want to be apart of 
    
    Data that gets published and subscribed to:

    * currentShipPosition
    * futureShipPosition (Not used for now)
    * shipsize (Used to spot several ships target the biggest)
    * shipNumberOfRobots (Used as ammunition to fire X amount of robots)

    You need to do this for each program of ships you want to start (Minimum of one of each team, for the moment the code only works for 1 of each team)

6. **Run the MissileCreator**
    To start your federate, use the following command in a new terminal in the build directory:
    ```bash
    ./MissileCreator
    ```
    **MissileCreator** receives fire requests (interactions) from the two different ship federates. When a ship wants to fire at a target ship, it sends an interaction to MissileCreator. Upon receiving such a request, MissileCreator launches a new Missile federate process, passing the relevant parameters (such as shooter, target, and initial positions) to the new Missile instance.

    **What MissileCreator does:**
    - Listens for fire requests from ships.
    - Launches a new Missile federate for each fire request, with the correct parameters.
    - Optionally tracks or logs which missiles have been launched and their assigned targets.

    **MissileCreator does NOT simulate missile flight or update missile state itself.**  
    Each Missile federate, once launched, is responsible for:
    * Subscribing to its target ship's attributes (position, speed, altitude, etc.)
    * Calculating its own flight path and distance to the target
    * Updating its own state (position, speed, altitude, direction, etc.)
    * Reporting when it reaches or hits the target

    **MissileCreator's responsibilities are limited to launching and managing Missile federates, not simulating missile behavior.**

    **Missile federates (launched by MissileCreator) update locally:**
    * Position
    * CurrentSpeed
    * CurrentFuelLevel
    * CurrentAltitude

    **Missile federate explanation:**
    Using the data it subscribes to from the target ship, each Missile federate calculates the distance to its target and updates its own values accordingly, such as:
    * DistanceToTarget (when this < 50, the federation is resigned and the target is considered reached)
    * Altitude
    * Direction (angle between position values, i.e., heading/bearing)
    * Speed (random values between 250-450 for now)
    * Position (including predicted next position)

7. **[Optional] Run Visualization Components (PyLink + ReceiveData.py)**

The components in the `VisualRepresentation/` folder provide post-simulation visualization of ship and missile data.  
These are **not required** for the simulation to function, but they offer useful insights after execution and help AdminFederate detect when data processing is complete.

#### Dependency

- **PyLink** (C++) and **ReceiveData.py** (Python) are interdependent; neither is meaningful without the other.
- Data is transferred from PyLink to ReceiveData.py using sockets, due to OpenRTI’s lack of native Python bindings.

#### Purpose

- **PyLink** subscribes to selected RTI data and forwards it to Python via sockets (every 10th update to reduce overhead).
- **ReceiveData.py** processes and visualizes this data after the simulation has completed.
- **ReceiveData.py** also sends heartbeats back to AdminFederate, allowing Admin to automatically shut down the federation once data handling is complete.
- These components do **not** provide live visualization. Data is only displayed after all simulation data has been received.

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
  This program should be run after the rest of the simulation have completed to generate a summary and average values for the simulation. Could serve useful for debugging or performance checking.

  To run:
  ```bash
  ./TextToTable
  ```

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

## How It Works

### Federate 

A federate is a simulation application that interacts with other federates in a federation. Each federate must join a federation to participate in the simulation.

### Federation

A federation is a collection of federates that share a common simulation environment. The federation is managed by the RTI, which handles communication and coordination between federates.

#

After installing the project you may try and use:
```
rtinode -c OpenRTI/configs/rtinode_config.xml
```
This command starts the RTI (Run-Time Infrastructure) using the configuration file rtinode_config.xml. 
In the config file you can change server options and more for the RTI. The program works with just running the rtinode commands aswell, but with this you can make some extra changes if needed.

**Explanation:**

    The rtinode process acts as the central RTI instance that federates connect to. It's like a server waiting for client connections.
    If it's running without error messages, it’s in standby mode, ready to accept connections from federates.

When the `rtinode` is running you may use:
```
sudo lsof -i -P -n | grep rtinode
```
to find what ports it's using. Since it's a OpenRTI project it's *most likely* **14321**. This can be changed if running `rtinode -i:14321` by inputing desired port in this case 14321 or changing the port in the rtinode_config.xml and then start the rtinode with the config file.


### TIPS
If you find OpenRTI's repository hard to understand, you may look at [Portico](https://github.com/openlvc/portico). They have some C++ example code that can be somewhat translated to OpenRTI. And if nothing else, you may see in their example how a ```.xml``` file could look like.
You may also look at [Pitch]-HLA tutorial (https://pitchtechnologies.com/hlatutorial/). They have some federate examples and a generic HLA-tutorial in text format.

Another thing is that you can install gdb debugger, a very handy tool for debugging code

Also if there is no folder with the name "log" within the src directory the user can create that folder for extra logging during simulation. Can be very useful for debugging and making sure that all federates work as intended.

### Good commandos to know

* `ps aux | grep "MyFederateStartCommandName` Gives output to show if federate is running
* `gdb ./MyFederateStartCommand` If GDB debugger is installed this command can be used to start debugging one of you programs
    #### Good commandos for GDB debugging (can only be used after starting gdb)
    * `run` Runs the program with the debugger
    * `break main.cpp:43` Creates a breakpoint in main.cpp at line 43
    * `break main.cpp:myFunction`This creates a breakpoint in main.cpp at the start of myFunction
    * `next` Used to travel line by line when program enters a breakpoint
    * `continue` Continues program to next Breakpoint
    * `backtrace` Good to use when program crashes to see backsteps of what happened
    * `print myDatatypeInCurrentContext` Prints the current value for a datatype, can also be used to print the output value for a return function