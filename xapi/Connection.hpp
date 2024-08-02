#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <chrono>
#include <coroutine>
#include <jsoncpp/json/json.h>

namespace xapi
{

class Connection
{
  public:
    Connection(boost::asio::io_context &context)
        : safeMode(false), m_context(context), m_websocket(m_context), m_lastRequestTime(std::chrono::system_clock::now()),
          m_connectionEstablished(false), m_requestTimeout(200)
    {
    }

    ~Connection()
    {
    }

    boost::asio::awaitable<void> connect(const std::string &url);
    boost::asio::awaitable<void> disconnect();
    boost::asio::awaitable<Json::Value> listen();

    bool safeMode;

    boost::asio::awaitable<Json::Value> transaction(const Json::Value &command);
    boost::asio::awaitable<void> request(const Json::Value &command);

  protected:
  private:
    boost::asio::io_context &m_context;
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> m_websocket;
    std::chrono::time_point<std::chrono::system_clock> m_lastRequestTime;
    bool m_connectionEstablished;

    const std::chrono::milliseconds m_requestTimeout;
};

} // namespace xapi
