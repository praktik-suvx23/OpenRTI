cmake_minimum_required(VERSION 3.10)
project(SharedLibraryExampleWithVector)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the shared library
add_library(sharedLibrary SHARED sharedLibrary.cpp)

# Set the output name for the shared library
set_target_properties(sharedLibrary PROPERTIES OUTPUT_NAME "sharedLibrary")

# Add the executable for sendData
add_executable(sendData sendData.cpp)

# Link the shared library to the executable
target_link_libraries(sendData PRIVATE sharedLibrary)

# Install the shared library
install(TARGETS sharedLibrary
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

# Install the executable
install(TARGETS sendData
        RUNTIME DESTINATION bin)