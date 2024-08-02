#include "xapi/Connection.hpp"
#include "xapi/Exceptions.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace xapi;

class ConnectionTest : public testing::Test
{
  protected:
    void SetUp() override {

    }

    void TearDown() override {
    }

  public:

    boost::asio::io_context &getIoContext()
    {
        return m_context;
    }

    template <typename Awaitable> auto run_awaitable(Awaitable &&awaitable)
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

    template <typename Awaitable> auto run_awaitable_void(Awaitable &&awaitable)
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

TEST_F(ConnectionTest, ConnectException)
{
    Connection connection(getIoContext());
    EXPECT_THROW(run_awaitable_void(connection.connect("ws://localhost:99999")), exception::ConnectionClosed);
}

TEST_F(ConnectionTest, DisconnectException)
{
    Connection connection(getIoContext());
    EXPECT_NO_THROW(run_awaitable_void(connection.disconnect()));
}

TEST_F(ConnectionTest, ListenException)
{
    Connection connection(getIoContext());
    EXPECT_THROW(run_awaitable(connection.listen()), exception::ConnectionClosed);
}

TEST(Coonn, Listen) {
    boost::asio::io_context testContext;

    auto testBody = [&]() -> boost::asio::awaitable<void>
    {
        Connection connection(testContext);

        co_await connection.connect("ws://localhost:8765/test");

        auto result = co_await connection.listen();

        std::cout << "-----------------------" << result["message"].asString() << std::endl;
        EXPECT_TRUE(result["message"].asString() == "This is a test JSON message");
    };

    boost::asio::co_spawn(testContext, testBody, boost::asio::detached);

    testContext.run();
}

TEST(Coonn, ListenContigiosly) {
    boost::asio::io_context testContext;

    auto testBody = [&]() -> boost::asio::awaitable<void>
    {
        Connection connection(testContext);

        co_await connection.connect("ws://localhost:8765/test/stream");

        int counter = 0;
        while(counter < 5) {
            auto result = co_await connection.listen();
            std::cout << "-----------------------" << result["message"].asString() << std::endl;

            EXPECT_TRUE(result["message"].asString() == "This is a test JSON message");

            counter++;
        }
    };

    boost::asio::co_spawn(testContext, testBody, boost::asio::detached);

    testContext.run();
}

TEST(Coonn, RequestOnce)
{
    boost::asio::io_context testContext;
    auto testBody = [&]() -> boost::asio::awaitable<void> {
        Connection connection(testContext);
        co_await connection.connect("ws://localhost:8765/test/request");

        Json::Value json_value;
        json_value["message"] = "===========================";

        EXPECT_NO_THROW(co_await connection.request(json_value));
    };

    boost::asio::co_spawn(testContext, testBody, boost::asio::detached);

    testContext.run();
}


TEST(Coonn, RequestSequence) {
    boost::asio::io_context testContext;

    auto testBody = [&]() -> boost::asio::awaitable<void>
    {
        Connection connection(testContext);

        co_await connection.connect("ws://localhost:8765/test/request");

        Json::Value json_value;
        json_value["message"] = "++++++++";

        for(int i = 0; i < 10; i++) {
            EXPECT_NO_THROW( co_await connection.request(json_value) );
        }
    };

    boost::asio::co_spawn(testContext, testBody, boost::asio::detached);

    testContext.run();
}

TEST(Coonn, TransactionSequence) {
    boost::asio::io_context testContext;

    auto testBody = [&]() -> boost::asio::awaitable<void>
    {
        Connection connection(testContext);

        co_await connection.connect("ws://localhost:8765/test/transaction");

        Json::Value json_value;
        json_value["message"] = "++++++++";

        auto result = co_await connection.transaction(json_value);
        // for(int i = 0; i < 10; i++) {
        //     EXPECT_NO_THROW( co_await connection.request(json_value) );
        // }
        std::cout << "-----------------------" << result["message"].asString() << std::endl;
        EXPECT_TRUE(result["message"].asString() == "This is a test JSON message");
    };

    boost::asio::co_spawn(testContext, testBody, boost::asio::detached);

    testContext.run();
}
