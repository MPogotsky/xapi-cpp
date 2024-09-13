# xStation5 API C++ Library

[![Test xapi-cpp](https://github.com/MPogotsky/xapi-cpp/actions/workflows/test-xapi-cpp.yml/badge.svg)](https://github.com/MPogotsky/xapi-cpp/actions/workflows/test-xapi-cpp.yml) 
[![license](https://img.shields.io/badge/license-MIT-blue)](https://github.com/MPogotsky/xapi-cpp/LICENSE)


The xStation5 API C++ library provides a simple and easy-to-use API for interacting with the xStation5 trading platform. With this library, you can connect to the xStation5 platform, retrieve market data, and execute trades.

This library may be used for [XTB](https://www.xtb.com) xStation5 accounts.

API documentation: <http://developers.xstore.pro/documentation>

## Disclaimer

This xStation5 API C++ library is not affiliated with, endorsed by, or in any way officially connected to the xStation5 trading platform or its parent company. The library is provided as-is and is not guaranteed to be suitable for any particular purpose. The use of this library is at your own risk, and the author(s) of this library will not be liable for any damages arising from the use or misuse of this library. Please refer to the license file for more information.


## Requirements

Make sure to list all necessary dependencies and tools required to build and install the library.

- **C++ Compiler**: Any C++20-compliant compiler (e.g., GCC, Clang)
- **CMake**: Version 3.22 or higher
- **Boost**: Version 1.83.0 or higher
- **OpenSSL**: Version 3.0.2 or higher

If you have the required dependencies installed, you can skip the next section and navigate straight to [Build & Install](#build--install).

### Installing Dependencies

To install the necessary dependencies, follow these steps:

1. Update the package list:

```bash
sudo apt update
```

2. Install the required packages:

```bash
sudo apt install libssl-dev libgtest-dev libboost-system1.83-dev libboost-url1.83-dev libboost-json1.83-dev
```

These commands will update the package list and install the necessary packages for building and installing the library.

#### for Ubuntu 22.04

Boost 1.83.0 package is not available by default on Ubuntu 22.04.
You can install using PPA. Follow these steps:

1. Run the following command to add the PPA repository:

    ```bash
    sudo add-apt-repository ppa:mhier/libboost-latest
    ```

2. Update the package list:

    ```bash
    sudo apt update
    ```

3. Install the required packages:

    ```bash
    sudo apt install libssl-dev libgtest-dev libboost-system1.83-dev libboost-url1.83-dev libboost-json1.83-dev
    ```

### Build & Install
Step-by-step guide to build the project using CMake.

1. Clone the repository and navigate to the `xapi-cpp` directory:

    ```bash
    git clone https://github.com/MPogotsky/xapi-cpp.git
    cd xapi-cpp
    ```

2. Create a `build` directory and navigate into it:

    ```bash
    mkdir build
    cd build
    ```

3. Run CMake to configure and build the project:

    ```bash
    cmake ..
    cmake --build . 
    ```

4. Run CMake command to install library:

    ```bash
    cmake --install . 
    ```

### Runing Tests
To build the tests, follow these steps:

1. Navigate to the `build` directory.
2. Run the following commands to configure and build the tests using CMake:

    ```bash
    cmake -DBUILD_TESTS=ON ..
    cmake --build .
    ```

    Once the build process is complete, you will have the tests ready to run.

    Use command below to run unit tests:

    ```bash
    test/tests
    ```
