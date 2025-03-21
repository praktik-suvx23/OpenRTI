cmake_minimum_required(VERSION 3.10)
project(MyOpenRTIProgram)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

add_executable(Missile 
    MissileManagerFederate/MissileFederate.cpp 
    MissileManagerFederate/MissileFederateAmbassador.cpp
    include/decodePosition.cpp
)

add_executable(AdminFederate 
    AdminFederate/AdminFederate.cpp 
    AdminFederate/AdminFederateAmbassador.cpp
) 

add_executable(ShooterShip
    ShooterShipFederate/ShootShipFederate.cpp
    ShooterShipFederate/ShootShipFederateAmbassador.cpp
    include/decodePosition.cpp
)

add_executable(EnemyShip
    EnemyShip/EnemyShipFederate.cpp
    EnemyShip/EnemyShipFederateAmbassador.cpp
)

# Install the executables
install(TARGETS Missile AdminFederate ShooterShip EnemyShip DESTINATION ${CMAKE_BINARY_DIR})

# Link OpenRTI libraries (correct library names without 'lib' prefix or '.so' suffix)

target_link_libraries(Missile PRIVATE rti1516e fedtime1516e)
target_link_libraries(AdminFederate PRIVATE rti1516e fedtime1516e)
target_link_libraries(ShooterShip PRIVATE rti1516e fedtime1516e)
target_link_libraries(EnemyShip PRIVATE rti1516e fedtime1516e)
