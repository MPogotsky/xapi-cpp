cmake_minimum_required(VERSION 3.10)
project(ExampleApp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Xapi REQUIRED)

set( SOURCES 
    main.cpp
)

add_executable(ExampleApp
    ${SOURCES}
)

target_link_libraries(ExampleApp PRIVATE
    Boost::system
    Boost::url
    jsoncpp_lib
    Xapi::Xapi
)
