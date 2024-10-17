#include "xapi/Exceptions.hpp"
#include "xapi/XStationClient.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace xapi;

class XStationClientTest : public testing::Test
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

    template <typename Awaitable> 
    auto runAwaitable(Awaitable &&awaitable)
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

    template <typename Awaitable> 
    auto runAwaitableVoid(Awaitable &&awaitable)
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

const boost::json::object testAccountCredentials = {
    {"accountId", "test"},
    {"password", "test"},
    {"accountType", "demo"}
};


TEST_F(XStationClientTest, XStationClient_string_constructor)
{
    EXPECT_NO_THROW(XStationClient client(getIoContext(), "test", "test", "demo"));
}

TEST_F(XStationClientTest, XStationClient_json_constructor)
{
    EXPECT_NO_THROW(XStationClient client(getIoContext(), testAccountCredentials););
}

TEST_F(XStationClientTest, XStationClient_login_exception_invalid_account_type)
{
    const boost::json::object accountCredentials = {
        {"accountId", "test"},
        {"password", "test"},
        {"accountType", "asdasda"}
    };

    XStationClient client(getIoContext(), accountCredentials);
    EXPECT_THROW(runAwaitableVoid(client.login()), exception::LoginFailed);
}

TEST_F(XStationClientTest, XStationClient_logout_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    EXPECT_THROW(runAwaitableVoid(client.logout()), exception::ConnectionClosed);
}

TEST_F(XStationClientTest, getAllSymbols_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.getAllSymbols()), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getCalendar_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.getCalendar()), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getChartLastRequest_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.getChartLastRequest("EURUSD", 1625097600, PeriodCode::PERIOD_M1)),
                 exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getChartRangeRequest_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(
        result = runAwaitable(client.getChartRangeRequest("EURUSD", 1625097600, 1625184000, PeriodCode::PERIOD_M1, 10)),
        exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getCommissionDef_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.getCommissionDef("EURUSD", 1.0f)), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getCurrentUserData_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.getCurrentUserData()), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getIbsHistory_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.getIbsHistory(1625097600, 1625184000)), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getMarginLevel_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.getMarginLevel()), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getMarginTrade_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.getMarginTrade("EURUSD", 1.0f)), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getNews_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.getNews(1625097600, 1625184000)), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getProfitCalculation_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.getProfitCalculation("EURUSD", 0, 1.1000f, 1.1050f, 1.0f)),
                 exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getServerTime_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.getServerTime()), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getStepRules_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.getStepRules()), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getSymbol_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.getSymbol("EURUSD")), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getTickPrices_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.getTickPrices({"EURUSD", "GBPUSD"}, 1625097600, 1)),
                 exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getTradeRecords_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.getTradeRecords({123456, 789012})), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getTrades_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.getTrades(true)), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getTradesHistory_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.getTradesHistory(1625097600, 1625184000)), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getTradingHours_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.getTradingHours({"EURUSD", "GBPUSD"})), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, getVersion_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.getVersion()), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, ping_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.ping()), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, tradeTransaction_safeMode_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_NO_THROW(result = runAwaitable(client.tradeTransaction("EURUSD", TradeCmd::BUY, TradeType::OPEN, 1.1000f,
                                                                  1.0f, 0.0f, 0.0f, 123456, 0, 0, "Test comment")));

    boost::json::object expectedResult;
    expectedResult["status"] = false;
    expectedResult["errorCode"] = "N/A";
    expectedResult["errorDescr"] = "Trading is disabled when safe=True";

    EXPECT_EQ(result, expectedResult);
}

TEST_F(XStationClientTest, tradeTransaction_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    client.setSafeMode(false);

    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.tradeTransaction("EURUSD", TradeCmd::BUY, TradeType::OPEN, 1.1000f, 1.0f,
                                                               0.0f, 0.0f, 123456, 0, 0, "Test comment")),
                 exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}

TEST_F(XStationClientTest, tradeTransactionStatus_exception)
{
    XStationClient client(getIoContext(), testAccountCredentials);
    boost::json::object result;
    EXPECT_THROW(result = runAwaitable(client.tradeTransactionStatus(123456)), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}
