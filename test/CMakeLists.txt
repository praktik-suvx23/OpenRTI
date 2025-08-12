add_test(
    NAME AdminFederateTest 
    COMMAND ${CMAKE_SOURCE_DIR}/test/admin_federate_test.sh
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_tests_properties(AdminFederateTest PROPERTIES TIMEOUT 10)

add_test(
    NAME LaunchTest
    COMMAND ${CMAKE_SOURCE_DIR}/test/launch_test.sh
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)