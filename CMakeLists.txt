cmake_minimum_required(VERSION 3.22)

project(xapi
  VERSION 1.0.0
  DESCRIPTION "C++ implementation of the xStation API module"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BOOST_VERSION_REQUIRED "1.83.0")

# OPTIONS ====================================
option(BUILD_TESTS "Build tests" ON)
if (BUILD_TESTS)
    add_definitions(-DBUILD_TESTS)
endif()

# XAPI =======================================
include(${CMAKE_SOURCE_DIR}/CMakeHelpers.cmake)

helper_FIND_BOOST_LIBS()
helper_FIND_JSONCPP_LIB()

add_subdirectory(xapi)
target_link_libraries(xapi PRIVATE Boost::system Boost::url jsoncpp_lib)

# TESTS ======================================
if(BUILD_TESTS)
    add_subdirectory(test)
endif()
