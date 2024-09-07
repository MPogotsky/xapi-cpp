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

/**
 * @brief Encapsulates operations for retrieving trading data from xAPI.
 *
 * The Socket class provides a high-level interface for retrieving trading data
 * from xAPI. It is built on top of the Connection class, which handles the
 * low-level details of establishing and maintaining a connection.
 */
class Socket final : protected internals::Connection
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

    /**
     * Flag to indicate if the socket operates in safe mode. Safe mode means that you can not send any trade commands.
     * Only read-only commands are allowed.
     */
    bool safeMode;

    /**
     * @brief Initializes a session with the specified account type.
     * @param accountType The type of account to initialize the session for.
     * Possible values are:
     *     - "demo" for a demo account,
     *     - "real" for a real money account.
     * @return An awaitable void.
     */
    boost::asio::awaitable<void> initSession(const std::string &accountType);

    /**
     * @brief Closes the current session.
     * @return An awaitable void.
     */
    boost::asio::awaitable<void> closeSession();

    /**
     * @brief Logs in to the server with the specified account ID and password.
     * @param accountId The account ID to log in with.
     * @param password The password for the account.
     * @return An awaitable string containing the stream session ID.
     */
    boost::asio::awaitable<std::string> login(const std::string &accountId, const std::string &password);

    /**
     * @brief Logs out of the server.
     * @return An awaitable boost::json::object containing the logout status.
     */
    boost::asio::awaitable<boost::json::object> logout();

    // Other methods omitted for brevity.
    // Description of the omitted methods: http://developers.xstore.pro/documentation/2.5.0#retrieving-trading-data

    boost::asio::awaitable<boost::json::object> getAllSymbols();

    boost::asio::awaitable<boost::json::object> getCalendar();

    boost::asio::awaitable<boost::json::object> getChartLastRequest(const std::string &symbol, const int start,
                                                            PeriodCode period);

    boost::asio::awaitable<boost::json::object> getChartRangeRequest(const std::string &symbol, int start, int end,
                                                             PeriodCode period, int ticks);

    boost::asio::awaitable<boost::json::object> getCommissionDef(const std::string &symbol, float volme);

    boost::asio::awaitable<boost::json::object> getCurrentUserData();

    boost::asio::awaitable<boost::json::object> getIbsHistory(int start, int end);

    boost::asio::awaitable<boost::json::object> getMarginLevel();

    boost::asio::awaitable<boost::json::object> getMarginTrade(const std::string &symbol, float volume);

    boost::asio::awaitable<boost::json::object> getNews(int start, int end);

    boost::asio::awaitable<boost::json::object> getProfitCalculation(const std::string &symbol, int cmd, float openPrice,
                                                             float closePrice, float volume);

    boost::asio::awaitable<boost::json::object> getServerTime();

    boost::asio::awaitable<boost::json::object> getStepRules();

    boost::asio::awaitable<boost::json::object> getSymbol(const std::string &symbol);

    boost::asio::awaitable<boost::json::object> getTickPrices(const std::vector<std::string> &symbols, int timestamp,
                                                      int level);

    boost::asio::awaitable<boost::json::object> getTradeRecords(const std::vector<int> &orders);

    boost::asio::awaitable<boost::json::object> getTrades(bool openedOnly);

    boost::asio::awaitable<boost::json::object> getTradesHistory(int start, int end);

    boost::asio::awaitable<boost::json::object> getTradingHours(const std::vector<std::string> &symbols);

    boost::asio::awaitable<boost::json::object> getVersion();

    boost::asio::awaitable<boost::json::object> ping();

    boost::asio::awaitable<boost::json::object> tradeTransaction(const std::string &symbol, TradeCmd cmd, TradeType type,
                                                         float price, float volume, float sl, float tp, int order,
                                                         int expiration, int offset, const std::string &customComment);

    boost::asio::awaitable<boost::json::object> tradeTransactionStatus(int order);

  private:
    /**
     * @brief Sends a request to the server and waits for response.
     * @param command The command to send as a boost::json::object.
     * @return An awaitable boost::json::object with the response from the server.
     */
    boost::asio::awaitable<boost::json::object> request(const boost::json::object &command);
};

} // namespace xapi
