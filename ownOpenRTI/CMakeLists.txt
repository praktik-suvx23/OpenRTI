cmake_minimum_required(VERSION 3.10)
project(MyOpenRTIFederate)

set(CMAKE_CXX_STANDARD 17)

set (Source_Directory ${CMAKE_SOURCE_DIR})

set(CMAKE_BUILD_TYPE Debug) # Ensure debug mode
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g") # Include debug symbols

# Include the OpenRTI headers
include_directories(/usr/local/include/rti1516e)
include_directories(/usr/local/include/rti1516e/RTI/time)
include_directories(${CMAKE_SOURCE_DIR}/OpenRTI)
include_directories(${CMAKE_SOURCE_DIR}/OpenRTI/RTI)
include_directories(${CMAKE_SOURCE_DIR}/OpenRTI/RTI/time)
include_directories(${Source_Directory}/include)

# Link the OpenRTI libraries (assuming they are found in /usr/local/lib)
link_directories(/usr/local/lib)

# Add the executables
add_executable(MyFederate src/main.cpp src/MyFederateAmbassador.cpp)
add_executable(SubFederate src/federateSubPositionAndSpeed.cpp src/MyFederateAmbassador.cpp)


install(TARGETS MyFederate SubFederate DESTINATION bin)
install(DIRECTORY configs DESTINATION share/MyOpenRTIFederate)

# Link OpenRTI libraries (correct library names without 'lib' prefix or '.so' suffix)
target_link_libraries(MyFederate PRIVATE rti1516e fedtime1516e OpenRTI)
target_link_libraries(SubFederate PRIVATE rti1516e fedtime1516e OpenRTI)
target_link_libraries(rtinode PRIVATE OpenRTI)


# Copy the FOM.xml file to the build directory
configure_file(${CMAKE_SOURCE_DIR}/foms/FOM.xml ${CMAKE_BINARY_DIR}/foms/FOM.xml COPYONLY)