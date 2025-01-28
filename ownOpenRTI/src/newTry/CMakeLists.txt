cmake_minimum_required(VERSION 3.10)

# Add executable for main.cpp
add_executable(newTry src/main.cpp src/fedAmbassador.cpp src/Federate.cpp)

install(TARGETS newTry DESTINATION bin)

# Include directories for header files
target_include_directories(newTry PRIVATE include)

# Link OpenRTI libraries
target_link_libraries(newTry PRIVATE rti1516e fedtime1516e)

# Install the executable in the Test folder in the build directory
install(TARGETS newTry DESTINATION Test)