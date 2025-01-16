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
    ```

3. **Run the Federate**
    To start your federate, use the following command:
    ```bash
    ./MyFederate
    ```

## How It Works

### Federate

A federate is a simulation application that interacts with other federates in a federation. Each federate must join a federation to participate in the simulation.

### Federation

A federation is a collection of federates that share a common simulation environment. The federation is managed by the RTI, which handles communication and coordination between federates.

### Starting a Federation

Currently, you have successfully started your federate but have not been able to start your federation. Ensure that you have the federation configuration file correctly set up and that the RTI is running. Here are the steps to start a federation: