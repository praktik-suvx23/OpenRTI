#add_test(
#    NAME AdminFederateTest 
#    COMMAND ${CMAKE_SOURCE_DIR}/test/admin_federate_test.sh
#    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#)
#set_tests_properties(AdminFederateTest PROPERTIES TIMEOUT 10)

#add_test(
#    NAME LaunchTest
#    COMMAND ${CMAKE_SOURCE_DIR}/test/launch_test.sh
#    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#)
add_test(
    NAME AdminFederateTest
    COMMAND ${CMAKE_SOURCE_DIR}/test/jenkinsTest/AdminTest.sh
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
add_test(
    NAME ShipTest
    COMMAND ${CMAKE_SOURCE_DIR}/test/jenkinsTest/ShipTest.sh
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
add_test(
    NAME MissileTest
    COMMAND ${CMAKE_SOURCE_DIR}/test/jenkinsTest/MissileTest.sh
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)