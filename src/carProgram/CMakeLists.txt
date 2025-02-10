cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)

add_executable(carMastFederate mastFederate/src/main.cpp mastFederate/src/mastFederate.cpp mastFederate/src/mastFedAmb.cpp)
add_executable(carScenarioFederate scenarioFederate/src/main.cpp scenarioFederate/src/scenarioFederate.cpp scenarioFederate/src/scenarioFedAmb.cpp)
#add_executable(carPrintFederate subFederate/src/subMain.cpp subFederate/src/subFederate.cpp subFederate/src/subFedAmb.cpp)
#add_executable(carFederate subFederate2/src/subMain.cpp subFederate2/src/subFederate.cpp subFederate2/src/subFedAmb.cpp)

target_link_libraries(carMastFederate PRIVATE rti1516e fedtime1516e)
target_link_libraries(carScenarioFederate PRIVATE rti1516e fedtime1516e)

add_definitions(-DFOM_MODULE_PATH=\"${CMAKE_BINARY_DIR}/foms/FOM.xml\")
add_definitions(-DMIM_MODULE_PATH=\"${CMAKE_BINARY_DIR}/foms/MIM.xml\")

install(TARGETS carMastFederate DESTINATION ${CMAKE_BINARY_DIR})
install(TARGETS carScenarioFederate DESTINATION ${CMAKE_BINARY_DIR})
