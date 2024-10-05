#include "XStationClient.hpp"
#include "Exceptions.hpp"

namespace xapi
{

XStationClient::XStationClient(boost::asio::io_context &ioContext, const std::string &accountId,
                               const std::string &password, const std::string &accountType)
    : socket(nullptr), stream(nullptr), m_ioContext(ioContext), m_accountId(accountId), m_password(password),
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

boost::asio::awaitable<void> XStationClient::setupSocketConnection()
{
    socket = std::make_unique<xapi::Socket>(m_ioContext);
    co_await socket->initSession(m_accountType);
    m_streamSessionId = co_await socket->login(m_accountId, m_password);
}

boost::asio::awaitable<void> XStationClient::setupStreamConnection()
{
    if (m_streamSessionId.empty())
    {
        throw exception::LoginFailed("No stream session ID, get Socket to establish a session first");
    }
    stream = std::make_unique<xapi::Stream>(m_ioContext);
    co_await stream->initSession(m_accountType, m_streamSessionId);
}

boost::asio::awaitable<void> XStationClient::closeSocketConnection()
{
    if (socket == nullptr)
    {
        co_return;
    }
    auto result = co_await socket->logout();
    if (result["status"].as_bool() != true)
    {
        // If logout fails and server is not closed the connection gracefully, close it from client side.
        co_await socket->closeSession();
    }
}

boost::asio::awaitable<void> XStationClient::closeStreamConnection()
{
    if (stream == nullptr)
    {
        co_return;
    }
    co_await stream->closeSession();
}

} // namespace xapi
