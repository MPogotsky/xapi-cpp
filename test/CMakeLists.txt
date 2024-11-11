enable_testing()

set( SOURCES 
    TestConnection.cpp
    TestXStationClient.cpp
    TestXStationClientStream.cpp
)

add_subdirectory(mocks)
add_executable( tests
    ${SOURCES}
)

target_include_directories(tests PRIVATE mocks)
target_link_libraries(tests PRIVATE 
    ${GTEST_LIBRARIES}
    Boost::system
    Boost::url
    Boost::json
    Xapi
    gtest
    gtest_main
    test_mocks
)

include(GoogleTest)
gtest_discover_tests(tests)
