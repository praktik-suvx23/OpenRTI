add_custom_target(run_simulation
    COMMAND bash ${CMAKE_SOURCE_DIR}/src/StartUp/runProgram.sh
    DEPENDS Ship AdminFederate MissileCreator MissileHandler PyLink 
)