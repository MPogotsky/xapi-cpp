#include "XStationClient.hpp"
#include "Exceptions.hpp"

namespace xapi
{

const std::unordered_set<std::string> XStationClient::m_knownAccountTypes = {"demo", "real"};

XStationClient::XStationClient(boost::asio::io_context &ioContext, const std::string &accountId,
                               const std::string &password, const std::string &accountType)
    : m_ioContext(ioContext), m_connection(std::make_unique<internals::Connection>(ioContext)), m_accountId(accountId), m_password(password),
      m_accountType(accountType), m_safeMode(true), m_streamSessionId("")
{
}

XStationClient::XStationClient(boost::asio::io_context &ioContext, const boost::json::object &accountCredentials)
    : XStationClient(ioContext,
                     std::string(accountCredentials.at("accountId").as_string()),
                     std::string(accountCredentials.at("password").as_string()),
                     std::string(accountCredentials.at("accountType").as_string()))
{
}

boost::asio::awaitable<void> XStationClient::login() {
    validateAccountType(m_accountType);

    const boost::url socketUrl = boost::urls::format("wss://ws.xtb.com/{}", m_accountType);
    co_await m_connection->connect(socketUrl);

    boost::json::object command = {
        {"command", "login"},
        {"arguments", {
            {"userId", m_accountId},
            {"password", m_password}
        }}
    };
    auto result = co_await request(command);

    if (!result.contains("status") && !result.contains("streamSessionId")) {
        throw exception::LoginFailed("Invalid response from the server");
    }

    if (result["status"].as_bool() != true)
    {
        throw exception::LoginFailed(boost::json::serialize(result));
    }

    m_streamSessionId = result["streamSessionId"].as_string();
}

boost::asio::awaitable<void> XStationClient::logout() {
    boost::json::object command = {
        {"command", "logout"}
    };
    co_await request(command);
    co_await m_connection->disconnect();

}

void XStationClient::setSafeMode(bool safeMode) {
    m_safeMode = safeMode;
}

XStationClientStream XStationClient::getClientStream() const {
    XStationClientStream stream(m_ioContext, m_accountType, m_streamSessionId);
    return stream;
}


boost::asio::awaitable<boost::json::object> XStationClient::getAllSymbols()
{
    boost::json::object command = {
        {"command", "getAllSymbols"}
    };   
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> XStationClient::getCalendar()
{
    boost::json::object command = {
        {"command", "getCalendar"}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> XStationClient::getChartLastRequest(const std::string &symbol, const std::int64_t start,
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

boost::asio::awaitable<boost::json::object> XStationClient::getChartRangeRequest(const std::string &symbol, std::int64_t start, std::int64_t end,
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

boost::asio::awaitable<boost::json::object> XStationClient::getCommissionDef(const std::string &symbol, float volume)
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

boost::asio::awaitable<boost::json::object> XStationClient::getCurrentUserData()
{
    boost::json::object command = {
        {"command", "getCurrentUserData"}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> XStationClient::getIbsHistory(std::int64_t start, std::int64_t end)
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

boost::asio::awaitable<boost::json::object> XStationClient::getMarginLevel()
{
    boost::json::object command = {
        {"command", "getMarginLevel"}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> XStationClient::getMarginTrade(const std::string &symbol, float volume)
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

boost::asio::awaitable<boost::json::object> XStationClient::getNews(std::int64_t start, std::int64_t end)
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

boost::asio::awaitable<boost::json::object> XStationClient::getProfitCalculation(const std::string &symbol, int cmd, float openPrice,
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

boost::asio::awaitable<boost::json::object> XStationClient::getServerTime()
{
    boost::json::object command = {
        {"command", "getServerTime"}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> XStationClient::getStepRules()
{
    boost::json::object command = {
        {"command", "getStepRules"}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> XStationClient::getSymbol(const std::string &symbol)
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

boost::asio::awaitable<boost::json::object> XStationClient::getTickPrices(const std::vector<std::string> &symbols, std::int64_t timestamp,
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

boost::asio::awaitable<boost::json::object> XStationClient::getTradeRecords(const std::vector<int> &orders)
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

boost::asio::awaitable<boost::json::object> XStationClient::getTrades(bool openedOnly)
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

boost::asio::awaitable<boost::json::object> XStationClient::getTradesHistory(std::int64_t start, std::int64_t end)
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

boost::asio::awaitable<boost::json::object> XStationClient::getTradingHours(const std::vector<std::string> &symbols)
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

boost::asio::awaitable<boost::json::object> XStationClient::getVersion()
{
    boost::json::object command = {
        {"command", "getVersion"}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> XStationClient::ping()
{
    boost::json::object command = {
        {"command", "ping"}
    };
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<boost::json::object> XStationClient::tradeTransaction(const std::string &symbol, TradeCmd cmd, TradeType type,
                                                             float price, float volume, float sl, float tp, int order,
                                                             std::int64_t expiration, int offset,
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

boost::asio::awaitable<boost::json::object> XStationClient::tradeTransactionStatus(int order)
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

boost::asio::awaitable<boost::json::object> XStationClient::request(const boost::json::object &command)
{
    co_await m_connection->makeRequest(command);
    auto result = co_await m_connection->waitResponse();
    co_return result;
}

void XStationClient::validateAccountType(const std::string &accountType)
{
    if (m_knownAccountTypes.find(accountType) == m_knownAccountTypes.end())
    {
        std::string reason("Invalid account type: " + accountType);
        throw exception::LoginFailed(reason);
    }
}

} // namespace xapi
