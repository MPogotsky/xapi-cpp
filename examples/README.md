# Introduction

Xapi-cpp is a C++ library that provides a simple and easy-to-use API for interacting with the xStation5 trading platform.
Built on top of the Boost libraries, it leverages Boost’s powerful and widely adopted capabilities, particularly for network-based applications. Boost provides robust, high-performance abstractions that simplify the development of complex asynchronous operations, making it an ideal choice for ensuring a reliable and efficient connection to the xStation5 platform.

## Key Concepts

### 1. Asynchronous Programming with Coroutines
The library makes heavy use of C++20 coroutines and Boost.Asio to handle asynchronous operations. Each API call is suspended until the operation completes, allowing the program to remain responsive and efficient.

While C++20 provides the coroutine syntax (``co_await``, ``co_return``, ``co_yield``), it doesn’t define a complete framework for asynchronous execution or I/O. Libraries like Boost.Asio provide the necessary abstractions and asynchronous I/O handling that C++20 coroutines on their own do not.

### 2. Secure WebSocket Communication
The xapi uses secure WebSockets (WebSockets over SSL) for communication, ensuring that the data transmitted between the client and server is encrypted. This provides a secure channel for sensitive information, such as account credentials and balance data.

### 3. Separate WebSocket Connections
The xapi establishes separate WebSocket connections for **xapi::Socket** and **xapi::Stream**. This separation aims to minimize congestion on a single connection especially if one connection is handling high-frequency data (like streaming).

 
# Getting Started
Let`s break down some important steps to start using xStation in your project

## Awailable operations
### 1. Socket Operations
The **xapi::Socket** object is used to Request-Reply commands. Full list of commands is in [xApi Retrieving Trading Data](http://developers.xstore.pro/documentation/2.5.0#retrieving-trading-data)

### 2. Stream Operations
The **xapi::Stream** object is used to manage real-time streaming data, such as balance updates, tick prises and so on. Full list of streaming operations is in [xApi Available Streaming Commands](http://developers.xstore.pro/documentation/2.5.0#available-streaming-commands)

Please note, that to use streaming operations, you first should get streaming id from **xapi::Socket** login() command.

## Cmake configuration
Xapi supports ``find_package``, simplifying the process of linking the library to your project. A typical CMake configuration might look like this:

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


## Simple example
The simplest example is [RetrieveStreamSessionId](RetrieveStreamSessionId.cpp), which demonstrates how to connect to an xStation server using Xapi. The program initializes a session, logs in, and retrieves a stream session ID.

Additional examples can be found in the .cpp files within the examples folder.

## Compile examples
You can compile all examples by following these steps:

1. Fix your credentials in the example you want to use. In ``run`` function:

    ```cpp
    const std::string accountId = "<your accountId>";
    const std::string password = "<your password>";

    // If you are using real account, replace with "real"
    const std::string accountType = "demo";
    ```

1. Create a `build` directory and navigate into it:

    ```bash
    mkdir build
    cd build
    ```

2. Run CMake to configure and build the project:

    ```bash
    cmake ..
    cmake --build . 
    ```

3. Binaries will be stored in ``bin`` directory. Run example app that you want (f.e. RetrieveStreamSessionId)
    ```bash
    ./bin/RetrieveStreamSessionId
    ```
