cmake_minimum_required(VERSION 3.10)
project(MyOpenRTIFederate)

set(CMAKE_CXX_STANDARD 17)

set (Source_Directory ${CMAKE_SOURCE_DIR})

set(CMAKE_BUILD_TYPE Debug) # Ensure debug mode
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g") # Include debug symbols

# Include the OpenRTI headers
include_directories(${Source_Directory}/libs)
include_directories(${Source_Directory}/libs/rti1516e)
include_directories(${Source_Directory}/libs/rti1516e/RTI)
include_directories(${Source_Directory}/libs/rti1516e/RTI/time)

# Link the OpenRTI libraries (assuming they are found in /usr/local/lib)
link_directories(${Source_Directory}/libs)

# Add the executables

# Link OpenRTI libraries (correct library names without 'lib' prefix or '.so' suffix)

# Copy the FOM.xml file to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/foms DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(src/myProgram)
add_subdirectory(src/syncProgram)
add_subdirectory(src/carProgram)