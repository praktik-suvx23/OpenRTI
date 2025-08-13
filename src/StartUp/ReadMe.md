# OjOpenRTI Simulation Startup

This folder contains input files and the startup script for running the OjOpenRTI simulation.

## Usage

- Run the simulation with:
  ```bash
  make run_simulation
  ```

- Each federate starts in its own GNOME Terminal window.
- Inputs for AdminFederate and Ship federates are read from files in `inputs/`.

## Input Files

- **Admin.txt**  
  ```
  2
  2
  2.0

  ```
  (Each line is an input for AdminFederate. Important that there is a blank line after 2.0 otherwise the program does not work)

- **Ship1.txt**  
  ```
  1
  ```
  (Team number for Ship1.)

- **Ship2.txt**  
  ```
  2
  ```
  (Team number for Ship2.)

## Notes

- Edit input files to change simulation parameters.
- All windows close automatically after the simulation run.