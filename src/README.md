# OpenRTI Project

## Overview

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

6. **Run the Missile**
    To start your federate, use the following command in a new terminal in the build directory:
    ```bash
    ./Missile
    ```
    Missile recieves interactions from the two different ships and launches missiles on targetship that gets sent as a parameter in the interaction. It then subscribes to the targeted ships attributes.

    **Data that gets updated Locally for Missile**
    * Position
    * CurrentSpeed
    * currentFuelLevel
    * currentAltitude

    **Explaination:**
    
    Using the data it is subscribed to from the target ship the Missile then later calculates the distance to said ship. It also then updates all values accordingly such as 
    * DistanceToTarget (for the moment when This<50 federation is resigned and target is reached)
    * Altitude
    * Direction (Angle between position values and therefor the heading/bearing for the Missile)
    * Speed (random values between 250-450 for now)
    * Position (Also the somewhat predicted next positionValue for Missile)

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

## How OpenRTI works

OpenRTI uses the IEEE HLA standard for it's structure and functionality. This program uses the rti1516e (1516 extended) standard but the RTI13 and rti1516 is also available.

The step by step list that you want to follow when creating and running a federate is

1. **Connect to the rti**

2. **Create federation execution (If not already created by another federate, also needs a valid FOM (check foms folder for FOM.xml))**

3. **Join federationExecution**

4. **Initialize handles**

5. **Get handles**

6. **Publish-Subscribe to classAttributes/InteractionClass (Depending on wanted functionality for federate)**

7. **Register objectInstance (Only publishers need this)**

8. **Send interaction or update attribute values**

9. **Request time advancement (if using time management with logicalTime)**

10. **Resignation of federates and cleanup**





