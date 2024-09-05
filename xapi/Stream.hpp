#pragma once

/**
 * @file Stream.hpp
 * @brief Defines the Stream class for managing streaming commands.
 *
 * This file contains the definition of the Stream class, which encapsulates
 * streaming operations for real-time streaming data.
 */

#include "Connection.hpp"

namespace xapi
{

/**
 * @brief Encapsulates operations for streaming real-time data from xAPI.
 *
 * The Stream class provides a high-level interface for streaming real-time data
 * from xAPI.
 */
class Stream final : protected internals::Connection
{
  public:
    Stream() = delete;

    Stream(const Stream &) = delete;
    Stream &operator=(const Stream &) = delete;

    Stream(Stream &&other) = default;
    Stream &operator=(Stream &&other) = delete;

    /**
     * @brief Constructs a new Stream object.
     * @param ioContext The IO context for asynchronous operations.
     */
    explicit Stream(boost::asio::io_context &ioContext);
    ~Stream() override = default;

    /**
     * @brief Initializes a streaming session with the specified account type and stream session ID.
     * @param accountType The type of account to initialize the session for.
     * Possible values are:
     *     - "demo" for a demo account,
     *     - "real" for a real money account.
     * @param streamSessionId The stream session ID returned by Socket login call.
     * @return An awaitable void.
     */
    boost::asio::awaitable<void> initSession(const std::string &accountType, const std::string &streamSessionId);

    /**
     * @brief Closes the current streaming session.
     * @return An awaitable void.
     */
    boost::asio::awaitable<void> closeSession();

    /**
     * @brief Starts listening for streaming data.
     * @return An awaitable Json::Value with streaming data.
     */
    boost::asio::awaitable<Json::Value> listen();

    // Other methods omitted for brevity.
    // Description of the omitted methods: http://developers.xstore.pro/documentation/2.5.0#retrieving-trading-data

    boost::asio::awaitable<void> getBalance();

    boost::asio::awaitable<void> stopBalance();

    boost::asio::awaitable<void> getCandles(const std::string &symbol);

    boost::asio::awaitable<void> stopCandles(const std::string &symbol);

    boost::asio::awaitable<void> getKeepAlive();

    boost::asio::awaitable<void> stopKeepAlive();

    boost::asio::awaitable<void> getNews();

    boost::asio::awaitable<void> stopNews();

    boost::asio::awaitable<void> getProfits();

    boost::asio::awaitable<void> stopProfits();

    boost::asio::awaitable<void> getTickPrices(const std::string &symbol, int minArrivalTime = 0, int maxLevel = 2);

    boost::asio::awaitable<void> stopTickPrices(const std::string &symbol);

    boost::asio::awaitable<void> getTrades();

    boost::asio::awaitable<void> stopTrades();

    boost::asio::awaitable<void> getTradeStatus();

    boost::asio::awaitable<void> stopTradeStatus();

    boost::asio::awaitable<void> ping();

  private:
    // The stream session ID.
    std::string m_streamSessionId;
};

} // namespace xapi
