cmake_minimum_required(VERSION 3.10)
project(MyOpenRTIProgram)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

add_executable(MyFederate MyFederate.cpp)
add_executable(MyPublisher MyPublisher.cpp)
add_executable(Robot Robot.cpp)
add_executable(MyShip MyShipFederate/MyShipFederate.cpp MyShipFederate/MyShipFederateAmbassador.cpp)
add_executable(syncFederate syncFederate/syncFederate.cpp syncFederate/syncFederateAmbassador.cpp) 

install(TARGETS MyFederate MyPublisher MyShip Robot syncFederate DESTINATION ${CMAKE_BINARY_DIR})
#install(DIRECTORY configs DESTINATION share/MyOpenRTIFederate)

# Link OpenRTI libraries (correct library names without 'lib' prefix or '.so' suffix)
target_link_libraries(MyFederate PRIVATE rti1516e fedtime1516e)
target_link_libraries(MyPublisher PRIVATE rti1516e fedtime1516e)
target_link_libraries(MyShip PRIVATE rti1516e fedtime1516e)
target_link_libraries(Robot PRIVATE rti1516e fedtime1516e)
target_link_libraries(syncFederate PRIVATE rti1516e fedtime1516e)