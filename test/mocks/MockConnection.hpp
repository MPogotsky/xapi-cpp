#pragma once

#include <gmock/gmock.h>
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <boost/url.hpp>
#include "xapi/IConnection.hpp"

class MockConnection : public xapi::internals::IConnection
{
public:
    // Mock the connect method
    MOCK_METHOD((boost::asio::awaitable<void>), connect, (const boost::url &url), (override));

    // Mock the disconnect method
    MOCK_METHOD((boost::asio::awaitable<void>), disconnect, (), (override));

    // Mock the makeRequest method
    MOCK_METHOD((boost::asio::awaitable<void>), makeRequest, (const boost::json::object &command), (override));

    // Mock the waitResponse method
    MOCK_METHOD((boost::asio::awaitable<boost::json::object>), waitResponse, (), (override));
};
