#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <chrono>
#include <coroutine>
#include <jsoncpp/json/json.h>

namespace xapi
{

class Connection
{
  public:
    Connection(boost::asio::io_context &ioContext)
        : safeMode(false), m_ioContext(ioContext), m_sslContext(boost::asio::ssl::context::tlsv13_client),
          m_websocket(m_ioContext, m_sslContext), m_lastRequestTime(std::chrono::system_clock::now()),
          m_connectionEstablished(false)
    {
    }

    ~Connection()
    {
    }

    boost::asio::awaitable<void> connect(const std::string &url);
    boost::asio::awaitable<void> disconnect();
    boost::asio::awaitable<Json::Value> listen();
    boost::asio::awaitable<Json::Value> transaction(const Json::Value &command);
    boost::asio::awaitable<void> request(const Json::Value &command);

    bool safeMode;

  protected:
  private:
    boost::asio::awaitable<void> establishSSLConnection(boost::asio::ip::tcp::resolver::results_type results,
                                                        const char *host);

    boost::asio::io_context &m_ioContext;
    // SSL context, stores certificates
    boost::asio::ssl::context m_sslContext;

    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::beast::tcp_stream>> m_websocket;
    std::chrono::time_point<std::chrono::system_clock> m_lastRequestTime;
    bool m_connectionEstablished;

    const std::chrono::milliseconds m_requestTimeout{200};
    const std::string m_websocketDefaultPort{"443"};
};

} // namespace xapi
