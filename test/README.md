# OpenRTI Test Suite

This folder contains automated test scripts for the OpenRTI project.  
Each test runs a simulation scenario and saves its output logs for review.

## Why Use Scripts?

Some of the C++ programs in this project require manual user input during execution.  
These test scripts automate the process by piping input from dummy files (such as `dummy_input_admin.txt` and `dummy_input_ship1.txt`) directly into the executables.  
This simulates user interaction, allowing you to run automated tests without manual intervention.

## Tests

- **admin_federate_test.sh**  
  Runs the AdminFederate with dummy input to verify setup interactions.  
  Output log is saved in `log/AdminFederateTest/admin_output.log`.

- **full_run_test.sh**  
  Runs a full simulation with all federates and components using dummy inputs.  
  Output logs are saved in `log/FullTest/`:
  - `pylink_output.log`
  - `admin_output.log`
  - `ship1_output.log`
  - `ship2_output.log`

## Output Location

All test logs are placed in subfolders inside the `log` directory:
- `log/AdminFederateTest/` for admin federate test output
- `log/FullTest/` for full simulation test outputs

You can review these logs to verify correct behavior and debug issues.

