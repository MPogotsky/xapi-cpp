#include "MockConnection.hpp"
#include "xapi/Exceptions.hpp"
#include "xapi/XStationClient.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <string>

namespace xapi
{

class XStationClientTest : public ::testing::Test
{
  protected:
    std::unique_ptr<XStationClient> client;
    void SetUp() override
    {
        client = std::make_unique<XStationClient>(m_context, "test", "test", "demo");
        auto connection = std::make_unique<MockConnection>();
        EXPECT_NO_THROW(client->m_connection = std::move(connection));
    }

    void TearDown() override
    {
        client.reset();
    }

    MockConnection &getMockedConnection()
    {
        return *dynamic_cast<MockConnection *>(client->m_connection.get());
    }

    boost::asio::io_context &getIoContext()
    {
        return m_context;
    }

    template <typename Awaitable> auto runAwaitable(Awaitable &&awaitable)
    {
        std::exception_ptr eptr;
        using result_type = typename Awaitable::value_type;
        result_type result;

        boost::asio::co_spawn(
            m_context,
            [&]() -> boost::asio::awaitable<void> {
                try
                {
                    result = co_await std::forward<Awaitable>(awaitable);
                }
                catch (...)
                {
                    eptr = std::current_exception();
                }
            },
            boost::asio::detached);

        m_context.run();

        if (eptr)
        {
            std::rethrow_exception(eptr);
        }

        return result;
    }

    template <typename Awaitable> auto runAwaitableVoid(Awaitable &&awaitable)
    {
        std::exception_ptr eptr;
        boost::asio::co_spawn(
            m_context,
            [&]() -> boost::asio::awaitable<void> {
                try
                {
                    co_await std::forward<Awaitable>(awaitable);
                }
                catch (...)
                {
                    eptr = std::current_exception();
                }
            },
            boost::asio::detached);

        m_context.run();

        if (eptr)
        {
            std::rethrow_exception(eptr);
        }
    }

  private:
    boost::asio::io_context m_context;
};

TEST(XStationClientConstructorTest, string_constructor)
{
    boost::asio::io_context ioContext;
    EXPECT_NO_THROW(XStationClient client(ioContext, "test", "test", "demo"));
}

TEST(XStationClientConstructorTest, json_constructor)
{
    boost::asio::io_context ioContext;
    const boost::json::object testAccountCredentials = {
        {"accountId", "test"},
        {"password", "test"},
        {"accountType", "demo"}
    };

    EXPECT_NO_THROW(XStationClient client(ioContext, testAccountCredentials););
}

TEST_F(XStationClientTest, login_invalid_account_type)
{
    const boost::json::object accountCredentials = {
        {"accountId", "test"},
        {"password", "test"},
        {"accountType", "asdasda"}
    };

    XStationClient client(getIoContext(), accountCredentials);
    EXPECT_THROW(runAwaitableVoid(client.login()), exception::LoginFailed);
    EXPECT_TRUE(client.m_streamSessionId.empty());
}

TEST_F(XStationClientTest, login_account_credentials_null)
{    
    const boost::json::object accountCredentials = {
        {"accountId", ""},
        {"password", ""},
        {"accountType", "test"}
    };

    std::unique_ptr<XStationClient> client;
    EXPECT_NO_THROW(client = std::make_unique<XStationClient>(getIoContext(), accountCredentials));
    EXPECT_THROW(runAwaitableVoid(client->login()), exception::LoginFailed);
    EXPECT_TRUE(client->m_streamSessionId.empty());
}

TEST_F(XStationClientTest, login_invalid_return_from_the_server)
{
    EXPECT_CALL(getMockedConnection(), connect(testing::_))
        .WillOnce([](const boost::url &url) -> boost::asio::awaitable<void> { co_return; });

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &command) -> boost::asio::awaitable<void> { co_return; });

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([]() -> boost::asio::awaitable<boost::json::object> {
            boost::json::object response = {{"broken", true}};
            co_return response;
        });

    EXPECT_THROW(runAwaitableVoid(client->login()), exception::LoginFailed);
    EXPECT_TRUE(client->m_streamSessionId.empty());
}

TEST_F(XStationClientTest, login_ok)
{
    const boost::json::object serverResponse = {{"status", true}, {"streamSessionId", "test"}};

    EXPECT_CALL(getMockedConnection(), connect(testing::_))
        .WillOnce([](const boost::url &url) -> boost::asio::awaitable<void> { co_return; });

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &command) -> boost::asio::awaitable<void> { co_return; });

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    EXPECT_NO_THROW(runAwaitableVoid(client->login()));
    EXPECT_TRUE(client->m_streamSessionId == "test");
}

TEST_F(XStationClientTest, logout_exception)
{
    const boost::json::object expectedCommand = {{"command", "logout"}};

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                throw exception::ConnectionClosed("Exception");
                co_return; 
            }
        );

    EXPECT_THROW(runAwaitableVoid(client->logout()), exception::ConnectionClosed);
}

TEST_F(XStationClientTest, logout_ok)
{
    const boost::json::object expectedCommand = {{"command", "logout"}};
    const boost::json::object serverResponse = {{"status", true}};

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });
    
    EXPECT_CALL(getMockedConnection(), disconnect())
        .WillOnce([]() -> boost::asio::awaitable<void> {
            co_return;
        });
    
    EXPECT_NO_THROW(runAwaitableVoid(client->logout()));
}

TEST_F(XStationClientTest, getAllSymbols_exception)
{
    const boost::json::object expectedCommand = {{"command", "getAllSymbols"}};

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                throw exception::ConnectionClosed("Exception");
                co_return; 
            }
        );

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->getAllSymbols()), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getAllSymbols_ok)
{
    const boost::json::object expectedCommand = {{"command", "getAllSymbols"}};
    const boost::json::object serverResponse = {{"status", true}, {"returnData", "test"}};

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->getAllSymbols()));
    EXPECT_FALSE(result.empty());
}

TEST_F(XStationClientTest, getCalendar_exception)
{
    const boost::json::object expectedCommand = {{"command", "getCalendar"}};

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                throw exception::ConnectionClosed("Exception");
                co_return;
            }
        );

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->getCalendar()), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getCalendar_ok)
{
    const boost::json::object expectedCommand = {{"command", "getCalendar"}};
    const boost::json::object serverResponse = {{"status", true}, {"returnData", "test"}};

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return;
            }
        );
    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->getCalendar()));
    EXPECT_FALSE(result.empty());
}

TEST_F(XStationClientTest, getServerTime_exception)
{
    const boost::json::object expectedCommand = {{"command", "getServerTime"}};

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                throw exception::ConnectionClosed("Exception");
                co_return; 
            }
        );

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->getServerTime()), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getServerTime_ok)
{
    const boost::json::object expectedCommand = {{"command", "getServerTime"}};
    const boost::json::object serverTimeResponse = {{"status", true}, {"returnData", "test"}};
    
    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverTimeResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverTimeResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->getServerTime()));
    EXPECT_EQ(result["returnData"].as_string(), "test");
}

TEST_F(XStationClientTest, getTickPrices_exception)
{
    const boost::json::object expectedCommand = {
        {"command", "getTickPrices"},
        {"arguments", {
            {"symbols", boost::json::array({"symbol1", "symbol2"})},
            {"timestamp", 1234567890},
            {"level", 2}
        }}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                throw exception::ConnectionClosed("Exception");
                co_return; 
            }
        );

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->getTickPrices({"symbol1", "symbol2"}, 1234567890, 2)), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getTickPrices_ok)
{
    const boost::json::object expectedCommand = {
        {"command", "getTickPrices"},
        {"arguments", {
            {"symbols", boost::json::array({"symbol1", "symbol2"})},
            {"timestamp", 1234567890},
            {"level", 2}
        }}
    };
    const boost::json::object serverResponse = {{"status", true}, {"returnData", "tick prices data"}};

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->getTickPrices({"symbol1", "symbol2"}, 1234567890, 2)));
    EXPECT_EQ(result["returnData"].as_string(), "tick prices data");
}

TEST_F(XStationClientTest, tradeTransaction_safeMode)
{
    client->setSafeMode(true);

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->tradeTransaction("EURUSD", TradeCmd::BUY, TradeType::OPEN, 1.1000f,
                                                                  1.0f, 0.0f, 0.0f, 123456, 0, 0, "Test comment")));

    boost::json::object expectedResult;
    expectedResult["status"] = false;
    expectedResult["errorCode"] = "N/A";
    expectedResult["errorDescr"] = "Trading is disabled when safe=True";

    EXPECT_EQ(result, expectedResult);
}

TEST_F(XStationClientTest, tradeTransaction_exception)
{
    const boost::json::object expectedCommand = {
        {"command", "tradeTransaction"},
        {"arguments", {
            {"tradeTransInfo", {
                {"cmd", 0},
                {"customComment", "test"},
                {"expiration", 0},
                {"offset", 0},
                {"order", 12345},
                {"price", 0.0},
                {"sl", 0.0},
                {"symbol", "testSymbol"},
                {"tp", 0.0},
                {"type", 0},
                {"volume", 0.0}
            }}
        }}
    };

    client->setSafeMode(false);

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand) << "Expected command: " << boost::json::serialize(expectedCommand) << "\n Actual command: " <<  boost::json::serialize(command);
                throw exception::ConnectionClosed("Exception");
                co_return; 
            }
        );

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->tradeTransaction("testSymbol", TradeCmd::BUY, TradeType::OPEN, \
                                                                0.0f, 0.0f, 0.0f, 0.0f, 12345, 0.0f, 0.0f, "test")), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, tradeTransaction_ok)
{
    const boost::json::object expectedCommand = {
        {"command", "tradeTransaction"},
        {"arguments", {
            {"tradeTransInfo", {
                {"cmd", 0},
                {"customComment", "test"},
                {"expiration", 0},
                {"offset", 0},
                {"order", 12345},
                {"price", 0.0},
                {"sl", 0.0},
                {"symbol", "testSymbol"},
                {"tp", 0.0},
                {"type", 0},
                {"volume", 0.0}
            }}
        }}
    };
    const boost::json::object serverResponse = {{"status", true}, {"returnData", "trade result"}};

    client->setSafeMode(false);

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand) << "Expected command: " << boost::json::serialize(expectedCommand) << "\n Actual command: " <<  boost::json::serialize(command);
                co_return; 
            }
        );

    
    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->tradeTransaction("testSymbol", TradeCmd::BUY, TradeType::OPEN, \
                                                                0.0f, 0.0f, 0.0f, 0.0f, 12345, 0.0f, 0.0f, "test")));
    EXPECT_EQ(result["returnData"].as_string(), "trade result");
}

TEST_F(XStationClientTest, getChartLastRequest_ok)
{
    const std::string symbol = "AAPL";
    const int64_t start = 1633046400000;
    const PeriodCode period = PeriodCode::PERIOD_H1;

    const boost::json::object expectedCommand = {
        {"command", "getChartLastRequest"},
        {"arguments", {
            {"info", {
                {"period", static_cast<int>(period)},
                {"start", start},
                {"symbol", symbol}
            }}
        }}
    };

    const boost::json::object serverResponse = {
        {"status", "success"},
        {"data", {{"chartData", {}}}}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->getChartLastRequest(symbol, start, period)));
    EXPECT_EQ(result["status"].as_string(), "success");
}

TEST_F(XStationClientTest, getChartLastRequest_exception)
{
    const std::string symbol = "AAPL";
    const int64_t start = 1633046400000;
    const PeriodCode period = PeriodCode::PERIOD_H1;

    const boost::json::object expectedCommand = {
        {"command", "getChartLastRequest"},
        {"arguments", {
            {"info", {
                {"period", static_cast<int>(period)},
                {"start", start},
                {"symbol", symbol}
            }}
        }}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([]() -> boost::asio::awaitable<boost::json::object> {
            throw exception::ConnectionClosed("Exception");
        });

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->getChartLastRequest(symbol, start, period)), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getChartRangeRequest_ok)
{
    const std::string symbol = "GOOG";
    const int64_t start = 1633046400000;
    const int64_t end = 1633132800000;
    const PeriodCode period = PeriodCode::PERIOD_D1;
    const int ticks = 10;

    const boost::json::object expectedCommand = {
        {"command", "getChartRangeRequest"},
        {"arguments", {
            {"info", {
                {"end", end},
                {"period", static_cast<int>(period)},
                {"start", start},
                {"symbol", symbol},
                {"ticks", ticks}
            }}
        }}
    };

    const boost::json::object serverResponse = {
        {"status", "success"},
        {"data", {{"chartData", {}}}}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->getChartRangeRequest(symbol, start, end, period, ticks)));
    EXPECT_EQ(result["status"].as_string(), "success");
}

TEST_F(XStationClientTest, getChartRangeRequest_exception)
{
    const std::string symbol = "GOOG";
    const int64_t start = 1633046400000;
    const int64_t end = 1633132800000;
    const PeriodCode period = PeriodCode::PERIOD_D1;
    const int ticks = 10;

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &command) -> boost::asio::awaitable<void> {
            co_return;
        });

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([]() -> boost::asio::awaitable<boost::json::object> {
            throw exception::ConnectionClosed("Exception");
        });

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->getChartRangeRequest(symbol, start, end, period, ticks)), exception::ConnectionClosed);
}

TEST_F(XStationClientTest, getCommissionDef_ok)
{
    const std::string symbol = "GOOG";
    const float volume = 100.0f;

    const boost::json::object expectedCommand = {
        {"command", "getCommissionDef"},
        {"arguments", {
            {"symbol", symbol},
            {"volume", volume}
        }}
    };

    const boost::json::object serverResponse = {
        {"status", "success"},
        {"commission", 5.0f}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->getCommissionDef(symbol, volume)));
    EXPECT_EQ(result["status"].as_string(), "success");
    EXPECT_EQ(result["commission"].as_double(), 5.0f);
}

TEST_F(XStationClientTest, getCommissionDef_exception)
{
    const std::string symbol = "GOOG";
    const float volume = 100.0f;

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &command) -> boost::asio::awaitable<void> {
            co_return;
        });

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([]() -> boost::asio::awaitable<boost::json::object> {
            throw exception::ConnectionClosed("Exception");
        });

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->getCommissionDef(symbol, volume)), exception::ConnectionClosed);
}

TEST_F(XStationClientTest, getCurrentUserData_ok)
{
    const boost::json::object expectedCommand = {
        {"command", "getCurrentUserData"}
    };

    const boost::json::object serverResponse = {
        {"status", "success"},
        {"userData", {{"name", "John Doe"}, {"account", "12345"}}}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->getCurrentUserData()));
    EXPECT_FALSE(result.empty());
}

TEST_F(XStationClientTest, getCurrentUserData_exception)
{
    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &command) -> boost::asio::awaitable<void> {
            co_return;
        });

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([]() -> boost::asio::awaitable<boost::json::object> {
            throw exception::ConnectionClosed("Exception");
        });

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->getCurrentUserData()), exception::ConnectionClosed);
}

TEST_F(XStationClientTest, getIbsHistory_ok)
{
    const std::int64_t start = 1633046400000;
    const std::int64_t end = 1633132800000;

    const boost::json::object expectedCommand = {
        {"command", "getIbsHistory"},
        {"arguments", {
            {"start", start},
            {"end", end}
        }}
    };

    const boost::json::object serverResponse = {
        {"status", "success"},
        {"historyData", {{"dataPoint1", 123}, {"dataPoint2", 456}}}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->getIbsHistory(start, end)));
    EXPECT_FALSE(result.empty());
}

TEST_F(XStationClientTest, getIbsHistory_exception)
{
    const std::int64_t start = 1633046400000;
    const std::int64_t end = 1633132800000;

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &command) -> boost::asio::awaitable<void> {
            co_return;
        });

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([]() -> boost::asio::awaitable<boost::json::object> {
            throw exception::ConnectionClosed("Exception");
        });

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->getIbsHistory(start, end)), exception::ConnectionClosed);
}

TEST_F(XStationClientTest, getMarginLevel_ok)
{
    const boost::json::object expectedCommand = {
        {"command", "getMarginLevel"}
    };

    const boost::json::object serverResponse = {
        {"status", "success"},
        {"marginLevel", 50.0}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->getMarginLevel()));
    EXPECT_FALSE(result.empty());
}

TEST_F(XStationClientTest, getMarginLevel_exception)
{
    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &command) -> boost::asio::awaitable<void> {
            co_return;
        });

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([]() -> boost::asio::awaitable<boost::json::object> {
            throw exception::ConnectionClosed("Exception");
        });

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->getMarginLevel()), exception::ConnectionClosed);
}

TEST_F(XStationClientTest, getMarginTrade_ok)
{
    const std::string symbol = "GOOG";
    const float volume = 100.0f;

    const boost::json::object expectedCommand = {
        {"command", "getMarginTrade"},
        {"arguments", {
            {"symbol", symbol},
            {"volume", volume}
        }}
    };

    const boost::json::object serverResponse = {
        {"status", "success"},
        {"margin", 2000.0}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->getMarginTrade(symbol, volume)));
    EXPECT_FALSE(result.empty());
}

TEST_F(XStationClientTest, getMarginTrade_exception)
{
    const std::string symbol = "GOOG";
    const float volume = 100.0f;

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &command) -> boost::asio::awaitable<void> {
            co_return;
        });

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([]() -> boost::asio::awaitable<boost::json::object> {
            throw exception::ConnectionClosed("Exception");
        });

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->getMarginTrade(symbol, volume)), exception::ConnectionClosed);
}

TEST_F(XStationClientTest, getNews_ok)
{
    const std::int64_t start = 1633046400000;
    const std::int64_t end = 1633132800000;

    const boost::json::object expectedCommand = {
        {"command", "getNews"},
        {"arguments", {
            {"start", start},
            {"end", end}
        }}
    };

    const boost::json::object serverResponse = {
        {"status", "success"},
        {"news", {{"headline", "Breaking news!"}}}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->getNews(start, end)));
    EXPECT_FALSE(result.empty());
}

TEST_F(XStationClientTest, getNews_exception)
{
    const std::int64_t start = 1633046400000;
    const std::int64_t end = 1633132800000;

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &command) -> boost::asio::awaitable<void> {
            co_return;
        });

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([]() -> boost::asio::awaitable<boost::json::object> {
            throw exception::ConnectionClosed("Exception");
        });

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->getNews(start, end)), exception::ConnectionClosed);
}

TEST_F(XStationClientTest, getProfitCalculation_ok)
{
    const std::string symbol = "GOOG";
    const int cmd = 1;
    const float openPrice = 1500.0f;
    const float closePrice = 1550.0f;
    const float volume = 100.0f;

    const boost::json::object expectedCommand = {
        {"command", "getProfitCalculation"},
        {"arguments", {
            {"symbol", symbol},
            {"cmd", cmd},
            {"openPrice", openPrice},
            {"closePrice", closePrice},
            {"volume", volume}
        }}
    };

    const boost::json::object serverResponse = {
        {"status", "success"},
        {"profit", 5000.0}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->getProfitCalculation(symbol, cmd, openPrice, closePrice, volume)));
    EXPECT_FALSE(result.empty());
}

TEST_F(XStationClientTest, getProfitCalculation_exception)
{
    const std::string symbol = "GOOG";
    const int cmd = 1;
    const float openPrice = 1500.0f;
    const float closePrice = 1550.0f;
    const float volume = 100.0f;

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &command) -> boost::asio::awaitable<void> {
            co_return;
        });

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([]() -> boost::asio::awaitable<boost::json::object> {
            throw exception::ConnectionClosed("Exception");
        });

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->getProfitCalculation(symbol, cmd, openPrice, closePrice, volume)), exception::ConnectionClosed);
}

TEST_F(XStationClientTest, getStepRules_ok)
{
    const boost::json::object expectedCommand = {
        {"command", "getStepRules"}
    };

    const boost::json::object serverResponse = {
        {"status", "success"},
        {"data", {{"stepRules", {}}}}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->getStepRules()));
    EXPECT_FALSE(result.empty());
}

TEST_F(XStationClientTest, getStepRules_exception)
{
    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &command) -> boost::asio::awaitable<void> {
            co_return;
        });

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([]() -> boost::asio::awaitable<boost::json::object> {
            throw exception::ConnectionClosed("Exception");
        });

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->getStepRules()), exception::ConnectionClosed);
}

TEST_F(XStationClientTest, getSymbol_ok)
{
    const std::string symbol = "AAPL";
    const boost::json::object expectedCommand = {
        {"command", "getSymbol"},
        {"arguments", {
            {"symbol", symbol}
        }}
    };

    const boost::json::object serverResponse = {
        {"status", "success"},
        {"symbol", symbol}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->getSymbol(symbol)));
    EXPECT_FALSE(result.empty());
}

TEST_F(XStationClientTest, getSymbol_exception)
{
    const std::string symbol = "AAPL";

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &command) -> boost::asio::awaitable<void> {
            co_return;
        });

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([]() -> boost::asio::awaitable<boost::json::object> {
            throw exception::ConnectionClosed("Exception");
        });

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->getSymbol(symbol)), exception::ConnectionClosed);
}

TEST_F(XStationClientTest, getTradeRecords_ok)
{
    const std::vector<int> orders = {12345, 67890};
    const boost::json::object expectedCommand = {
        {"command", "getTradeRecords"},
        {"arguments", {
            {"orders", boost::json::array(orders.begin(), orders.end())}
        }}
    };

    const boost::json::object serverResponse = {
        {"status", "success"},
        {"data", {{"tradeRecords", {}}}}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->getTradeRecords(orders)));
    EXPECT_FALSE(result.empty());
}

TEST_F(XStationClientTest, getTradeRecords_exception)
{
    const std::vector<int> orders = {12345, 67890};

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &command) -> boost::asio::awaitable<void> {
            co_return;
        });

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([]() -> boost::asio::awaitable<boost::json::object> {
            throw exception::ConnectionClosed("Exception");
        });

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->getTradeRecords(orders)), exception::ConnectionClosed);
}

TEST_F(XStationClientTest, getTrades_ok)
{
    const bool openedOnly = true;
    const boost::json::object expectedCommand = {
        {"command", "getTrades"},
        {"arguments", {
            {"openedOnly", openedOnly}
        }}
    };

    const boost::json::object serverResponse = {
        {"status", "success"},
        {"data", {{"trades", {}}}}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->getTrades(openedOnly)));
    EXPECT_FALSE(result.empty());
}

TEST_F(XStationClientTest, getTrades_exception)
{
    const bool openedOnly = true;

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &command) -> boost::asio::awaitable<void> {
            co_return;
        });

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([]() -> boost::asio::awaitable<boost::json::object> {
            throw exception::ConnectionClosed("Exception");
        });

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->getTrades(openedOnly)), exception::ConnectionClosed);
}

TEST_F(XStationClientTest, getTradesHistory_ok)
{
    const std::int64_t start = 1633046400000;
    const std::int64_t end = 1633132800000;
    const boost::json::object expectedCommand = {
        {"command", "getTradesHistory"},
        {"arguments", {
            {"start", start},
            {"end", end}
        }}
    };

    const boost::json::object serverResponse = {
        {"status", "success"},
        {"data", {{"tradeHistory", {}}}}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->getTradesHistory(start, end)));
    EXPECT_FALSE(result.empty());
}

TEST_F(XStationClientTest, getTradesHistory_exception)
{
    const std::int64_t start = 1633046400000;
    const std::int64_t end = 1633132800000;

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &command) -> boost::asio::awaitable<void> {
            co_return;
        });

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([]() -> boost::asio::awaitable<boost::json::object> {
            throw exception::ConnectionClosed("Exception");
        });

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->getTradesHistory(start, end)), exception::ConnectionClosed);
}

TEST_F(XStationClientTest, getTradingHours_ok)
{
    const std::vector<std::string> symbols = {"AAPL", "GOOG"};
    const boost::json::object expectedCommand = {
        {"command", "getTradingHours"},
        {"arguments", {
            {"symbols", boost::json::array(symbols.begin(), symbols.end())}
        }}
    };

    const boost::json::object serverResponse = {
        {"status", "success"},
        {"data", {{"tradingHours", {}}}}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->getTradingHours(symbols)));
    EXPECT_FALSE(result.empty());
}

TEST_F(XStationClientTest, getTradingHours_exception)
{
    const std::vector<std::string> symbols = {"AAPL", "GOOG"};

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &command) -> boost::asio::awaitable<void> {
            co_return;
        });

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([]() -> boost::asio::awaitable<boost::json::object> {
            throw exception::ConnectionClosed("Exception");
        });

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->getTradingHours(symbols)), exception::ConnectionClosed);
}

TEST_F(XStationClientTest, getVersion_ok)
{
    const boost::json::object expectedCommand = {
        {"command", "getVersion"}
    };

    const boost::json::object serverResponse = {
        {"status", "success"},
        {"data", {{"version", "1.0.0"}}}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->getVersion()));
    EXPECT_FALSE(result.empty());
}

TEST_F(XStationClientTest, getVersion_exception)
{
    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &command) -> boost::asio::awaitable<void> {
            co_return;
        });

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([]() -> boost::asio::awaitable<boost::json::object> {
            throw exception::ConnectionClosed("Exception");
        });

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->getVersion()), exception::ConnectionClosed);
}

TEST_F(XStationClientTest, tradeTransactionStatus_ok)
{
    const int order = 12345;
    const boost::json::object expectedCommand = {
        {"command", "tradeTransactionStatus"},
        {"arguments", {
            {"order", order}
        }}
    };

    const boost::json::object serverResponse = {
        {"status", "success"},
        {"data", {{"status", "completed"}}}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
                EXPECT_EQ(command, expectedCommand);
                co_return; 
            }
        );

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client->tradeTransactionStatus(order)));
    EXPECT_FALSE(result.empty());
}

TEST_F(XStationClientTest, tradeTransactionStatus_exception)
{
    const int order = 12345;

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &command) -> boost::asio::awaitable<void> {
            co_return;
        });

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([]() -> boost::asio::awaitable<boost::json::object> {
            throw exception::ConnectionClosed("Exception");
        });

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client->tradeTransactionStatus(order)), exception::ConnectionClosed);
}

} // namespace xapi
