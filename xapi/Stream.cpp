#include "Stream.hpp"

namespace xapi
{

boost::asio::awaitable<Json::Value> Stream::getBalance()
{
    Json::Value command;
    command["command"] = "getBalance";
    command["streamSessionId"] = streamSessionId;
    auto result = co_await transaction(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Stream::stopBalance()
{
    Json::Value command;
    command["command"] = "stopBalance";
    auto result = co_await transaction(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Stream::getCandles(const std::string &symbol)
{
    Json::Value command;
    command["command"] = "getCandles";
    command["streamSessionId"] = streamSessionId;
    command["symbol"] = symbol;
    auto result = co_await transaction(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Stream::stopCandles(const std::string &symbol)
{
    Json::Value command;
    command["command"] = "stopCandles";
    command["symbol"] = symbol;
    auto result = co_await transaction(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Stream::getKeepAlive()
{
    Json::Value command;
    command["command"] = "getKeepAlive";
    command["streamSessionId"] = streamSessionId;
    auto result = co_await transaction(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Stream::stopKeepAlive()
{
    Json::Value command;
    command["command"] = "stopKeepAlive";
    auto result = co_await transaction(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Stream::getNews()
{
    Json::Value command;
    command["command"] = "getNews";
    command["streamSessionId"] = streamSessionId;
    auto result = co_await transaction(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Stream::stopNews()
{
    Json::Value command;
    command["command"] = "stopNews";
    auto result = co_await transaction(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Stream::getProfits()
{
    Json::Value command;
    command["command"] = "getProfits";
    command["streamSessionId"] = streamSessionId;
    auto result = co_await transaction(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Stream::stopProfits()
{
    Json::Value command;
    command["command"] = "stopProfits";
    auto result = co_await transaction(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Stream::getTickPrices(const std::string &symbol, int minArrivalTime, int maxLevel)
{
    Json::Value command;
    command["command"] = "getTickPrices";
    command["streamSessionId"] = streamSessionId;
    command["symbol"] = symbol;
    command["minArrivalTime"] = minArrivalTime;
    command["maxLevel"] = maxLevel;
    auto result = co_await transaction(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Stream::stopTickPrices(const std::string &symbol)
{
    Json::Value command;
    command["command"] = "stopTickPrices";
    command["symbol"] = symbol;
    auto result = co_await transaction(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Stream::getTrades()
{
    Json::Value command;
    command["command"] = "getTrades";
    command["streamSessionId"] = streamSessionId;
    auto result = co_await transaction(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Stream::stopTrades()
{
    Json::Value command;
    command["command"] = "stopTrades";
    auto result = co_await transaction(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Stream::getTradeStatus()
{
    Json::Value command;
    command["command"] = "getTradeStatus";
    command["streamSessionId"] = streamSessionId;
    auto result = co_await transaction(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Stream::stopTradeStatus()
{
    Json::Value command;
    command["command"] = "stopTradeStatus";
    auto result = co_await transaction(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Stream::ping()
{
    Json::Value command;
    command["command"] = "ping";
    command["streamSessionId"] = streamSessionId;
    auto result = co_await transaction(command);
    co_return result;
}

} // namespace xapi
