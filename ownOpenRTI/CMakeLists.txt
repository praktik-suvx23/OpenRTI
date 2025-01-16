cmake_minimum_required(VERSION 3.10)
project(MyOpenRTIFederate)

set(CMAKE_CXX_STANDARD 17)

# Include the OpenRTI headers
include_directories(/usr/local/include/rti1516e)
include_directories(/usr/local/include/rti1516e/RTI/time)

# Link the OpenRTI libraries (assuming they are found in /usr/local/lib)
link_directories(/usr/local/lib)

# Find your federate source files
add_executable(MyFederate src/main.cpp)

# Link OpenRTI libraries (correct library names without 'lib' prefix or '.so' suffix)
target_link_libraries(MyFederate PRIVATE rti1516e fedtime1516e)
