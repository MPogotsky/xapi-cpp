#include "XStationClientStream.hpp"
#include "Exceptions.hpp"

namespace xapi
{

XStationClientStream::XStationClientStream(boost::asio::io_context &ioContext, const std::string &accountType, const std::string& streamSessionId) 
: Connection(ioContext), m_streamUrl(boost::urls::format("wss://ws.xtb.com/{}Stream", accountType)), m_streamSessionId(streamSessionId)
{
}

boost::asio::awaitable<void> XStationClientStream::open()
{
    co_await connect(m_streamUrl);
}

boost::asio::awaitable<void> XStationClientStream::close()
{
    co_await disconnect();
}

boost::asio::awaitable<boost::json::object> XStationClientStream::listen()
{
    auto result = co_await waitResponse();
    co_return result;
}

boost::asio::awaitable<void> XStationClientStream::getBalance()
{
    boost::json::object command = {
        {"command", "getBalance"},
        {"streamSessionId", m_streamSessionId}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> XStationClientStream::stopBalance()
{
    boost::json::object command = {
        {"command", "stopBalance"}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> XStationClientStream::getCandles(const std::string &symbol)
{
    boost::json::object command = {
        {"command", "getCandles"},
        {"streamSessionId", m_streamSessionId},
        {"symbol", symbol}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> XStationClientStream::stopCandles(const std::string &symbol)
{
    boost::json::object command = {
        {"command", "stopCandles"},
        {"symbol", symbol}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> XStationClientStream::getKeepAlive()
{
    boost::json::object command = {
        {"command", "getKeepAlive"},
        {"streamSessionId", m_streamSessionId}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> XStationClientStream::stopKeepAlive()
{
    boost::json::object command = {
        {"command", "stopKeepAlive"}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> XStationClientStream::getNews()
{
    boost::json::object command = {
        {"command", "getNews"},
        {"streamSessionId", m_streamSessionId}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> XStationClientStream::stopNews()
{
    boost::json::object command = {
        {"command", "stopNews"}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> XStationClientStream::getProfits()
{
    boost::json::object command = {
        {"command", "getProfits"},
        {"streamSessionId", m_streamSessionId}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> XStationClientStream::stopProfits()
{
    boost::json::object command = {
        {"command", "stopProfits"}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> XStationClientStream::getTickPrices(const std::string &symbol, int minArrivalTime, int maxLevel)
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

boost::asio::awaitable<void> XStationClientStream::stopTickPrices(const std::string &symbol)
{
    boost::json::object command = {
        {"command", "stopTickPrices"},
        {"symbol", symbol}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> XStationClientStream::getTrades()
{
    boost::json::object command = {
        {"command", "getTrades"},
        {"streamSessionId", m_streamSessionId}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> XStationClientStream::stopTrades()
{
    boost::json::object command = {
        {"command", "stopTrades"}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> XStationClientStream::getTradeStatus()
{
    boost::json::object command = {
        {"command", "getTradeStatus"},
        {"streamSessionId", m_streamSessionId}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> XStationClientStream::stopTradeStatus()
{
    boost::json::object command = {
        {"command", "stopTradeStatus"}
    };
    co_await makeRequest(command);
}

boost::asio::awaitable<void> XStationClientStream::ping()
{
    boost::json::object command = {
        {"command", "ping"},
        {"streamSessionId", m_streamSessionId}
    };
    co_await makeRequest(command);
}

} // namespace xapi
