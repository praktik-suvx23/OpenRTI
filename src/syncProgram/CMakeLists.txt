cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)

add_executable(testFederate MasterFederate/src/main.cpp MasterFederate/src/mastFederate.cpp MasterFederate/src/mastFedAmb.cpp)
add_executable(testPublisher pubFederate/src/pubMain.cpp pubFederate/src/pubFederate.cpp pubFederate/src/pubFedAmb.cpp)
add_executable(testSubscriber subFederate/src/subMain.cpp subFederate/src/subFederate.cpp subFederate/src/subFedAmb.cpp)
add_executable(testSubscriber2 subFederate2/src/subMain.cpp subFederate2/src/subFederate.cpp subFederate2/src/subFedAmb.cpp)

target_link_libraries(testFederate PRIVATE rti1516e fedtime1516e)
target_link_libraries(testPublisher PRIVATE rti1516e fedtime1516e)
target_link_libraries(testSubscriber PRIVATE rti1516e fedtime1516e)
target_link_libraries(testSubscriber2 PRIVATE rti1516e fedtime1516e)

add_definitions(-DFOM_MODULE_PATH=\"${CMAKE_BINARY_DIR}/foms/FOM.xml\")
add_definitions(-DMIM_MODULE_PATH=\"${CMAKE_BINARY_DIR}/foms/MIM.xml\")

install(TARGETS testFederate DESTINATION ${CMAKE_BINARY_DIR})
install(TARGETS testPublisher DESTINATION ${CMAKE_BINARY_DIR})
install(TARGETS testSubscriber DESTINATION ${CMAKE_BINARY_DIR})
install(TARGETS testSubscriber2 DESTINATION ${CMAKE_BINARY_DIR})