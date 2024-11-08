set (TEST_MOCK_LIB test_mocks)

set (TEST_MOCK_LIB_PUBLIC_H
    MockConnection.hpp
)

add_library(${TEST_MOCK_LIB} INTERFACE ${TEST_MOCK_LIB_PUBLIC_H})
target_link_libraries(${TEST_MOCK_LIB} INTERFACE gmock gmock_main)
