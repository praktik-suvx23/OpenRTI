# OpenRTI Project

## Prerequisites

ensure you have the following tools and dependencies installed:
- CMake
- Make
- GCC or Clang
- Git

## How to Get Started

1. **Download the OpenRTI repository**  
    Open the OpenRTI (https://github.com/onox/OpenRTI) repo and do the following steps in a Linux environment:

2. **Create a build directory**  
    ```sh
    mkdir build
    cd build
    ```

3. **Generate build files using CMake**  
    ```sh
    cmake ..
    ```

4. **Build the project**  
    ```sh
    make
    ```

5. **Install the project**  
    ```sh
    sudo make install
    ```

6. **Copy header files to the system include directory**  
    ```sh
    sudo cp -r ../include/* /usr/local/include/
    ```

7. **Copy library files to the system library directory**  
    ```sh
    sudo cp -r lib/* /usr/local/lib/
    ```

These steps are required to access the OpenRTI library and files in your own project.

When done, continue to the `README.md` in the `src/myProgram` folder.
