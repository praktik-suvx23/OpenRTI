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

add_compile_definitions(
    DATA_LOG_PATH="${CMAKE_SOURCE_DIR}/src/myProgram/log/finalData.txt"
    JSON_PARSER_PATH="${CMAKE_SOURCE_DIR}/src/myProgram/ShipData/ShipData.json"
)

# Link the OpenRTI libraries (assuming they are found in /usr/local/lib)
link_directories(${Source_Directory}/libs)

# Copy the FOM.xml file to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/src/myProgram/foms DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(src/myProgram)