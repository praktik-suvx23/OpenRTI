### README ABOUT carPROGRAM

## Overview

This program is a re-creation of Pitch's "Fuel Economy" example, but minimized to use *only* C++ instead of both Java and C++. It is primarily a proof of concept (PoC) where we recreated "CarSimC" from "FuelEconomy" in a *light* version.

This new standalone C++ program is based on the original CarSimC's FOM, scenario files, and car files (found in the ``fomCar`` and ``configCar`` folders). It is purely terminal-based and uses three different federate files:

1. **carMastFederate**
        Synchronizes all federates at launch.
        Ensures that the federation and its objects/attributes are destroyed upon completion.
        Requires manual control by following instructions in the terminal.

2.  **carScenarioFederate**
        Allows the user to select the preferred scenario, initial fuel amount, and a time scale factor for the simulation.

3.  **carFederate**
        Assigns federates to cars.
        Creates four separate federates that load different ``carConfig.car`` files.
        Runs the selected scenario by moving the car in a straight line from one coordinate point to another.


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

3. **Start four terminals**
    Terminal 1: run `rtinode`
    Terminal 2: in the ``build`` folder, run `./carMastFederate`
    Terminal 3: in the ``build`` folder, run `./carScenarioFederate`
    Terminal 4: in the ``build`` folder, run `./carFederate`

4. **Follow instructions in each terminal**
    TL;DR Guide
    **In `./carMastFederate` terminal**:
    Press ``Enter`` when all other terminals are up and running to sync all federates. This terminal should be closed last to ensure the federation and its variables are correctly closed. When all other federates have finished or been closed, keep pressing ``Enter`` until this federate resigns from the federation.
    
    **In `./carScenarioFederate` terminal**: 
    Once all federates are synced, you will be prompted to select a scenario. After launching and publishing the scenario for ``carFederate``, this federate will resign.

    **In `./carFederate` terminal**: 
    Multiple federates (cars) will run the simulation. This terminal will display position, time, and fuel consumption data. No user input is required.

### DISCLAIMER: Work in Progress (PoC)
This project is not yet complete and may not function flawlessly. It can run simulations and load car files within an HLA environment.

The code is provided *as is*. Use at your own discretion.
