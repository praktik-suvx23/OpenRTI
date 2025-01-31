cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)

# Add executable for main.cpp
add_executable(newTry src/main.cpp src/fedAmbassador.cpp src/Federate.cpp)

# Include directories for header files
target_include_directories(newTry PRIVATE include)

# Link OpenRTI libraries
target_link_libraries(newTry PRIVATE rti1516e fedtime1516e)

# Copy FOM.xml to the output directory
add_custom_command(TARGET newTry POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/foms/FOM.xml $<TARGET_FILE_DIR:newTry>/FOM.xml
)

# Install the executable in the bin directory
install(TARGETS newTry DESTINATION bin)

# Install the executable in the Test folder in the build directory
install(TARGETS newTry DESTINATION Test)