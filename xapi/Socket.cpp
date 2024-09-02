#include "Socket.hpp"
#include "Exceptions.hpp"

namespace xapi
{

Socket::Socket(boost::asio::io_context &ioContext)
: Connection(ioContext), safeMode(true) 
{}

boost::asio::awaitable<void> Socket::initSession(const std::string &host, const std::string &type)
{
    const std::string socketUrl = urlWithValidHost(host).value_or(host) + "/" + type;
    co_await connect(socketUrl);
}

boost::asio::awaitable<void> Socket::closeSession()
{
    co_await disconnect();
}

boost::asio::awaitable<std::string> Socket::login(const std::string &accountId, const std::string &password)
{
    Json::Value command;
    command["command"] = "login";
    command["arguments"]["userId"] = accountId;
    command["arguments"]["password"] = password;
    auto result = co_await request(command);

    if(result["status"].asBool() != true) {
        Json::StreamWriterBuilder writer;
        throw exception::LoginFailed(Json::writeString(writer, result));
    }
    
    co_return result["streamSessionId"].asString();
}

boost::asio::awaitable<Json::Value> Socket::logout()
{
    Json::Value command;
    command["command"] = "logout";
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::getAllSymbols()
{
    Json::Value command;
    command["command"] = "getAllSymbols";
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::getCalendar()
{
    Json::Value command;
    command["command"] = "getCalendar";
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::getChartLastRequest(const std::string &symbol, const int start,
                                                                PeriodCode period)
{
    Json::Value command;
    command["command"] = "getChartLastRequest";
    command["arguments"]["info"]["period"] = static_cast<int>(period);
    command["arguments"]["info"]["start"] = start;
    command["arguments"]["info"]["symbol"] = symbol;
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::getChartRangeRequest(const std::string &symbol, int start, int end, PeriodCode period, int ticks)
{
    Json::Value command;
    command["command"] = "getChartRangeRequest";
    command["arguments"]["info"]["end"] = end;
    command["arguments"]["info"]["period"] = static_cast<int>(period);
    command["arguments"]["info"]["start"] = start;
    command["arguments"]["info"]["symbol"] = symbol;
    command["arguments"]["info"]["ticks"] = ticks;
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::getCommissionDef(const std::string &symbol, float volume)
{
    Json::Value command;
    command["command"] = "getCommissionDef";
    command["arguments"]["symbol"] = symbol;
    command["arguments"]["volume"] = volume;
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::getCurrentUserData()
{
    Json::Value command;
    command["command"] = "getCurrentUserData";
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::getIbsHistory(int start, int end)
{
    Json::Value command;
    command["command"] = "getIbsHistory";
    command["arguments"]["start"] = start;
    command["arguments"]["end"] = end;
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::getMarginLevel()
{
    Json::Value command;
    command["command"] = "getMarginLevel";
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::getMarginTrade(const std::string &symbol, float volume)
{
    Json::Value command;
    command["command"] = "getMarginTrade";
    command["arguments"]["symbol"] = symbol;
    command["arguments"]["volume"] = volume;
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::getNews(int start, int end)
{
    Json::Value command;
    command["command"] = "getNews";
    command["arguments"]["start"] = start;
    command["arguments"]["end"] = end;
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::getProfitCalculation(const std::string &symbol, int cmd, float openPrice,
                                                                 float closePrice, float volume)
{
    Json::Value command;
    command["command"] = "getProfitCalculation";
    command["arguments"]["symbol"] = symbol;
    command["arguments"]["cmd"] = cmd;
    command["arguments"]["openPrice"] = openPrice;
    command["arguments"]["closePrice"] = closePrice;
    command["arguments"]["volume"] = volume;
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::getServerTime()
{
    Json::Value command;
    command["command"] = "getServerTime";
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::getStepRules()
{
    Json::Value command;
    command["command"] = "getStepRules";
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::getSymbol(const std::string &symbol)
{
    Json::Value command;
    command["command"] = "getSymbol";
    command["arguments"]["symbol"] = symbol;
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::getTickPrices(const std::vector<std::string> &symbols, int timestamp,
                                                          int level)
{
    Json::Value command;
    command["command"] = "getTickPrices";
    command["arguments"]["symbols"] = Json::arrayValue;
    for (const auto &sym : symbols)
    {
        command["arguments"]["symbols"].append(sym);
    }
    command["arguments"]["timestamp"] = timestamp;
    command["arguments"]["level"] = level;
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::getTradeRecords(const std::vector<int> &orders)
{
    Json::Value command;
    command["command"] = "getTradeRecords";
    command["arguments"]["orders"] = Json::arrayValue;
    for (const auto &order : orders)
    {
        command["arguments"]["orders"].append(order);
    }
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::getTrades(bool openedOnly)
{
    Json::Value command;
    command["command"] = "getTrades";
    command["arguments"]["openedOnly"] = openedOnly;
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::getTradesHistory(int start, int end)
{
    Json::Value command;
    command["command"] = "getTradesHistory";
    command["arguments"]["start"] = start;
    command["arguments"]["end"] = end;
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::getTradingHours(const std::vector<std::string> &symbols)
{
    Json::Value command;
    command["command"] = "getTradingHours";
    command["arguments"]["symbols"] = Json::arrayValue;
    for (const auto &sym : symbols)
    {
        command["arguments"]["symbols"].append(sym);
    }
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::getVersion()
{
    Json::Value command;
    command["command"] = "getVersion";
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::ping()
{
    Json::Value command;
    command["command"] = "ping";
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::tradeTransaction(const std::string &symbol, TradeCmd cmd, TradeType type,
                                                             float price, float volume, float sl, float tp, int order,
                                                             int expiration, int offset,
                                                             const std::string &customComment)
{
    if (safeMode)
    {
        Json::Value response;
        response["status"] = false;
        response["errorCode"] = "N/A";
        response["errorDescr"] = "Trading is disabled when safe=True";
        co_return response;
    }

    Json::Value command;
    command["command"] = "tradeTransaction";
    command["arguments"]["tradeTransInfo"]["cmd"] = static_cast<int>(cmd);
    command["arguments"]["tradeTransInfo"]["customComment"] = customComment;
    command["arguments"]["tradeTransInfo"]["expiration"] = expiration;
    command["arguments"]["tradeTransInfo"]["offset"] = offset;
    command["arguments"]["tradeTransInfo"]["order"] = order;
    command["arguments"]["tradeTransInfo"]["price"] = price;
    command["arguments"]["tradeTransInfo"]["sl"] = sl;
    command["arguments"]["tradeTransInfo"]["symbol"] = symbol;
    command["arguments"]["tradeTransInfo"]["tp"] = tp;
    command["arguments"]["tradeTransInfo"]["type"] = static_cast<int>(type);
    command["arguments"]["tradeTransInfo"]["volume"] = volume;

    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::tradeTransactionStatus(int order)
{
    Json::Value command;
    command["command"] = "tradeTransactionStatus";
    command["arguments"]["order"] = order;
    auto result = co_await request(command);
    co_return result;
}

boost::asio::awaitable<Json::Value> Socket::request(const Json::Value& command)
{
    co_await makeRequest(command);
    auto result = co_await waitResponse();
    co_return result;
}

} // namespace xapi
