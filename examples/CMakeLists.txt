cmake_minimum_required(VERSION 3.10)
project(ExampleApp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Xapi REQUIRED)

function(add_example name)

    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} 
        PRIVATE
        Boost::system
        Boost::json
        Xapi::Xapi
    )

endfunction()

add_example(GetAllSymbols)
add_example(GetBalance)
add_example(GetCandles)
add_example(TradeTransaction)
