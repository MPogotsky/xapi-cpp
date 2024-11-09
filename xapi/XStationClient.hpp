#pragma once

/**
 * @file XStationClient.hpp
 * @brief Defines the XStationClient class for retrieving trading data.
 *
 * This file contains the definition of the XStationClient class, which encapsulates
 * operations for retrieving trading data from xAPI.
 */

#include "Connection.hpp"
#include "XStationClientStream.hpp"
#include "Enums.hpp"
#include <unordered_set>

#undef TEST_FRIENDS
#ifdef ENABLE_TEST
#include "gtest/gtest_prod.h"
class XStationClientTest;
#define TEST_FRIENDS \
    friend class XStationClientTest; \
    FRIEND_TEST(XStationClientTest, login_ok); \
    FRIEND_TEST(XStationClientTest, login_invalid_account_type); \
    FRIEND_TEST(XStationClientTest, login_account_credentials_null); \
    FRIEND_TEST(XStationClientTest, login_invalid_return_from_the_server);
#else
#define TEST_FRIENDS
#endif

namespace xapi
{

/**
 * @brief Encapsulates operations for retrieving trading data from xAPI.
 *
 * The XStationClient class provides a high-level interface for retrieving trading data
 * from xAPI. It is built on top of the Connection class, which handles the
 * low-level details of establishing and maintaining a connection.
 */
class XStationClient final
{
  public:
    XStationClient() = delete;

    XStationClient(const XStationClient &) = delete;
    XStationClient &operator=(const XStationClient &) = delete;

    XStationClient(XStationClient &&other) = default;
    XStationClient &operator=(XStationClient &&other) = delete;

    /**
     * @brief Constructs a new XStationClient object.
     * @param ioContext The IO context for asynchronous operations.
     * @param accountId The account ID to use for the client.
     * @param password The password to use for the client.
     * @param accountType The type of account to use for the client.
     * Possible values are:
     *
     *      - `"demo"` for a demo account.
     *
     *      - `"real"` for a real money account.
     */
    explicit XStationClient(boost::asio::io_context &ioContext, const std::string &accountId,
                            const std::string &password, const std::string &accountType = "demo");

    /**
     * @brief Constructs a new XStationClient object.
     *
     * Initializes the client with the provided IO context and account credentials.
     *
     * @param ioContext The IO context for managing asynchronous operations.
     * @param accountCredentials A boost::json::object containing the account credentials required for the client.
     *                           The map should include the following keys:
     *
     *    - `accountId`: The account ID as a string.
     *
     *    - `password`: The account password as a string.
     *
     *    - `accountType`: The type of account. Possible values are: `"demo"` or `"real"`
     *
     */
    explicit XStationClient(boost::asio::io_context &ioContext, const boost::json::object &accountCredentials);
    
    ~XStationClient() = default;

    /**
     * @brief Opens connection to the server and logs in.
     * @return An awaitable void.
     * @throw xapi::exception::ConnectionClosed if the connection fails.
     * @throw xapi::exception::LoginFailed if the login fails.
     */
    boost::asio::awaitable<void> login();

    /**
     * @brief Logs out from the server and closes the connection(if not closed by server).
     * @return An awaitable void.
     */
    boost::asio::awaitable<void> logout();

    /**
     * @brief Sets the safe mode flag. If true, you can not send any trade commands and
     * only read-only commands are allowed.
     * @param safeMode true/false to enable/disable safe mode.
     */
    void setSafeMode(bool safeMode);

    /**
     * @brief Gets the client stream object.
     * @return The XStationClientStream object.
     */
    XStationClientStream getClientStream() const;

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

  private:

    boost::asio::io_context &m_ioContext;
    std::unique_ptr<internals::IConnection> m_connection;

    const std::string m_accountId;
    const std::string m_password;
    const std::string m_accountType;

    /**
     * Flag to indicate if the user operates in safe mode.
     */
    bool m_safeMode;

    std::string m_streamSessionId;

    // Set of known account types.
    static const std::unordered_set<std::string> m_knownAccountTypes;

    /**
     * @brief Sends a request to the server and waits for response.
     * @param command The command to send as a boost::json::object.
     * @return An awaitable boost::json::object with the response from the server.
     */
    boost::asio::awaitable<boost::json::object> request(const boost::json::object &command);

    /**
     * @brief Validates the account type.
     * @param accountType The account type to validate.
     * @throw xapi::exception::ConnectionClosed if the account type is not known.
     */
    static void validateAccountType(const std::string &accountType);

    TEST_FRIENDS
};

} // namespace xapi
