#pragma once

/**
 * @file XStationClient.hpp
 * @brief Defines the XStationClient class for managing xStation client.
 *
 * This file contains the definition of the XStationClient class, which provides
 * an interface for managing xStation client and retreiving Socket and Stream objects.
 */

#include "Socket.hpp"
#include "XClientStream.hpp"

namespace xapi
{
/**
 * @brief Provides an interface for managing xStation client and retreiving Socket and Stream objects.
 *
 * The XStationClient class provides a high-level interface for managing xStation client and retreiving xapi::Socket and
 * xapi::Stream objects that can be used to interact with xAPI.
 */
class XStationClient final : public Socket
{
  public:
    XStationClient() = delete;

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
    XStationClient(boost::asio::io_context &ioContext, const std::string &accountId,
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
    XStationClient(boost::asio::io_context &ioContext, const boost::json::object &accountCredentials);

    ~XStationClient() = default;


    boost::asio::awaitable<void> login();

    boost::asio::awaitable<void> logout();

    XClientStream getClientStream();

  private:
    boost::asio::io_context &m_ioContext;

    const std::string m_accountId;
    const std::string m_password;
    const std::string m_accountType;

    std::string m_streamSessionId;
};

} // namespace xapi
