#pragma once

/**
 * @file Socket.hpp
 * @brief Defines the Socket class for retrieving trading data.
 *
 * This file contains the definition of the Socket class, which encapsulates
 * operations for retrieving trading data from xAPI.
 */

#include "Connection.hpp"
#include "Enums.hpp"

namespace xapi
{
namespace internals
{

/**
 * @brief Encapsulates operations for retrieving trading data from xAPI.
 *
 * The Socket class provides a high-level interface for retrieving trading data
 * from xAPI. It is built on top of the Connection class, which handles the
 * low-level details of establishing and maintaining a connection.
 */
class Socket : protected internals::Connection
{
  public:
    Socket() = delete;

    Socket(const Socket &) = delete;
    Socket &operator=(const Socket &) = delete;

    Socket(Socket &&other) = default;
    Socket &operator=(Socket &&other) = delete;

    /**
     * @brief Constructs a new Socket object.
     * @param ioContext The IO context for asynchronous operations.
     */
    explicit Socket(boost::asio::io_context &ioContext);
    ~Socket() override = default;

    void setSafeMode(bool safeMode);

    // Other methods omitted for brevity.
    // Description of the omitted methods: http://developers.xstore.pro/documentation/2.5.0#retrieving-trading-data

    boost::asio::awaitable<boost::json::object> getAllSymbols();

    boost::asio::awaitable<boost::json::object> getCalendar();

    boost::asio::awaitable<boost::json::object> getChartLastRequest(const std::string &symbol, const std::int64_t start,
                                                            PeriodCode period);

    boost::asio::awaitable<boost::json::object> getChartRangeRequest(const std::string &symbol, std::int64_t start, std::int64_t end,
                                                             PeriodCode period, int ticks);

    boost::asio::awaitable<boost::json::object> getCommissionDef(const std::string &symbol, float volme);

    boost::asio::awaitable<boost::json::object> getCurrentUserData();

    boost::asio::awaitable<boost::json::object> getIbsHistory(std::int64_t start, std::int64_t end);

    boost::asio::awaitable<boost::json::object> getMarginLevel();

    boost::asio::awaitable<boost::json::object> getMarginTrade(const std::string &symbol, float volume);

    boost::asio::awaitable<boost::json::object> getNews(std::int64_t start, std::int64_t end);

    boost::asio::awaitable<boost::json::object> getProfitCalculation(const std::string &symbol, int cmd, float openPrice,
                                                             float closePrice, float volume);

    boost::asio::awaitable<boost::json::object> getServerTime();

    boost::asio::awaitable<boost::json::object> getStepRules();

    boost::asio::awaitable<boost::json::object> getSymbol(const std::string &symbol);

    boost::asio::awaitable<boost::json::object> getTickPrices(const std::vector<std::string> &symbols, std::int64_t timestamp,
                                                      int level);

    boost::asio::awaitable<boost::json::object> getTradeRecords(const std::vector<int> &orders);

    boost::asio::awaitable<boost::json::object> getTrades(bool openedOnly);

    boost::asio::awaitable<boost::json::object> getTradesHistory(std::int64_t start, std::int64_t end);

    boost::asio::awaitable<boost::json::object> getTradingHours(const std::vector<std::string> &symbols);

    boost::asio::awaitable<boost::json::object> getVersion();

    boost::asio::awaitable<boost::json::object> ping();

    boost::asio::awaitable<boost::json::object> tradeTransaction(const std::string &symbol, TradeCmd cmd, TradeType type,
                                                         float price, float volume, float sl, float tp, int order,
                                                         std::int64_t expiration, int offset, const std::string &customComment);

    boost::asio::awaitable<boost::json::object> tradeTransactionStatus(int order);

  protected:
    /**
     * @brief Sends a request to the server and waits for response.
     * @param command The command to send as a boost::json::object.
     * @return An awaitable boost::json::object with the response from the server.
     */
    boost::asio::awaitable<boost::json::object> request(const boost::json::object &command);

    /**
     * Flag to indicate if the socket operates in safe mode. Safe mode means that you can not send any trade commands.
     * Only read-only commands are allowed.
     */
    bool m_safeMode;
};

} // namespace internals
} // namespace xapi
