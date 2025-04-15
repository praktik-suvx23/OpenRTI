add_executable(PyLink
    PyLink.cpp
    PyLinkAmbassador.cpp
    ../../include/decodePosition.cpp
)

target_link_libraries(PyLink PRIVATE rti1516e fedtime1516e)