helper_FIND_GTEST_LIBS()
enable_testing()

set( SOURCES 
    TestConnection.cpp
    TestSocket.cpp
    TestStream.cpp
)

add_executable( tests
    ${SOURCES}
)

target_compile_options(tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wno-unused-parameter
    -Wno-missing-field-initializers
)

target_link_libraries(tests PRIVATE 
    ${GTEST_LIBRARIES}
    Boost::system
    Boost::url
    OpenSSL::SSL
    OpenSSL::Crypto
    jsoncpp_lib
    Xapi
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)
