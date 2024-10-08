#include "Socket.hpp"
#include "Exceptions.hpp"

namespace xapi
{

Socket::Socket(boost::asio::io_context &ioContext) : Connection(ioContext), m_safeMode(true)
{
}

void Socket::setSafeMode(bool safeMode) {
    m_safeMode = safeMode;
}

boost::asio::awaitable<boost::json::object> Socket::getAllSymbols()
{
    boost::json::object command = {
        {"command", "getAllSymbols"}
    };   
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::getCalendar()
{
    boost::json::object command = {
        {"command", "getCalendar"}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::getChartLastRequest(const std::string &symbol, const int start,
                                                                PeriodCode period)
{
    boost::json::object command = {
        {"command", "getChartLastRequest"},
        {"arguments", {
            {"info", {
                {"period", static_cast<int>(period)},
                {"start", start},
                {"symbol", symbol}
            }}
        }}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::getChartRangeRequest(const std::string &symbol, int start, int end,
                                                                 PeriodCode period, int ticks)
{
    boost::json::object command = {
        {"command", "getChartRangeRequest"},
        {"arguments", {
            {"info", {
                {"end", end},
                {"period", static_cast<int>(period)},
                {"start", start},
                {"symbol", symbol},
                {"ticks", ticks}
            }}
        }}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::getCommissionDef(const std::string &symbol, float volume)
{
    boost::json::object command = {
        {"command", "getCommissionDef"},
        {"arguments", {
            {"symbol", symbol},
            {"volume", volume}
        }}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::getCurrentUserData()
{
    boost::json::object command = {
        {"command", "getCurrentUserData"}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::getIbsHistory(int start, int end)
{
    boost::json::object command = {
        {"command", "getIbsHistory"},
        {"arguments", {
            {"start", start},
            {"end", end}
        }}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::getMarginLevel()
{
    boost::json::object command = {
        {"command", "getMarginLevel"}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::getMarginTrade(const std::string &symbol, float volume)
{
    boost::json::object command = {
        {"command", "getMarginTrade"},
        {"arguments", {
            {"symbol", symbol},
            {"volume", volume}
        }}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::getNews(int start, int end)
{
    boost::json::object command = {
        {"command", "getNews"},
        {"arguments", {
            {"start", start},
            {"end", end}
        }}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::getProfitCalculation(const std::string &symbol, int cmd, float openPrice,
                                                                 float closePrice, float volume)
{
    boost::json::object command = {
        {"command", "getProfitCalculation"},
        {"arguments", {
            {"symbol", symbol},
            {"cmd", cmd},
            {"openPrice", openPrice},
            {"closePrice", closePrice},
            {"volume", volume}
        }}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::getServerTime()
{
    boost::json::object command = {
        {"command", "getServerTime"}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::getStepRules()
{
    boost::json::object command = {
        {"command", "getStepRules"}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::getSymbol(const std::string &symbol)
{
    boost::json::object command = {
        {"command", "getSymbol"},
        {"arguments", {
            {"symbol", symbol}
        }}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::getTickPrices(const std::vector<std::string> &symbols, int timestamp,
                                                          int level)
{
    boost::json::object command = {
        {"command", "getTickPrices"},
        {"arguments", {
            {"symbols", boost::json::array(symbols.begin(), symbols.end())},
            {"timestamp", timestamp},
            {"level", level}
        }}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::getTradeRecords(const std::vector<int> &orders)
{
    boost::json::object command = {
        {"command", "getTradeRecords"},
        {"arguments", {
            {"orders", boost::json::array(orders.begin(), orders.end())}
        }}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::getTrades(bool openedOnly)
{
    boost::json::object command = {
        {"command", "getTrades"},
        {"arguments", {
            {"openedOnly", openedOnly}
        }}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::getTradesHistory(int start, int end)
{
    boost::json::object command = {
        {"command", "getTradesHistory"},
        {"arguments", {
            {"start", start},
            {"end", end}
        }}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::getTradingHours(const std::vector<std::string> &symbols)
{
    boost::json::object command = {
        {"command", "getTradingHours"},
        {"arguments", {
            {"symbols", boost::json::array(symbols.begin(), symbols.end())}
        }}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::getVersion()
{
    boost::json::object command = {
        {"command", "getVersion"}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::ping()
{
    boost::json::object command = {
        {"command", "ping"}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::tradeTransaction(const std::string &symbol, TradeCmd cmd, TradeType type,
                                                             float price, float volume, float sl, float tp, int order,
                                                             int expiration, int offset,
                                                             const std::string &customComment)
{
    if (m_safeMode) {
        boost::json::object response = {
            {"status", false},
            {"errorCode", "N/A"},
            {"errorDescr", "Trading is disabled when safe=True"}
        };
        co_return response;
    }

    boost::json::object command = {
        {"command", "tradeTransaction"},
        {"arguments", {
            {"tradeTransInfo", {
                {"cmd", static_cast<int>(cmd)},
                {"customComment", customComment},
                {"expiration", expiration},
                {"offset", offset},
                {"order", order},
                {"price", price},
                {"sl", sl},
                {"symbol", symbol},
                {"tp", tp},
                {"type", static_cast<int>(type)},
                {"volume", volume}
            }}
        }}
    };

    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::tradeTransactionStatus(int order)
{
    boost::json::object command = {
        {"command", "tradeTransactionStatus"},
        {"arguments", {
            {"order", order}
        }}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> Socket::request(const boost::json::object &command)
{
    co_await makeRequest(command);
    auto result = co_await waitResponse();
    co_return result;
}

} // namespace xapi
