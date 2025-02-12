cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)

add_executable(carMastFederate mastFederate/src/main.cpp mastFederate/src/mastFederate.cpp mastFederate/src/mastFedAmb.cpp)
add_executable(carScenarioFederate scenarioFederate/src/main.cpp scenarioFederate/src/scenarioFederate.cpp scenarioFederate/src/scenarioFedAmb.cpp)
#add_executable(carPrintFederate subFederate/src/subMain.cpp subFederate/src/subFederate.cpp subFederate/src/subFedAmb.cpp)
#add_executable(carFederate carFederate/src/carMain.cpp carFederate/src/carFederate.cpp carFederate/src/carFedAmb.cpp)

target_link_libraries(carMastFederate PRIVATE rti1516e fedtime1516e)
target_link_libraries(carScenarioFederate PRIVATE rti1516e fedtime1516e)
#target_link_libraries(carPrintFederate PRIVATE rti1516e fedtime1516e)
#target_link_libraries(carFederate PRIVATE rti1516e fedtime1516e)

# Ensure the required folders are copied to the build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/configCar DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/fomCar DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Define paths to use the copied files from the build directory
add_compile_definitions(
    FOM_MODULE_PATH=\"${CMAKE_CURRENT_BINARY_DIR}/fomCar/FOM.xml\"
    MIM_MODULE_PATH=\"${CMAKE_CURRENT_BINARY_DIR}/fomCar/MIM.xml\"
    SCENARIO_MODULE_PATH=\"${CMAKE_CURRENT_BINARY_DIR}/configCar/scenario/redmond.scenario\"
    CAR_MODULE_PATH=\"${CMAKE_CURRENT_BINARY_DIR}/configCar/car/turbo_terrific.car\"
)


install(TARGETS carMastFederate DESTINATION ${CMAKE_BINARY_DIR})
install(TARGETS carScenarioFederate DESTINATION ${CMAKE_BINARY_DIR})
#install(TARGETS carFederate DESTINATION ${CMAKE_BINARY_DIR})

#file(COPY ${CMAKE_SOURCE_DIR}/fom DESTINATION ${CMAKE_BINARY_DIR})
#file(COPY ${CMAKE_SOURCE_DIR}/config DESTINATION ${CMAKE_BINARY_DIR})