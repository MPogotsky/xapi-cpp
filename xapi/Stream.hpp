#pragma once

#include "Connection.hpp"
#include "Enums.hpp"

namespace xapi
{

class Stream final : protected internals::Connection
{
  public:

    Stream() = delete;
    ~Stream() = default;

    using Connection::Connection;

    boost::asio::awaitable<void> initSession(const std::string &host, const std::string &type, const std::string& streamSessionId);

    boost::asio::awaitable<void> closeSession();

    boost::asio::awaitable<Json::Value> listen();

    boost::asio::awaitable<void> getBalance();

    boost::asio::awaitable<void> stopBalance();

    boost::asio::awaitable<void> getCandles(const std::string &symbol);

    boost::asio::awaitable<void> stopCandles(const std::string &symbol);

    boost::asio::awaitable<void> getKeepAlive();

    boost::asio::awaitable<void> stopKeepAlive();

    boost::asio::awaitable<void> getNews();

    boost::asio::awaitable<void> stopNews();

    boost::asio::awaitable<void> getProfits();

    boost::asio::awaitable<void> stopProfits();

    boost::asio::awaitable<void> getTickPrices(const std::string &symbol, int minArrivalTime = 0, int maxLevel = 2);

    boost::asio::awaitable<void> stopTickPrices(const std::string &symbol);

    boost::asio::awaitable<void> getTrades();

    boost::asio::awaitable<void> stopTrades();

    boost::asio::awaitable<void> getTradeStatus();

    boost::asio::awaitable<void> stopTradeStatus();

    boost::asio::awaitable<void> ping();

private:
    std::string m_streamSessionId;
};

} // namespace xapi
