# Helper macro to find boost libraries
macro(helper_FIND_BOOST_LIBS)

    find_package(Boost ${BOOST_VERSION_REQUIRED} REQUIRED COMPONENTS system url json)

    if(NOT Boost_FOUND)
        message(FATAL_ERROR "Boost not found. Please install Boost version ${BOOST_VERSION_REQUIRED} or set Boost_DIR variable.")
    elseif(NOT Boost_VERSION VERSION_GREATER_EQUAL ${BOOST_VERSION_REQUIRED})
        message(FATAL_ERROR "Boost version ${BOOST_VERSION_REQUIRED} or higher is required. Found version ${Boost_VERSION}.")
    else()
        include_directories(${Boost_INCLUDE_DIRS})
    endif()

endmacro()

#Helper macro to find openSSL library
macro(helper_FIND_OPENSSL_LIB)
    set(OPENSSL_USE_STATIC_LIBS TRUE)
    find_package(OpenSSL REQUIRED)
    if(NOT OpenSSL_FOUND)
        message(FATAL_ERROR "OpenSSL not found. Please check if openSSL is installed")
    else()
        include_directories(${OPENSSL_INCLUDE_DIR})
    endif()
endmacro()

# Helper macro to find google test library
macro(helper_FIND_GTEST_LIBS)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/heads/main.zip
    )
    # Download and make gtest/gmock available
    FetchContent_MakeAvailable(googletest)
endmacro()
