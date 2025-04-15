add_executable(PyLink
    PyLink.cpp
    PyLink.h
)

target_link_libraries(PyLink PRIVATE rti1516e fedtime1516e)