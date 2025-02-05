cmake_minimum_required(VERSION 3.10)
project(MyOpenRTIProgram)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug) # Ensure debug mode
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g") # Include debug symbols

# Include the OpenRTI headers
include_directories(${CMAKE_SOURCE_DIR}/../../include)
include_directories(${CMAKE_SOURCE_DIR}/../../include/rti1516e)
include_directories(${CMAKE_SOURCE_DIR}/../../include/rti1516e/RTI)
include_directories(${CMAKE_SOURCE_DIR}/../../include/rti1516e/RTI/time)
include_directories(${CMAKE_SOURCE_DIR}/../../build/src/OpenRTI)
include_directories(${CMAKE_SOURCE_DIR})

# Link the OpenRTI libraries
link_directories(${CMAKE_SOURCE_DIR}/../../lib)

# Add the executables

add_executable(MyFederate MyFederate.cpp)
add_executable(MyPublisher MyPublisher.cpp)

install(TARGETS MyFederate MyPublisher DESTINATION bin)
install(DIRECTORY configs DESTINATION share/MyOpenRTIFederate)

# Link OpenRTI libraries (correct library names without 'lib' prefix or '.so' suffix)
target_link_libraries(MyFederate PRIVATE rti1516e fedtime1516e OpenRTI)
target_link_libraries(MyPublisher PRIVATE rti1516e fedtime1516e OpenRTI)

# Copy the FOM.xml file to the build directory
#configure_file(${CMAKE_SOURCE_DIR}/foms/FOM.xml ${CMAKE_BINARY_DIR}/foms/FOM.xml COPYONLY)
#configure_file(${CMAKE_SOURCE_DIR}/foms/test_fdd.xml ${CMAKE_BINARY_DIR}/foms/test_fdd.xml COPYONLY)
