#pragma once

#include "Connection.hpp"
#include "Enums.hpp"

namespace xapi
{

class Stream : public Connection
{
  public:
    using Connection::Connection;

    std::string streamSessionId;

    boost::asio::awaitable<Json::Value> getBalance();

    boost::asio::awaitable<Json::Value> stopBalance();

    boost::asio::awaitable<Json::Value> getCandles(const std::string &symbol);

    boost::asio::awaitable<Json::Value> stopCandles(const std::string &symbol);

    boost::asio::awaitable<Json::Value> getKeepAlive();

    boost::asio::awaitable<Json::Value> stopKeepAlive();

    boost::asio::awaitable<Json::Value> getNews();

    boost::asio::awaitable<Json::Value> stopNews();

    boost::asio::awaitable<Json::Value> getProfits();

    boost::asio::awaitable<Json::Value> stopProfits();

    boost::asio::awaitable<Json::Value> getTickPrices(const std::string &symbol, int minArrivalTime, int maxLevel);

    boost::asio::awaitable<Json::Value> stopTickPrices(const std::string &symbol);

    boost::asio::awaitable<Json::Value> getTrades();

    boost::asio::awaitable<Json::Value> stopTrades();

    boost::asio::awaitable<Json::Value> getTradeStatus();

    boost::asio::awaitable<Json::Value> stopTradeStatus();

    boost::asio::awaitable<Json::Value> ping();
};

} // namespace xapi
