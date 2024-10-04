#include "XStationClient.hpp"
#include "Exceptions.hpp"

namespace xapi
{

XStationClient::XStationClient(boost::asio::io_context &ioContext, const std::string &accountId,
                               const std::string &password, const std::string &accountType)
    : m_ioContext(ioContext), m_accountId(accountId), m_password(password), m_accountType(accountType),
      m_streamSessionId("")
{
}

XStationClient::XStationClient(boost::asio::io_context &ioContext, const boost::json::object &accountCredentials)
    : m_ioContext(ioContext), m_accountId(accountCredentials.at("accountId").as_string()),
      m_password(accountCredentials.at("password").as_string()),
      m_accountType(accountCredentials.at("accountType").as_string()), m_streamSessionId("")
{
}

boost::asio::awaitable<std::shared_ptr<xapi::Socket>> XStationClient::getSocket()
{
    auto socket = std::make_shared<xapi::Socket>(m_ioContext);
    co_await socket->initSession(m_accountType);
    m_streamSessionId = co_await socket->login(m_accountId, m_password);
    co_return socket;
}

boost::asio::awaitable<std::shared_ptr<xapi::Stream>> XStationClient::getStream() const
{
    if (m_streamSessionId.empty())
    {
        throw xapi::exception::LoginFailed("No stream session ID, get Socket to establish a session first");
    }
    auto stream = std::make_shared<xapi::Stream>(m_ioContext);
    co_await stream->initSession(m_accountType, m_streamSessionId);
    co_return stream;
}

} // namespace xapi
