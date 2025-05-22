add_executable(PyLink
    PyLink.cpp
    PyLinkAmbassador.cpp
    ../../include/decodePosition.cpp
    ../../include/loggingFunctions.cpp
)

target_link_libraries(PyLink PRIVATE rti1516e fedtime1516e)