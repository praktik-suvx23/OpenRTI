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
    Ensure you have the necessary build tools and dependencies installed. Then, run the following commands:
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
    Explanation:
    This starts that rti to listen on a specifik port (defaultPort:14321)

4. **Run syncFederate**
    This is a simple help federate that makes sure every other federate launches at the same time. This federate require a manual input in the terminal for the rest of the program to launch.
    ```bash
    ./syncFederate
    ```

5. **Run the ShipPublisherFederate**
    To start your federate, use the following command in a new terminal in the build directory:
    ```bash
    ./MyShip
    ```
    Explanation:
    Starts a shipPublisher that publishes position value. Position value is dependent on speed and angle for a new position in a specific direction.
    Data that gets published:

    * currentShipPosition
    * futureShipPosition

6. **Run the subscriber**
    To start your federate, use the following command in a new terminal in the build directory:
    ```bash
    ./Robot
    ```
    **Data that gets updated Locally for Robot**
    * Name (name for the robot, e.g Robot(instanceNumber))
    * Position
    * CurrentSpeed
    * currentFuelLevel
    * currentAltitude
    * FederateName (used to handle unique subscribes of unique publishers)

    Explaination: 
    
    Using the data it is subscribed to from the ship the Robot then later calculates the distance to said ship. It also then updates all values accordingly such as 
    * DistanceToTarget (for the moment when This<50 federation is resigned and target is reached)
    * Altitude
    * Direction (Angle between position values and therefor the heading/bearing for the Robot)
    * Speed (random values between 250-450 for now)
    * Position (Also the somewhat predicted next positionValue for Robot)

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

Explanation:

    The rtinode process acts as the central RTI instance that federates connect to. It's like a server waiting for client connections.
    If it's running without error messages, it’s in standby mode, ready to accept connections from federates.

When the `rtinode` is running you may use:
```
sudo lsof -i -P -n | grep rtinode
```
to find what ports it's using. Since it's a OpenRTI project it's *most likely* **14321**. This can be changed if running `rtinode -i:14321` or changing the port in the rtinode_config.xml 


### TIPS
If you find OpenRTI's repository hard to understand, you may look at [Portico](https://github.com/openlvc/portico). They have some C++ example code that can be somewhat translated to OpenRTI. And if nothing else, you may see in their example how a ```.xml``` file could look like.
