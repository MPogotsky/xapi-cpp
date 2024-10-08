#include "XStationClient.hpp"
#include "Exceptions.hpp"

namespace xapi
{

XStationClient::XStationClient(boost::asio::io_context &ioContext, const std::string &accountId,
                               const std::string &password, const std::string &accountType)
    : Socket(ioContext), m_ioContext(ioContext), m_accountId(accountId), m_password(password),
      m_accountType(accountType), m_streamSessionId("")
{
}

XStationClient::XStationClient(boost::asio::io_context &ioContext, const boost::json::object &accountCredentials)
    : XStationClient(ioContext,
                     std::string(accountCredentials.at("accountId").as_string()),
                     std::string(accountCredentials.at("password").as_string()),
                     std::string(accountCredentials.at("accountType").as_string()))
{
}

boost::asio::awaitable<void> XStationClient::login() {
    const boost::url socketUrl = boost::urls::format("wss://ws.xtb.com/{}", m_accountType);
    co_await connect(socketUrl);

    boost::json::object command = {
        {"command", "login"},
        {"arguments", {
            {"userId", m_accountId},
            {"password", m_password}
        }}
    };
    auto result = co_await request(command);
    if (result["status"].as_bool() != true)
    {
        throw exception::LoginFailed(boost::json::serialize(result));
    }

    m_streamSessionId = result["streamSessionId"].as_string();
}

boost::asio::awaitable<void> XStationClient::logout() {
    boost::json::object command = {
        {"command", "logout"}
    };
    auto result = co_await request(command);
    if (result["status"].as_bool() != true)
    {
        // If logout fails and server is not closed the connection gracefully, close it from client side.
        co_await disconnect();
    }
}

XClientStream XStationClient::getClientStream() {
    XClientStream stream(m_ioContext, m_accountType, m_streamSessionId);
    return stream;
}

} // namespace xapi
