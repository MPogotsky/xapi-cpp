#include "MockConnection.hpp"
#include "xapi/Exceptions.hpp"
#include "xapi/XStationClientStream.hpp"
#include <gtest/gtest.h>
#include <string>

namespace xapi
{

class XStationClientStreamTest : public ::testing::Test
{
  protected:
    std::unique_ptr<XStationClientStream> stream;
    void SetUp() override
    {
        stream = std::make_unique<XStationClientStream>(m_context, "demo", "testStreamSessionId");
        auto connection = std::make_unique<MockConnection>();
        EXPECT_NO_THROW(stream->m_connection = std::move(connection));
    }

    void TearDown() override
    {
        stream.reset();
    }

    MockConnection &getMockedConnection()
    {
        return *dynamic_cast<MockConnection *>(stream->m_connection.get());
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

TEST(XStationClientStreamConstructorTest, constructor)
{
    boost::asio::io_context ioContext;
    EXPECT_NO_THROW(XStationClientStream stream(ioContext, "demo", "streamSessionId"));
}

TEST_F(XStationClientStreamTest, open_ok)
{
    EXPECT_CALL(getMockedConnection(), connect(testing::_))
        .WillOnce([](const boost::url &url) -> boost::asio::awaitable<void> { 
            EXPECT_EQ(url.buffer(), "wss://ws.xtb.com/demoStream");
            co_return; 
        });

    EXPECT_NO_THROW(runAwaitableVoid(stream->open()));
}

TEST_F(XStationClientStreamTest, open_exception)
{
    EXPECT_CALL(getMockedConnection(), connect(::testing::_))
        .WillOnce([](const boost::url &url) -> boost::asio::awaitable<void> {
            throw exception::ConnectionClosed("Exception");
        });

    EXPECT_THROW(runAwaitableVoid(stream->open()), exception::ConnectionClosed);
}

TEST_F(XStationClientStreamTest, close_ok)
{
    EXPECT_CALL(getMockedConnection(), disconnect())
        .WillOnce([]() -> boost::asio::awaitable<void> {
            co_return;
        });

    EXPECT_NO_THROW(runAwaitableVoid(stream->close()));
}

TEST_F(XStationClientStreamTest, close_exception)
{
    EXPECT_CALL(getMockedConnection(), disconnect())
        .WillOnce([]() -> boost::asio::awaitable<void> {
            throw exception::ConnectionClosed("Exception");
        });

    EXPECT_THROW(runAwaitableVoid(stream->close()), exception::ConnectionClosed);
}

TEST_F(XStationClientStreamTest, listen_ok)
{
    const boost::json::object serverResponse = {
        {"status", true}
    };

    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([&serverResponse]() -> boost::asio::awaitable<boost::json::object> {
            co_return serverResponse;
        });

    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(stream->listen()));
    EXPECT_EQ(result, serverResponse);
}

TEST_F(XStationClientStreamTest, listen_exception)
{
    EXPECT_CALL(getMockedConnection(), waitResponse())
        .WillOnce([]() -> boost::asio::awaitable<boost::json::object> {
            throw exception::ConnectionClosed("Exception");
        });

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(stream->listen()), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientStreamTest, getBalance_ok)
{
    const boost::json::object expectedCommand = {
        {"command", "getBalance"},
        {"streamSessionId", "testStreamSessionId"}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
            EXPECT_EQ(command, expectedCommand);
            co_return;
        });

    EXPECT_NO_THROW(runAwaitableVoid(stream->getBalance()));
}

TEST_F(XStationClientStreamTest, getBalance_exception)
{
    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &) -> boost::asio::awaitable<void> {
            throw exception::ConnectionClosed("Exception");
        });

    EXPECT_THROW(runAwaitableVoid(stream->getBalance()), exception::ConnectionClosed);
}

TEST_F(XStationClientStreamTest, stopBalance_ok)
{
    const boost::json::object expectedCommand = {
        {"command", "stopBalance"}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
            EXPECT_EQ(command, expectedCommand);
            co_return;
        });

    EXPECT_NO_THROW(runAwaitableVoid(stream->stopBalance()));
}

TEST_F(XStationClientStreamTest, stopBalance_exception)
{
    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &) -> boost::asio::awaitable<void> {
            throw exception::ConnectionClosed("Exception");
        });

    EXPECT_THROW(runAwaitableVoid(stream->stopBalance()), exception::ConnectionClosed);
}

TEST_F(XStationClientStreamTest, getCandles_ok)
{
    const std::string symbol = "EURUSD";
    const boost::json::object expectedCommand = {
        {"command", "getCandles"},
        {"streamSessionId", "testStreamSessionId"},
        {"symbol", symbol}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
            EXPECT_EQ(command, expectedCommand);
            co_return;
        });

    EXPECT_NO_THROW(runAwaitableVoid(stream->getCandles(symbol)));
}

TEST_F(XStationClientStreamTest, getCandles_exception)
{
    const std::string symbol = "EURUSD";

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &) -> boost::asio::awaitable<void> {
            throw exception::ConnectionClosed("Exception");
        });

    EXPECT_THROW(runAwaitableVoid(stream->getCandles(symbol)), exception::ConnectionClosed);
}

TEST_F(XStationClientStreamTest, stopCandles_ok)
{
    const std::string symbol = "EURUSD";
    const boost::json::object expectedCommand = {
        {"command", "stopCandles"},
        {"symbol", symbol}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
            EXPECT_EQ(command, expectedCommand);
            co_return;
        });

    EXPECT_NO_THROW(runAwaitableVoid(stream->stopCandles(symbol)));
}

TEST_F(XStationClientStreamTest, stopCandles_exception)
{
    const std::string symbol = "EURUSD";

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &) -> boost::asio::awaitable<void> {
            throw exception::ConnectionClosed("Exception");
        });

    EXPECT_THROW(runAwaitableVoid(stream->stopCandles(symbol)), exception::ConnectionClosed);
}

TEST_F(XStationClientStreamTest, getKeepAlive_ok)
{
    const boost::json::object expectedCommand = {
        {"command", "getKeepAlive"},
        {"streamSessionId", "testStreamSessionId"}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
            EXPECT_EQ(command, expectedCommand);
            co_return;
        });

    EXPECT_NO_THROW(runAwaitableVoid(stream->getKeepAlive()));
}

TEST_F(XStationClientStreamTest, getKeepAlive_exception)
{
    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &) -> boost::asio::awaitable<void> {
            throw exception::ConnectionClosed("Exception");
        });

    EXPECT_THROW(runAwaitableVoid(stream->getKeepAlive()), exception::ConnectionClosed);
}

TEST_F(XStationClientStreamTest, stopKeepAlive_ok)
{
    const boost::json::object expectedCommand = {
        {"command", "stopKeepAlive"}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
            EXPECT_EQ(command, expectedCommand);
            co_return;
        });

    EXPECT_NO_THROW(runAwaitableVoid(stream->stopKeepAlive()));
}

TEST_F(XStationClientStreamTest, stopKeepAlive_exception)
{
    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &) -> boost::asio::awaitable<void> {
            throw exception::ConnectionClosed("Exception");
        });

    EXPECT_THROW(runAwaitableVoid(stream->stopKeepAlive()), exception::ConnectionClosed);
}

TEST_F(XStationClientStreamTest, getNews_ok)
{
    const boost::json::object expectedCommand = {
        {"command", "getNews"},
        {"streamSessionId", "testStreamSessionId"}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
            EXPECT_EQ(command, expectedCommand);
            co_return;
        });

    EXPECT_NO_THROW(runAwaitableVoid(stream->getNews()));
}

TEST_F(XStationClientStreamTest, getNews_exception)
{
    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &) -> boost::asio::awaitable<void> {
            throw exception::ConnectionClosed("Exception");
        });

    EXPECT_THROW(runAwaitableVoid(stream->getNews()), exception::ConnectionClosed);
}

TEST_F(XStationClientStreamTest, stopNews_ok)
{
    const boost::json::object expectedCommand = {
        {"command", "stopNews"}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
            EXPECT_EQ(command, expectedCommand);
            co_return;
        });

    EXPECT_NO_THROW(runAwaitableVoid(stream->stopNews()));
}

TEST_F(XStationClientStreamTest, stopNews_exception)
{
    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &) -> boost::asio::awaitable<void> {
            throw exception::ConnectionClosed("Exception");
        });

    EXPECT_THROW(runAwaitableVoid(stream->stopNews()), exception::ConnectionClosed);
}

TEST_F(XStationClientStreamTest, getProfits_ok)
{
    const boost::json::object expectedCommand = {
        {"command", "getProfits"},
        {"streamSessionId", "testStreamSessionId"}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
            EXPECT_EQ(command, expectedCommand);
            co_return;
        });

    EXPECT_NO_THROW(runAwaitableVoid(stream->getProfits()));
}

TEST_F(XStationClientStreamTest, getProfits_exception)
{
    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &) -> boost::asio::awaitable<void> {
            throw exception::ConnectionClosed("Exception");
        });

    EXPECT_THROW(runAwaitableVoid(stream->getProfits()), exception::ConnectionClosed);
}

TEST_F(XStationClientStreamTest, stopProfits_ok)
{
    const boost::json::object expectedCommand = {
        {"command", "stopProfits"}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
            EXPECT_EQ(command, expectedCommand);
            co_return;
        });

    EXPECT_NO_THROW(runAwaitableVoid(stream->stopProfits()));
}

TEST_F(XStationClientStreamTest, stopProfits_exception)
{
    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &) -> boost::asio::awaitable<void> {
            throw exception::ConnectionClosed("Exception");
        });

    EXPECT_THROW(runAwaitableVoid(stream->stopProfits()), exception::ConnectionClosed);
}

TEST_F(XStationClientStreamTest, getTickPrices_ok)
{
    const std::string symbol = "EURUSD";
    const int minArrivalTime = 500;
    const int maxLevel = 2;

    const boost::json::object expectedCommand = {
        {"command", "getTickPrices"},
        {"streamSessionId", "testStreamSessionId"},
        {"symbol", symbol},
        {"minArrivalTime", minArrivalTime},
        {"maxLevel", maxLevel}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
            EXPECT_EQ(command, expectedCommand);
            co_return;
        });

    EXPECT_NO_THROW(runAwaitableVoid(stream->getTickPrices(symbol, minArrivalTime, maxLevel)));
}

TEST_F(XStationClientStreamTest, getTickPrices_exception)
{
    const std::string symbol = "EURUSD";
    const int minArrivalTime = 500;
    const int maxLevel = 2;

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &) -> boost::asio::awaitable<void> {
            throw exception::ConnectionClosed("Exception");
        });

    EXPECT_THROW(runAwaitableVoid(stream->getTickPrices(symbol, minArrivalTime, maxLevel)), exception::ConnectionClosed);
}

TEST_F(XStationClientStreamTest, stopTickPrices_ok)
{
    const std::string symbol = "EURUSD";

    const boost::json::object expectedCommand = {
        {"command", "stopTickPrices"},
        {"symbol", symbol}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
            EXPECT_EQ(command, expectedCommand);
            co_return;
        });

    EXPECT_NO_THROW(runAwaitableVoid(stream->stopTickPrices(symbol)));
}

TEST_F(XStationClientStreamTest, stopTickPrices_exception)
{
    const std::string symbol = "EURUSD";

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &) -> boost::asio::awaitable<void> {
            throw exception::ConnectionClosed("Exception");
        });

    EXPECT_THROW(runAwaitableVoid(stream->stopTickPrices(symbol)), exception::ConnectionClosed);
}

TEST_F(XStationClientStreamTest, getTrades_ok)
{
    const boost::json::object expectedCommand = {
        {"command", "getTrades"},
        {"streamSessionId", "testStreamSessionId"}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
            EXPECT_EQ(command, expectedCommand);
            co_return;
        });

    EXPECT_NO_THROW(runAwaitableVoid(stream->getTrades()));
}

TEST_F(XStationClientStreamTest, getTrades_exception)
{
    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &) -> boost::asio::awaitable<void> {
            throw exception::ConnectionClosed("Exception");
        });

    EXPECT_THROW(runAwaitableVoid(stream->getTrades()), exception::ConnectionClosed);
}

TEST_F(XStationClientStreamTest, stopTrades_ok)
{
    const boost::json::object expectedCommand = {
        {"command", "stopTrades"}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
            EXPECT_EQ(command, expectedCommand);
            co_return;
        });

    EXPECT_NO_THROW(runAwaitableVoid(stream->stopTrades()));
}

TEST_F(XStationClientStreamTest, stopTrades_exception)
{
    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &) -> boost::asio::awaitable<void> {
            throw exception::ConnectionClosed("Exception");
        });

    EXPECT_THROW(runAwaitableVoid(stream->stopTrades()), exception::ConnectionClosed);
}

TEST_F(XStationClientStreamTest, getTradeStatus_ok)
{
    const boost::json::object expectedCommand = {
        {"command", "getTradeStatus"},
        {"streamSessionId", "testStreamSessionId"}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
            EXPECT_EQ(command, expectedCommand);
            co_return;
        });

    EXPECT_NO_THROW(runAwaitableVoid(stream->getTradeStatus()));
}

TEST_F(XStationClientStreamTest, getTradeStatus_exception)
{
    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &) -> boost::asio::awaitable<void> {
            throw exception::ConnectionClosed("Exception");
        });

    EXPECT_THROW(runAwaitableVoid(stream->getTradeStatus()), exception::ConnectionClosed);
}

TEST_F(XStationClientStreamTest, stopTradeStatus_ok)
{
    const boost::json::object expectedCommand = {
        {"command", "stopTradeStatus"}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
            EXPECT_EQ(command, expectedCommand);
            co_return;
        });

    EXPECT_NO_THROW(runAwaitableVoid(stream->stopTradeStatus()));
}

TEST_F(XStationClientStreamTest, stopTradeStatus_exception)
{
    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &) -> boost::asio::awaitable<void> {
            throw exception::ConnectionClosed("Exception");
        });

    EXPECT_THROW(runAwaitableVoid(stream->stopTradeStatus()), exception::ConnectionClosed);
}

TEST_F(XStationClientStreamTest, ping_ok)
{
    const boost::json::object expectedCommand = {
        {"command", "ping"},
        {"streamSessionId", "testStreamSessionId"}
    };

    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([&expectedCommand](const boost::json::object &command) -> boost::asio::awaitable<void> {
            EXPECT_EQ(command, expectedCommand);
            co_return;
        });

    EXPECT_NO_THROW(runAwaitableVoid(stream->ping()));
}

TEST_F(XStationClientStreamTest, ping_exception)
{
    EXPECT_CALL(getMockedConnection(), makeRequest(testing::_))
        .WillOnce([](const boost::json::object &) -> boost::asio::awaitable<void> {
            throw exception::ConnectionClosed("Exception");
        });

    EXPECT_THROW(runAwaitableVoid(stream->ping()), exception::ConnectionClosed);
}

} // namespace xapi
