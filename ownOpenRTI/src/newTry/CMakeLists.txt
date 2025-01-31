cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)

# Add executable for main.cpp
add_executable(newTry src/main.cpp src/fedAmbassador.cpp src/Federate.cpp)
add_executable(tryPub src/mainPub.cpp src/fedAmbassador.cpp src/pubFederate.cpp)
add_executable(trySub src/mainSub.cpp src/fedAmbassador.cpp src/subFederate.cpp)

# Include directories for header files
target_include_directories(newTry PRIVATE include)
target_include_directories(tryPub PRIVATE include)
target_include_directories(trySub PRIVATE include)

# Link OpenRTI libraries
target_link_libraries(newTry PRIVATE rti1516e fedtime1516e)
target_link_libraries(tryPub PRIVATE rti1516e fedtime1516e)
target_link_libraries(trySub PRIVATE rti1516e fedtime1516e)

# Copy FOM.xml to the output directory
add_custom_command(TARGET newTry POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/foms/FOM.xml $<TARGET_FILE_DIR:newTry>/FOM.xml
)

# Install the executable in the bin directory
install(TARGETS newTry DESTINATION bin)
install(TARGETS tryPub DESTINATION bin)
install(TARGETS trySub DESTINATION bin)

# Install the executable in the Test folder in the build directory
install(TARGETS newTry DESTINATION Test)
install(TARGETS tryPub DESTINATION Test)
install(TARGETS trySub DESTINATION Test)