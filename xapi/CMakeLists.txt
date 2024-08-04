set(XAPI_PUBLIC_H
    Enums.hpp
    Exceptions.hpp
    Connection.hpp
    Socket.hpp
    Stream.hpp
    Xapi.hpp
)

set(XAPI_SOURCES
    ${XAPI_PUBLIC_H}
    Connection.cpp
    Socket.cpp
    Stream.cpp
    Xapi.cpp
)

add_library(xapi STATIC ${XAPI_SOURCES})
target_compile_options(xapi PRIVATE -Wall -Werror -Wpedantic -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -O2)
target_link_libraries( xapi PRIVATE
    Boost::system
    Boost::url
    OpenSSL::SSL
    OpenSSL::Crypto
    jsoncpp_lib
)
target_include_directories(xapi PUBLIC ${CMAKE_SOURCE_DIR})
