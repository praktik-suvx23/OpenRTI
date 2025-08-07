# OpenRTI `include/` Folder Overview

This folder contains shared helper files and utility modules used throughout the Federation project.  
Most files here provide reusable functions, classes, or definitions that support simulation logic, data handling, and inter-federate communication.  
If you need common functionality—such as position encoding, logging, or ship/missile calculations—these files are designed to be easily available and integrated across multiple components.

---

## File Summaries

### decodePosition.cpp

Utility functions for converting geographic position data between HLA-encoded types and standard C++ types.

- `decodePositionRec`: Decodes HLA `VariableLengthData` to a C++ `(latitude, longitude)` pair, with range validation.
- `encodePositionRec`: Encodes a C++ position pair into an HLA `HLAfixedRecord`.

### jsonParse.h

Simple `JsonParser` class for extracting ship configuration data from a JSON-like text file.

- Parses ship attributes (length, width, height, missiles, canons).
- Getter methods for each attribute.
- Quick configuration loading and debugging.

### loggingFunctions.cpp

Utility functions for logging simulation events to text files, organized by federate type.

- Configurable log file paths for RedShip, BlueShip, Missile, Admin, and PyLink.
- Functions to initialize log files, write messages (`std::string` and `std::wstring`), and convert between string types.

### MissileCalculator.cpp

Helper functions for missile simulation calculations.

- Speed, fuel, and altitude adjustment for realistic movement.
- Geographic conversions (degrees/radians), bearing calculations.
- Position, bearing, and distance computations using spherical geometry.
- Randomization for speed and robust altitude handling.

### MissileStatus.h

Defines missile status codes and a communication struct for ship-missile interaction.

- `MissileStatus` enum: Missile lifecycle states (inactive, active, target locked, hit, destroyed, etc.).
- `MissileCommunication` struct: Bundles ship, target, and missile IDs, number of missiles fired, and current/previous status.

### ObjectInstanceHandleHash.h

Custom hash specialization for `rti1516e::ObjectInstanceHandle` to enable its use as a key in C++ hash containers.

- Implements `std::hash<ObjectInstanceHandle>` using the handle’s string representation.
- Enables efficient storage and lookup in `std::unordered_map` and similar structures.

### ShipHelperFunctions.cpp

Helper functions for ship placement and navigation.

- `generateDoubleShipPosition`: Randomized ship positions within a grid, accounting for team and spatial separation.
- `getOptimalShipsPerRow`: Calculates optimal row size for ship formation.
- `calculateBearing`: Computes compass bearing between geographic positions.

---

**Note:**  
These files are intended for use across the entire Federation codebase.  
If you need a utility or helper function, check here first before duplicating logic elsewhere.