#pragma once

/**
 * @file XStationClientStream.hpp
 * @brief Defines the XStationClientStream class for managing streaming commands.
 *
 * This file contains the definition of the XStationClientStream class, which encapsulates
 * streaming operations for real-time streaming data.
 */

#include "Connection.hpp"

namespace xapi
{

/**
 * @brief Encapsulates operations for streaming real-time data from xAPI.
 *
 * The XStationClientStream class provides a high-level interface for streaming real-time data
 * from xAPI.
 */
class XStationClientStream : protected internals::Connection
{
  public:
    XStationClientStream() = delete;

    XStationClientStream(const XStationClientStream &) = delete;
    XStationClientStream &operator=(const XStationClientStream &) = delete;

    XStationClientStream(XStationClientStream &&other) = default;
    XStationClientStream &operator=(XStationClientStream &&other) = delete;

    /**
     * @brief Constructs a new XStationClientStream object.
     * @param ioContext The IO context for asynchronous operations.
     */
    explicit XStationClientStream(boost::asio::io_context &ioContext, const std::string &accountType, const std::string& streamSessionId);
    ~XStationClientStream() override = default;

    /**
     * @brief Opens a connection to the streaming server.
     * @return An awaitable void.
     * @throw xapi::exception::ConnectionClosed if the connection fails.
     */
    boost::asio::awaitable<void> open();

    /**
     * @brief Closes the connection to the streaming server.
     * @return An awaitable void.
     * @throw xapi::exception::ConnectionClosed if closing the connection fails unexpectedly.
     */
    boost::asio::awaitable<void> close();

    /**
     * @brief Starts listening for streaming data.
     * @return An awaitable boost::json::object with streaming data.
     */
    boost::asio::awaitable<boost::json::object> listen();

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
    const boost::url m_streamUrl;
    const std::string m_streamSessionId;
};

} // namespace xapi
