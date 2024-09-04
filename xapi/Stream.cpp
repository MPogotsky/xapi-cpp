#include "Stream.hpp"
#include "Exceptions.hpp"

namespace xapi
{

Stream::Stream(boost::asio::io_context &ioContext) : Connection(ioContext), m_streamSessionId("")
{
}

boost::asio::awaitable<void> Stream::initSession(const std::string &accountType,
                                                 const std::string &streamSessionId)
{
    validateAccountType(accountType);
    const boost::url streamUrl = boost::urls::format("wss://ws.xtb.com/{}Stream", accountType);
    co_await connect(streamUrl);
    m_streamSessionId = streamSessionId;
}

boost::asio::awaitable<void> Stream::closeSession()
{
    co_await disconnect();
}

boost::asio::awaitable<Json::Value> Stream::listen()
{
    auto result = co_await waitResponse();
    co_return result;
}

boost::asio::awaitable<void> Stream::getBalance()
{
    Json::Value command;
    command["command"] = "getBalance";
    command["streamSessionId"] = m_streamSessionId;
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::stopBalance()
{
    Json::Value command;
    command["command"] = "stopBalance";
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::getCandles(const std::string &symbol)
{
    Json::Value command;
    command["command"] = "getCandles";
    command["streamSessionId"] = m_streamSessionId;
    command["symbol"] = symbol;
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::stopCandles(const std::string &symbol)
{
    Json::Value command;
    command["command"] = "stopCandles";
    command["symbol"] = symbol;
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::getKeepAlive()
{
    Json::Value command;
    command["command"] = "getKeepAlive";
    command["streamSessionId"] = m_streamSessionId;
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::stopKeepAlive()
{
    Json::Value command;
    command["command"] = "stopKeepAlive";
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::getNews()
{
    Json::Value command;
    command["command"] = "getNews";
    command["streamSessionId"] = m_streamSessionId;
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::stopNews()
{
    Json::Value command;
    command["command"] = "stopNews";
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::getProfits()
{
    Json::Value command;
    command["command"] = "getProfits";
    command["streamSessionId"] = m_streamSessionId;
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::stopProfits()
{
    Json::Value command;
    command["command"] = "stopProfits";
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::getTickPrices(const std::string &symbol, int minArrivalTime, int maxLevel)
{
    Json::Value command;
    command["command"] = "getTickPrices";
    command["streamSessionId"] = m_streamSessionId;
    command["symbol"] = symbol;
    command["minArrivalTime"] = minArrivalTime;
    command["maxLevel"] = maxLevel;
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::stopTickPrices(const std::string &symbol)
{
    Json::Value command;
    command["command"] = "stopTickPrices";
    command["symbol"] = symbol;
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::getTrades()
{
    Json::Value command;
    command["command"] = "getTrades";
    command["streamSessionId"] = m_streamSessionId;
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::stopTrades()
{
    Json::Value command;
    command["command"] = "stopTrades";
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::getTradeStatus()
{
    Json::Value command;
    command["command"] = "getTradeStatus";
    command["streamSessionId"] = m_streamSessionId;
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::stopTradeStatus()
{
    Json::Value command;
    command["command"] = "stopTradeStatus";
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::ping()
{
    Json::Value command;
    command["command"] = "ping";
    command["streamSessionId"] = m_streamSessionId;
    co_await makeRequest(command);
}

} // namespace xapi
