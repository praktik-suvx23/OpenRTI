cmake_minimum_required(VERSION 3.10)

# Add executable for main.cpp
add_executable(ExamplePortico main.cpp testFederate.cpp testFedAmb.cpp)

# Link OpenRTI libraries
target_link_libraries(ExamplePortico PRIVATE rti1516e fedtime1516e)

# Install the executable in the Test folder in the build directory
install(TARGETS ExamplePortico DESTINATION Test)