# xStation5 API C++ Library

[![Test xapi-cpp](https://github.com/MPogotsky/xapi-cpp/actions/workflows/test-xapi-cpp.yml/badge.svg)](https://github.com/MPogotsky/xapi-cpp/actions/workflows/test-xapi-cpp.yml) 
[![license](https://img.shields.io/badge/license-MIT-blue)](https://github.com/MPogotsky/xapi-cpp/LICENSE)


The xStation5 API C++ library provides a simple and easy-to-use API for interacting with the xStation5 trading platform. With this library, you can connect to the xStation5 platform, retrieve market data, and execute trades.

This library provides interface to work with [XTB](https://www.xtb.com) xStation5 accounts.

API documentation: <http://developers.xstore.pro/documentation>

### Disclaimer
This xStation5 API C++ library is not affiliated with, endorsed by, or in any way officially connected to the xStation5 trading platform or its parent company. 

Please refer to the [license](LICENSE) file (MIT License) for terms and conditions governing the use of this library, including disclaimers of warranties and liabilities.

## Requirements

Make sure to list all necessary dependencies and tools required to build and install the library.

- **C++ Compiler**: Any C++20-compliant compiler (e.g., GCC, Clang)
- **CMake**: Version 3.22 or higher
- **Boost**: Version 1.83.0 or higher
- **OpenSSL**: Version 3.0.2 or higher


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

## Usage
To use xAPI, an active account on the xStation5 trading platform is required. It could be a real account, or demo account. To create one, you can visit [xtb.com](https://www.xtb.com) 

Once your account is set up, you can leverage the Xapi library to connect to the platform and start trading.

### Cmake configuration

Xapi supports ``find_package``, simplifying the process of linking the library to your project. A typical `CMakeLists.txt` for your project might look like this:

```cmake
    cmake_minimum_required(VERSION 3.10)
    project(ExampleApp)

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    find_package(Xapi REQUIRED)

    add_executable(ExampleApp main.cpp)

    target_link_libraries(ExampleApp
        PRIVATE
        Boost::system
        Boost::json
        Xapi::Xapi
    )
```

### Example getAllSymbols
Here’s a simple example of how to use the library in your ``main.cpp``:
```cpp
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <xapi/Xapi.hpp>


boost::asio::awaitable<void> run(boost::asio::io_context &context)
{
    const boost::json::object accountCredentials = {
        {"accountId", "accountId"}, // Replace value with your account ID
        {"password", "password"}, // Replace valuer with your password
        {"accountType", "demo"} // Account type, "demo" or "real"
    };

    xapi::XStationClient user(context, accountCredentials);

    try
    {
        co_await user.login();

        auto result = co_await user.getAllSymbols();
        std::cout << boost::json::serialize(result) << std::endl;

        co_await user.logout();
    }
    catch (xapi::exception::ConnectionClosed &e)
    {
        std::cout << "Connection failed: " << e.what() << std::endl;
    }
    catch (xapi::exception::LoginFailed &e)
    {
        std::cout << "Logging failed: " << e.what() << std::endl;
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    boost::asio::io_context context;

    boost::asio::co_spawn(context, run(context), boost::asio::detached);

    context.run();
    return 0;
}
```

More examples can be found in [examples](examples/) folder.

## Runing Tests
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

## Getting Help

If you have questions, issues, or need assistance with this project, you can:

- Check the [GitHub Issues](https://github.com/MPogotsky/xapi-cpp/issues) page to report problems or check for known issues.
- Review the [Wiki](https://github.com/MPogotsky/xapi-cpp/wiki) for more detailed documentation and FAQs.

Feel free to reach out via email at matsvei.pahotski@gmail.com if you need direct assistance.


## Contributing

To contribute, please fork the repository, make your changes, and submit a pull request. All contributions are reviewed, and any useful additions are welcome!
