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
option(BUILD_RELEASE "Release version" OFF)
if (BUILD_RELEASE)
    message("-- Build for release")
endif()
add_definitions(-DBUILD_RELEASE)

# XAPI =======================================
include(${CMAKE_SOURCE_DIR}/CMakeHelpers.cmake)

helper_FIND_BOOST_LIBS()
helper_FIND_OPENSSL_LIB()

add_subdirectory(xapi)

# TESTS ======================================
if(NOT BUILD_RELEASE)
    add_subdirectory(test)
endif()
