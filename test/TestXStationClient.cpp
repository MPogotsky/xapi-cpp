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
    std::shared_ptr<XStationClient> client;
    EXPECT_NO_THROW(client = std::make_unique<XStationClient>(getIoContext(), "test", "test", "demo"));
    EXPECT_TRUE(client->socket == nullptr);
    EXPECT_TRUE(client->stream == nullptr);
}

TEST_F(XStationClientTest, XStationClient_json_constructor)
{
    const boost::json::object accountCredentials = {
        {"accountId", "test"},
        {"password", "test"},
        {"accountType", "demo"}
    };

    std::shared_ptr<XStationClient> client;
    EXPECT_NO_THROW(client = std::make_unique<XStationClient>(getIoContext(), accountCredentials));
    EXPECT_TRUE(client->socket == nullptr);
    EXPECT_TRUE(client->stream == nullptr);
}

TEST_F(XStationClientTest, XStationClient_setupSocketConnection_exception)
{
    const boost::json::object accountCredentials = {
        {"accountId", "test"},
        {"password", "test"},
        {"accountType", "demo"}
    };

    std::unique_ptr<XStationClient> client;
    EXPECT_NO_THROW(client = std::make_unique<XStationClient>(getIoContext(), accountCredentials));
    EXPECT_THROW(runAwaitableVoid(client->setupSocketConnection()), exception::LoginFailed);
    EXPECT_TRUE(client->socket != nullptr);
    EXPECT_TRUE(client->stream == nullptr);
}

TEST_F(XStationClientTest, XStationClient_setupStreamConnection_exception)
{
    const boost::json::object accountCredentials = {
        {"accountId", "test"},
        {"password", "test"},
        {"accountType", "demo"}
    };

    std::unique_ptr<XStationClient> client;
    EXPECT_NO_THROW(client = std::make_unique<XStationClient>(getIoContext(), accountCredentials));
    EXPECT_THROW(runAwaitableVoid(client->setupStreamConnection()), exception::LoginFailed);
    EXPECT_TRUE(client->socket == nullptr);
    // Socket is initialized, but not logged in, so no stream session ID is available, that is why no stream is created.
    EXPECT_TRUE(client->stream == nullptr);
}

TEST_F(XStationClientTest, XStationClient_closeSocketConnection)
{
    const boost::json::object accountCredentials = {
        {"accountId", "test"},
        {"password", "test"},
        {"accountType", "demo"}
    };

    std::unique_ptr<XStationClient> client;
    EXPECT_NO_THROW(client = std::make_unique<XStationClient>(getIoContext(), accountCredentials));
    EXPECT_NO_THROW(runAwaitableVoid(client->closeSocketConnection()));
}

TEST_F(XStationClientTest, XStationClient_closeStreamConnection)
{
    const boost::json::object accountCredentials = {
        {"accountId", "test"},
        {"password", "test"},
        {"accountType", "demo"}
    };

    std::unique_ptr<XStationClient> client;
    EXPECT_NO_THROW(client = std::make_unique<XStationClient>(getIoContext(), accountCredentials));
    EXPECT_NO_THROW(runAwaitableVoid(client->closeStreamConnection()));
}
