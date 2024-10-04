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

TEST_F(XStationClientTest, XStationClient_string_constructor)
{
    EXPECT_NO_THROW(auto client = std::make_unique<XStationClient>(getIoContext(), "test", "test", "demo"));
}

TEST_F(XStationClientTest, XStationClient_json_constructor)
{
    const boost::json::object accountCredentials = {
        {"accountId", "test"},
        {"password", "test"},
        {"accountType", "demo"}
    };

    EXPECT_NO_THROW(auto client = std::make_unique<XStationClient>(getIoContext(), accountCredentials));
}

TEST_F(XStationClientTest, XStationClient_getSocket_exception)
{
    const boost::json::object accountCredentials = {
        {"accountId", "test"},
        {"password", "test"},
        {"accountType", "demo"}
    };

    std::unique_ptr<XStationClient> client;
    EXPECT_NO_THROW(client = std::make_unique<XStationClient>(getIoContext(), accountCredentials));
    EXPECT_THROW(runAwaitable(client->getSocket()), exception::LoginFailed);
}

TEST_F(XStationClientTest, XStationClient_getStream_exception)
{
    const boost::json::object accountCredentials = {
        {"accountId", "test"},
        {"password", "test"},
        {"accountType", "demo"}
    };

    std::unique_ptr<XStationClient> client;
    EXPECT_NO_THROW(client = std::make_unique<XStationClient>(getIoContext(), accountCredentials));
    EXPECT_THROW(runAwaitable(client->getStream()), exception::LoginFailed);
}
