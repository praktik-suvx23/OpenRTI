cmake_minimum_required(VERSION 3.15...3.20)
project(CarSimC LANGUAGES CXX)


# Dependencies

find_package(Threads REQUIRED)


# Compiler options

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_compile_options(/D_HAS_AUTO_PTR_ETC=1)
    set(LIB_DIR "lib/win_vc140")
    set(FEDERATE_LIBS "librti1516e64d" "libfedtime1516e64d")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_compile_options(-Wno-deprecated -Wno-deprecated-declarations)
    add_link_options(-Wl,-unresolved-symbols=ignore-in-shared-libs)
    set(LIB_DIR "lib/lin_gcc73")
    set(FEDERATE_LIBS "rti1516e64" "fedtime1516e64" "stdc++fs")
else()
    add_compile_options(-Wno-deprecated-declarations)
    set(LIB_DIR "lib/mac_clang12")
    set(FEDERATE_LIBS "rti1516e" "fedtime1516e")
endif()


# Federate executable

add_executable(CarSimC
    src/Car.cpp
    src/CarFactory.cpp
    src/CarProperties.cpp
    src/CarSimC.cpp
    src/Controller.cpp
    src/FuelType.cpp
    src/HLAfuelType.cpp
    src/HLAmodule.cpp
    src/HLAposition.cpp
    src/LatLongHelper.cpp
    src/NoScenarioState.cpp
    src/Position.cpp
    src/Properties.cpp
    src/RunningState.cpp
    src/Scenario.cpp
    src/ScenarioLoadedState.cpp
    src/Simulator.cpp
    src/SimulatorConfig.cpp
    src/State.cpp
    src/StringUtils.cpp
)

target_compile_features(CarSimC
    PRIVATE
        cxx_std_14
)

target_compile_definitions(CarSimC
    PRIVATE
        RTI_DISABLE_WARNINGS=1
)

target_include_directories(CarSimC
    SYSTEM PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_directories(CarSimC
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/${LIB_DIR}
)

target_link_libraries(CarSimC
    Threads::Threads
    ${FEDERATE_LIBS}
)

install(
    TARGETS
        CarSimC
    DESTINATION
        bin
)
