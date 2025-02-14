# OpenRTI Project

## Overview

This project is an implementation of the OpenRTI (Run-Time Infrastructure) for distributed simulations. It allows multiple simulation applications, known as federates, to communicate and coordinate their actions in a shared simulation environment called a federation.

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

4. **Run the PublisherFederate**
    To start your federate, use the following command in a new terminal in the build directory:
    ```bash
    ./MyPublisher
    ```
    Explanation:
    This starts the publisherFederate. The PublisherFederate publishes data for the subscriber MyFederate by connecting to the rti and updateing data values. 
    Data that gets published:

    * Name (name for the robot, e.g Robot(instanceNumber))
    * StartPosition (gets sent once the subscriber unsubscribes.)
    * CurrentSpeed
    * currentFuelLevel
    * currentAltitude
    * FederateName (used to handle unique subscribes of unique publishers)


5. **Run the ShipPublisherFederate**
    To start your federate, use the following command in a new terminal in the build directory:
    ```bash
    ./MyShip
    ```
    Explanation:
    Starts a shipPublisher that publishes position value. (Future update will make position value dependent on speed and angle for a new position in a specifik direction)
    Data that gets published:

    * currentShipPosition
    * currentShipDirection (not Implemented yet)
    * currentShipSpeed  (not Implemented yet)

6. **Run the subscriber**
    To start your federate, use the following command in a new terminal in the build directory:
    ```bash
    ./MyFederate
    ```

## How It Works

### Federate

A federate is a simulation application that interacts with other federates in a federation. Each federate must join a federation to participate in the simulation.

### Federation

A federation is a collection of federates that share a common simulation environment. The federation is managed by the RTI, which handles communication and coordination between federates.

#

After installing the project you may try and use:
```
rtinode -c /usr/OjOpenRTI/OpenRTI/configs/rtinode_config.xml
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
to find what ports it's using. Since it's a OpenRTI project it's *most likely* **14321**. This can be changed if running `rtinode -i:14321` or changeing the port in the rtinode_config.xml 


### TIPS
If you find OpenRTI's repository hard to understand, you may look at [Portico](https://github.com/openlvc/portico). They have some C++ example code that can be somewhat translated to OpenRTI. And if nothing else, you may see in their example how a ```.xml``` file could look like.
