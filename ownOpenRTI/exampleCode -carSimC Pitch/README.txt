# Building the CarSimC federate with CMake

## With Visual Studio Code

Visual Studio Code requires the Microsoft extension "CMake Tools" to work with
CMake projects, be sure and install it.

Once installed, VSCode will recognise the CMakeLists.txt and CMakePresets.json
at the top of the CarSimC folder which specify how to build the federate. We
have defined one config preset and one build preset per platform (windows,
linux, and macos).

Select the preset for your platform in the blue bar that runs along the bottom
of the application and press "Build".


## On the command line

Run these commands to build the federate:

$ cmake --preset windows
$ cmake --build --preset windows

Replace "windows" with the platform you are using, i.e., "linux" or "macos".