helper_FIND_GTEST_LIBS()
enable_testing()

set( SOURCES 
    TestConnection.cpp
    TestXStationClient.cpp
    TestXStationClientStream.cpp
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
    Boost::json
    Xapi
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)
