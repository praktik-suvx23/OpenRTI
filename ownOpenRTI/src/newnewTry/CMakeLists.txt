cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)

# Add executable for main.cpp
add_executable(mastFed masterFederate/src/main.cpp masterFederate/src/masterFedAmbassador.cpp masterFederate/src/masterFederate.cpp)
add_executable(carFed carFederate/src/carFedAmbassador.cpp carFederate/src/carFederate.cpp)

# Include directories for header files
target_include_directories(mastFed PRIVATE include)
target_include_directories(carFed PRIVATE include)

# Link OpenRTI libraries
target_link_libraries(mastFed PRIVATE rti1516e fedtime1516e)
target_link_libraries(carFed PRIVATE rti1516e fedtime1516e)

# Copy FOM.xml to the output directory
add_custom_command(TARGET mastFed POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/foms/FOM.xml $<TARGET_FILE_DIR:mastFed>/FOM.xml
)

# Install the executable in the bin directory
install(TARGETS mastFed DESTINATION bin)
install(TARGETS carFed DESTINATION bin)

# Install the executable in the Test folder in the build directory
install(TARGETS mastFed DESTINATION Test)
install(TARGETS carFed DESTINATION Test)