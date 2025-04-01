cmake_minimum_required(VERSION 3.10)
project(SharedLibraryExample)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the shared library
add_library(SharedLibrary SHARED SharedLibrary.cpp)

# Set the output name for the shared library
set_target_properties(SharedLibrary PROPERTIES OUTPUT_NAME "SharedLibrary")

# Add the executable for testingDataTransfer
add_executable(testingDataTransfer testingDataTransfer.cpp)

# Link the shared library to the executable
target_link_libraries(testingDataTransfer PRIVATE SharedLibrary)

# Install the shared library
install(TARGETS SharedLibrary
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

# Install the executable
install(TARGETS testingDataTransfer
        RUNTIME DESTINATION bin)