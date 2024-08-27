helper_FIND_GTEST_LIBS()
enable_testing()

set( SOURCES 
    TestConnection.cpp
    TestSocket.cpp
    TestStream.cpp
)

# Add your tests
add_executable( tests
    ${SOURCES}
)

target_link_libraries( tests ${GTEST_LIBRARIES} Xapi GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(tests)
