#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/url.hpp>
#include <chrono>
#include <coroutine>
#include <jsoncpp/json/json.h>
#include <optional>
#include <string>
#include <unordered_set>

namespace xapi
{
namespace internals
{

class Connection
{
  public:
    Connection() = delete;

    Connection(const Connection &) = delete;
    Connection &operator=(const Connection &) = delete;

    Connection(Connection &&other) = default;
    // Move assignment operatopr not supported because of boost::beast::websocket::stream
    Connection &operator=(Connection &&other) = delete;

    explicit Connection(boost::asio::io_context &ioContext);
    virtual ~Connection() = default;

    boost::asio::awaitable<void> connect(const boost::url &url);

    boost::asio::awaitable<void> disconnect();

    void validateAccountType(const std::string &accountType) const;

    boost::asio::awaitable<void> makeRequest(const Json::Value &command);
    boost::asio::awaitable<Json::Value> waitResponse();

  private:
    boost::asio::awaitable<void> establishSSLConnection(boost::asio::ip::tcp::resolver::results_type results,
                                                        const char *host);

    boost::asio::io_context &m_ioContext;
    // SSL context, stores certificates
    boost::asio::ssl::context m_sslContext;

    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::beast::tcp_stream>> m_websocket;
    std::chrono::time_point<std::chrono::system_clock> m_lastRequestTime;
    bool m_connectionEstablished;

    const std::chrono::milliseconds m_requestTimeout;
    const std::string m_websocketDefaultPort;
    const std::unordered_set<std::string> m_knownAccountTypes;
};

} // namespace internals
} // namespace xapi
