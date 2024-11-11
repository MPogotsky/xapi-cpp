cmake_minimum_required(VERSION 3.22)

project(xapi
  VERSION 1.0.0
  DESCRIPTION "C++ implementation of the xStation API module"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BOOST_VERSION_REQUIRED "1.83.0")

include(${CMAKE_SOURCE_DIR}/CMakeHelpers.cmake)

# OPTIONS ====================================
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# XAPI =======================================
helper_FIND_BOOST_LIBS()
helper_FIND_OPENSSL_LIB()
add_subdirectory(xapi)

# TESTS ======================================
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    helper_FIND_GTEST_LIBS()
    add_definitions(-DENABLE_TEST)
    add_subdirectory(test)
endif()
