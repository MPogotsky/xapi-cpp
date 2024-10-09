# Examples

This directory contains example implementations showcasing various scenarios where the library's features can be used.

## Compile examples
You can compile all examples by following these steps:

1. Fix your credentials in the example you want to use. In ``run`` function:

    ```cpp
    const boost::json::object accountCredentials = {
        {"accountId", "<your accountId>"},
        {"password", "<your password>"},
        {"accountType", "demo"} // If you are using real account, replace with "real"
    };

    ```

1. In examples directory create a `build` directory and navigate into it:

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
