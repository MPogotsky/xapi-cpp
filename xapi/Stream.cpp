#include "Stream.hpp"
#include "Exceptions.hpp"

namespace xapi
{
namespace internals
{

Stream::Stream(boost::asio::io_context &ioContext, const std::string& streamSessionId ) : Connection(ioContext), m_streamSessionId(streamSessionId)
{
}

boost::asio::awaitable<boost::json::object> Stream::listen()
{
    auto result = co_await waitResponse();
    co_return result;
}

boost::asio::awaitable<void> Stream::getBalance()
{
    boost::json::object command = {
        {"command", "getBalance"},
        {"streamSessionId", m_streamSessionId}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::stopBalance()
{
    boost::json::object command = {
        {"command", "stopBalance"}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::getCandles(const std::string &symbol)
{
    boost::json::object command = {
        {"command", "getCandles"},
        {"streamSessionId", m_streamSessionId},
        {"symbol", symbol}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::stopCandles(const std::string &symbol)
{
    boost::json::object command = {
        {"command", "stopCandles"},
        {"symbol", symbol}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::getKeepAlive()
{
    boost::json::object command = {
        {"command", "getKeepAlive"},
        {"streamSessionId", m_streamSessionId}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::stopKeepAlive()
{
    boost::json::object command = {
        {"command", "stopKeepAlive"}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::getNews()
{
    boost::json::object command = {
        {"command", "getNews"},
        {"streamSessionId", m_streamSessionId}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::stopNews()
{
    boost::json::object command = {
        {"command", "stopNews"}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::getProfits()
{
    boost::json::object command = {
        {"command", "getProfits"},
        {"streamSessionId", m_streamSessionId}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::stopProfits()
{
    boost::json::object command = {
        {"command", "stopProfits"}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::getTickPrices(const std::string &symbol, int minArrivalTime, int maxLevel)
{
    boost::json::object command = {
        {"command", "getTickPrices"},
        {"streamSessionId", m_streamSessionId},
        {"symbol", symbol},
        {"minArrivalTime", minArrivalTime},
        {"maxLevel", maxLevel}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::stopTickPrices(const std::string &symbol)
{
    boost::json::object command = {
        {"command", "stopTickPrices"},
        {"symbol", symbol}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::getTrades()
{
    boost::json::object command = {
        {"command", "getTrades"},
        {"streamSessionId", m_streamSessionId}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::stopTrades()
{
    boost::json::object command = {
        {"command", "stopTrades"}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::getTradeStatus()
{
    boost::json::object command = {
        {"command", "getTradeStatus"},
        {"streamSessionId", m_streamSessionId}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::stopTradeStatus()
{
    boost::json::object command = {
        {"command", "stopTradeStatus"}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> Stream::ping()
{
    boost::json::object command = {
        {"command", "ping"},
        {"streamSessionId", m_streamSessionId}
    };
    co_await makeRequest(command);
}

} // namespace internals
} // namespace xapi
