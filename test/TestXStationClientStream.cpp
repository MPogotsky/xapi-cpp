#include "xapi/Exceptions.hpp"
#include "xapi/XStationClientStream.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace xapi;

class StreamTest : public testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

  public:
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


TEST_F(StreamTest, listen_exception)
{
    XStationClientStream stream(getIoContext(), "demo", "");
    
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(stream.listen()), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(StreamTest, getBalance_exception)
{
    XStationClientStream stream(getIoContext(), "demo", "");
    EXPECT_THROW(runAwaitableVoid(stream.getBalance()), exception::ConnectionClosed);
}

TEST_F(StreamTest, stopBalance_exception)
{
    XStationClientStream stream(getIoContext(), "demo", "");
    EXPECT_THROW(runAwaitableVoid(stream.stopBalance()), exception::ConnectionClosed);
}

TEST_F(StreamTest, getCandles_exception)
{
    XStationClientStream stream(getIoContext(), "demo", "");
    EXPECT_THROW(runAwaitableVoid(stream.getCandles("EURUSD")), exception::ConnectionClosed);
}

TEST_F(StreamTest, stopCandles_exception)
{
    XStationClientStream stream(getIoContext(), "demo", "");
    EXPECT_THROW(runAwaitableVoid(stream.stopCandles("EURUSD")), exception::ConnectionClosed);
}

TEST_F(StreamTest, getKeepAlive_exception)
{
    XStationClientStream stream(getIoContext(), "demo", "");
    EXPECT_THROW(runAwaitableVoid(stream.getKeepAlive()), exception::ConnectionClosed);
}

TEST_F(StreamTest, stopKeepAlive_exception)
{
    XStationClientStream stream(getIoContext(), "demo", "");
    EXPECT_THROW(runAwaitableVoid(stream.stopKeepAlive()), exception::ConnectionClosed);
}

TEST_F(StreamTest, getNews_exception)
{
    XStationClientStream stream(getIoContext(), "demo", "");
    EXPECT_THROW(runAwaitableVoid(stream.getNews()), exception::ConnectionClosed);
}

TEST_F(StreamTest, stopNews_exception)
{
    XStationClientStream stream(getIoContext(), "demo", "");
    EXPECT_THROW(runAwaitableVoid(stream.stopNews()), exception::ConnectionClosed);
}

TEST_F(StreamTest, getProfits_exception)
{
    XStationClientStream stream(getIoContext(), "demo", "");
    EXPECT_THROW(runAwaitableVoid(stream.getProfits()), exception::ConnectionClosed);
}

TEST_F(StreamTest, stopProfits_exception)
{
    XStationClientStream stream(getIoContext(), "demo", "");
    EXPECT_THROW(runAwaitableVoid(stream.stopProfits()), exception::ConnectionClosed);
}

TEST_F(StreamTest, getTickPrices_exception)
{
    XStationClientStream stream(getIoContext(), "demo", "");
    EXPECT_THROW(runAwaitableVoid(stream.getTickPrices("EURUSD", 100, 10)), exception::ConnectionClosed);
}

TEST_F(StreamTest, stopTickPrices_exception)
{
    XStationClientStream stream(getIoContext(), "demo", "");
    EXPECT_THROW(runAwaitableVoid(stream.stopTickPrices("EURUSD")), exception::ConnectionClosed);
}

TEST_F(StreamTest, getTrades_exception)
{
    XStationClientStream stream(getIoContext(), "demo", "");
    EXPECT_THROW(runAwaitableVoid(stream.getTrades()), exception::ConnectionClosed);
}

TEST_F(StreamTest, stopTrades_exception)
{
    XStationClientStream stream(getIoContext(), "demo", "");
    EXPECT_THROW(runAwaitableVoid(stream.stopTrades()), exception::ConnectionClosed);
}

TEST_F(StreamTest, getTradeStatus_exception)
{
    XStationClientStream stream(getIoContext(), "demo", "");
    EXPECT_THROW(runAwaitableVoid(stream.getTradeStatus()), exception::ConnectionClosed);
}

TEST_F(StreamTest, stopTradeStatus_exception)
{
    XStationClientStream stream(getIoContext(), "demo", "");
    EXPECT_THROW(runAwaitableVoid(stream.stopTradeStatus()), exception::ConnectionClosed);
}

TEST_F(StreamTest, ping_exception)
{
    XStationClientStream stream(getIoContext(), "demo", "");
    EXPECT_THROW(runAwaitableVoid(stream.ping()), exception::ConnectionClosed);
}
