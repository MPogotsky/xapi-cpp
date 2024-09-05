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
     * @return An awaitable Json::Value containing the logout status.
     */
    boost::asio::awaitable<Json::Value> logout();

    // Other methods omitted for brevity.
    // Description of the omitted methods: http://developers.xstore.pro/documentation/2.5.0#retrieving-trading-data

    boost::asio::awaitable<Json::Value> getAllSymbols();

    boost::asio::awaitable<Json::Value> getCalendar();

    boost::asio::awaitable<Json::Value> getChartLastRequest(const std::string &symbol, const int start,
                                                            PeriodCode period);

    boost::asio::awaitable<Json::Value> getChartRangeRequest(const std::string &symbol, int start, int end,
                                                             PeriodCode period, int ticks);

    boost::asio::awaitable<Json::Value> getCommissionDef(const std::string &symbol, float volme);

    boost::asio::awaitable<Json::Value> getCurrentUserData();

    boost::asio::awaitable<Json::Value> getIbsHistory(int start, int end);

    boost::asio::awaitable<Json::Value> getMarginLevel();

    boost::asio::awaitable<Json::Value> getMarginTrade(const std::string &symbol, float volume);

    boost::asio::awaitable<Json::Value> getNews(int start, int end);

    boost::asio::awaitable<Json::Value> getProfitCalculation(const std::string &symbol, int cmd, float openPrice,
                                                             float closePrice, float volume);

    boost::asio::awaitable<Json::Value> getServerTime();

    boost::asio::awaitable<Json::Value> getStepRules();

    boost::asio::awaitable<Json::Value> getSymbol(const std::string &symbol);

    boost::asio::awaitable<Json::Value> getTickPrices(const std::vector<std::string> &symbols, int timestamp,
                                                      int level);

    boost::asio::awaitable<Json::Value> getTradeRecords(const std::vector<int> &orders);

    boost::asio::awaitable<Json::Value> getTrades(bool openedOnly);

    boost::asio::awaitable<Json::Value> getTradesHistory(int start, int end);

    boost::asio::awaitable<Json::Value> getTradingHours(const std::vector<std::string> &symbols);

    boost::asio::awaitable<Json::Value> getVersion();

    boost::asio::awaitable<Json::Value> ping();

    boost::asio::awaitable<Json::Value> tradeTransaction(const std::string &symbol, TradeCmd cmd, TradeType type,
                                                         float price, float volume, float sl, float tp, int order,
                                                         int expiration, int offset, const std::string &customComment);

    boost::asio::awaitable<Json::Value> tradeTransactionStatus(int order);

  private:
    /**
     * @brief Sends a request to the server and waits for response.
     * @param command The command to send as a Json::Value.
     * @return An awaitable Json::Value with the response from the server.
     */
    boost::asio::awaitable<Json::Value> request(const Json::Value &command);
};

} // namespace xapi
