#include "xapi/Connection.hpp"
#include "xapi/Exceptions.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace xapi;

class ConnectionTest : public testing::Test
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

TEST_F(ConnectionTest, connect_exception)
{
    internals::Connection connection(getIoContext());
    EXPECT_THROW(runAwaitableVoid(connection.connect(boost::url("wss://localhost:99999"))), exception::ConnectionClosed);
}

TEST_F(ConnectionTest, disconnect_exception)
{
    internals::Connection connection(getIoContext());
    EXPECT_NO_THROW(runAwaitableVoid(connection.disconnect()));
}

TEST_F(ConnectionTest, validateAccountType_ok)
{
    internals::Connection connection(getIoContext());

    std::vector<std::string> okAccountType = {"demo", "real"};

    for (const auto &testAccountType : okAccountType)
    {
        EXPECT_NO_THROW(connection.validateAccountType(testAccountType));
    }
}

TEST_F(ConnectionTest, validateAccountType_bad)
{
    internals::Connection connection(getIoContext());

    std::vector<std::string> okAccountType = {"demoStream", "invalid", ""};

    for (const auto &testAccountType : okAccountType)
    {
        EXPECT_THROW(connection.validateAccountType(testAccountType), exception::ConnectionClosed);
    }
}

TEST_F(ConnectionTest, makeRequest_exception)
{
    internals::Connection connection(getIoContext());

    Json::Value command;
    command["command"] = "invalid";
    EXPECT_THROW(runAwaitableVoid(connection.makeRequest(command)), exception::ConnectionClosed);
}

TEST_F(ConnectionTest, waitResponse_exception)
{
    internals::Connection connection(getIoContext());

    Json::Value result;
    EXPECT_THROW(result = runAwaitable(connection.waitResponse()), exception::ConnectionClosed);
    EXPECT_TRUE(result.empty());
}
