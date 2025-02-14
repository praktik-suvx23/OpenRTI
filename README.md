# OpenRTI

## How to Get Started

1. **Download the OpenRTI repository**  
    Open the OpenRTI repo and do the following steps in a Linux environment:

2. **Create a build directory**  
    ```sh
    mkdir build
    cd build
    ```

3. **Generate build files using CMake**  
    ```sh
    cmake ..
    ```

4. **Build the project**  
    ```sh
    make
    ```

5. **Install the project**  
    ```sh
    sudo make install
    ```

6. **Copy header files to the system include directory**  
    ```sh
    sudo cp -r ../include/* /usr/local/include/
    ```

7. **Copy library files to the system library directory**  
    ```sh
    sudo cp -r lib/* /usr/local/lib/
    ```

These steps are required to access the OpenRTI library and files in your own project.

When done, continue to the `README.md` in the `ownOpenRTI` folder.


# OpenRTI Project

## Overview

This project is an implementation of the OpenRTI (Run-Time Infrastructure) for distributed simulations. It allows multiple simulation applications, known as federates, to communicate and coordinate their actions in a shared simulation environment called a federation.

## Project Setup

1. **Clone the Repository**
    ```bash
    git clone https://github.com/praktik-suvx23/OpenRTI.git
    cd OpenRTI/ownOpenRTI
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

3. **Run the Federate**
    To start your federate, use the following command:
    ```bash
    MyFederate
    ```

## How It Works

### Federate

A federate is a simulation application that interacts with other federates in a federation. Each federate must join a federation to participate in the simulation.

### Federation

A federation is a collection of federates that share a common simulation environment. The federation is managed by the RTI, which handles communication and coordination between federates.

### Starting a Federation
Currently unable to start the RTI or the federation.
Work in progress!


#

After installing the project you may try and use:
```
rtinode -f path/to/configs/federation.xml
```
This command starts the RTI (Run-Time Infrastructure) using the configuration file federation.xml. However, since you're not seeing any output, it’s likely that rtinode is running but waiting for a connection from a federate. This is expected behavior unless there's an error message.

Explanation:

    The rtinode process acts as the central RTI instance that federates connect to. It's like a server waiting for client connections.
    If it's running without error messages, it’s in standby mode, ready to accept connections from federates.

When the `rtinode` is running you may use:
```
sudo lsof -i -P -n | grep rtinode
```
to find what ports it's using. Since it's a OpenRTI project it's *most likely* **14321**.


### TIPS
If you find OpenRTI's repository hard to understand, you may look at [Portico](https://github.com/openlvc/portico). They have some C++ example code that can be somewhat translated to OpenRTI. And if nothing else, you may see in their example how a ```.xml``` file could look like.

Or, you can sign up for the [Pitch](https://pitchtechnologies.com/hlatutorial/) tutorial. It's a free trial tier program, but it may be difficult to develop anything from their example. However, it's a great place to start for a better understanding!