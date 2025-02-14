### README ABOUT carPROGRAM

## Overview

This is a simple program where we experimented with making sync points work between multiple federates.

1. **testFederate**
        Acts as the MasterFederate that synchronizes all federates at launch.
        Requires manual control by following the instructions in the terminal.

2.  **testPublisher**
        Publishes a parameter that increments over time.

3.  **testSubscriber**
        Receives the parameter supplied by ``testPublisher`` and subscribes to that value.

3.  **testSubscriber2**
        Same as ``testSubscriber``, except it only subscribes to every 10th value


## How to get started

1. **rtinode**
    Ensure that ``rtinode`` from OpenRTI is installed. If not, follow the instructions in the ``README`` file in the root folder.

2.  **Build the Project**
    Ensure you have the necessary build tools and dependencies installed, then run the following commands:
    ```bash
    mkdir build
    cd build
    cmake ..
    make
    make install
    ```

3. **Start a minimum of four terminals**
    Terminal 1: run `rtinode`
    Terminal 2: in the ``build`` folder, run `./testFederate`
    Terminal 3: in the ``build`` folder, run `./testPublisher`
    Terminal 4: in the ``build`` folder, run `./testSubscriber`
    Terminal 5: in the ``build`` folder, run `./testSubscriber2` (optional)

4. **Follow instructions in each terminal**
    **In `./carMastFederate` terminal**:
    Press ``Enter`` when all other terminals are up and running to sync all federates. This terminal should be closed last to ensure the federation and its variables are correctly closed. When all other federates have finished or been closed, keep pressing ``Enter`` until this federate resigns from the federation.

### DISCLAIMER
This project was created to gain a better understanding of HLA and how FOM files work, particularly the ``synchronizationPoint`` function in the FOM file.

The code is provided *as is*. Use at your own discretion.
