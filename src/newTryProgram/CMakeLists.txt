cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)

add_executable(testFederate MasterFederate/src/main.cpp MasterFederate/src/mastFederate.cpp MasterFederate/src/mastFedAmb.cpp)
add_executable(testPublisher pubFederate/src/pubFederate.cpp)

target_link_libraries(testFederate PRIVATE rti1516e fedtime1516e)
target_link_libraries(testPublisher PRIVATE rti1516e fedtime1516e)

install(TARGETS testFederate DESTINATION ${CMAKE_BINARY_DIR})
install(TARGETS testPublisher DESTINATION ${CMAKE_BINARY_DIR})