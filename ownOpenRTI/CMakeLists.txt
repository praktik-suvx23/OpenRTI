cmake_minimum_required(VERSION 3.10)
project(MyOpenRTIFederate)

set(CMAKE_CXX_STANDARD 17)

set (Source_Directory ${CMAKE_SOURCE_DIR})

set(CMAKE_BUILD_TYPE Debug) # Ensure debug mode
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g") # Include debug symbols

# Include the OpenRTI headers
include_directories(/usr/local/include/rti1516e)
include_directories(/usr/local/include/rti1516e/RTI/time)
include_directories(${Source_Directory}/include)

# Link the OpenRTI libraries (assuming they are found in /usr/local/lib)
link_directories(/usr/local/lib)

# Find your federate source files
add_executable(MyFederate src/main.cpp)

install(TARGETS MyFederate DESTINATION bin)
install(DIRECTORY configs DESTINATION share/MyOpenRTIFederate)

# Link OpenRTI libraries (correct library names without 'lib' prefix or '.so' suffix)
target_link_libraries(MyFederate PRIVATE rti1516e fedtime1516e)
