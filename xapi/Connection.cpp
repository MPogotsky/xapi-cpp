#include "Connection.hpp"
#include "Exceptions.hpp"

namespace xapi
{
namespace internals
{

Connection::Connection(boost::asio::io_context &ioContext)
    : m_ioContext(ioContext), m_sslContext(boost::asio::ssl::context::tlsv13_client),
      m_websocket(m_ioContext, m_sslContext), m_cancellationSignal(),
      m_lastRequestTime(std::chrono::system_clock::now()), m_requestTimeout(200), m_websocketDefaultPort("443")
{
}

Connection::Connection(Connection &&other) noexcept
    : m_ioContext(other.m_ioContext),
      m_sslContext(std::move(other.m_sslContext)),
      m_websocket(std::move(other.m_websocket)),
      m_lastRequestTime(std::move(other.m_lastRequestTime)),
      m_requestTimeout(other.m_requestTimeout),
      m_websocketDefaultPort(std::move(other.m_websocketDefaultPort))
{
}

boost::asio::awaitable<void> Connection::connect(const boost::url &url)
{
    const auto executor = co_await boost::asio::this_coro::executor;
    try
    {
        boost::asio::ip::tcp::resolver resolver(executor);
        auto const results =
            co_await resolver.async_resolve(url.host(), m_websocketDefaultPort, boost::asio::use_awaitable);

        co_await establishSSLConnection(results, url.host().c_str());

        m_websocket.set_option(
            boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::client));

        co_await m_websocket.async_handshake(url.host(), url.path(), boost::asio::use_awaitable);

        // Start sending periodic ping messages to keep the connection alive
        boost::asio::co_spawn(executor, startKeepAlive(m_cancellationSignal.slot()), boost::asio::detached);
    }
    catch (const boost::system::system_error &e)
    {
        throw exception::ConnectionClosed(e.what());
    }
};

boost::asio::awaitable<void> Connection::establishSSLConnection(
    const boost::asio::ip::tcp::resolver::results_type &results, const char *host)
{
    try
    {
        auto &tcpStream = boost::beast::get_lowest_layer(m_websocket);
        co_await tcpStream.async_connect(results, boost::asio::use_awaitable);
        tcpStream.expires_after(std::chrono::seconds(30));

        auto &sslStream = m_websocket.next_layer();
        if (!SSL_set_tlsext_host_name(sslStream.native_handle(), host))
        {
            boost::beast::error_code ec(static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category());
            throw boost::beast::system_error{ec};
        }
        co_await sslStream.async_handshake(boost::asio::ssl::stream_base::client, boost::asio::use_awaitable);
        tcpStream.expires_never();
    }
    catch (const boost::system::system_error &e)
    {
        throw exception::ConnectionClosed(e.what());
    }
}

boost::asio::awaitable<void> Connection::disconnect()
{
    m_cancellationSignal.emit(boost::asio::cancellation_type::all);
    // Cancel all pending asynchronous operations
    m_websocket.next_layer().next_layer().cancel();

    try
    {
        co_await m_websocket.async_close(boost::beast::websocket::close_code::normal, boost::asio::use_awaitable);
    }
    catch (const boost::system::system_error &e)
    {
        if (e.code() == boost::asio::error::operation_aborted || e.code() == boost::asio::error::eof)
        {
            // operation_aborted or eof is expected when the connection is closed by remote peer
            co_return;
        }
    }
};

boost::asio::awaitable<void> Connection::makeRequest(const boost::json::object &command)
{
    const auto currentTime = std::chrono::system_clock::now();
    const auto duration = currentTime - m_lastRequestTime;
    if (duration < m_requestTimeout)
    {
        std::this_thread::sleep_for(m_requestTimeout - duration);
    }

    try
    {
        const std::string message = boost::json::serialize(command);
        co_await m_websocket.async_write(boost::asio::buffer(message), boost::asio::use_awaitable);
        m_lastRequestTime = std::chrono::system_clock::now();
    }
    catch (const boost::system::system_error &e)
    {
        throw exception::ConnectionClosed(e.what());
    }
}

boost::asio::awaitable<boost::json::object> Connection::waitResponse()
{
    boost::beast::flat_buffer buffer;
    try
    {
        co_await m_websocket.async_read(buffer, boost::asio::use_awaitable);
        auto dataString = boost::beast::buffers_to_string(buffer.data());
        buffer.consume(buffer.size());

        boost::json::object jsonData = boost::json::parse(dataString).as_object();

        co_return jsonData;
    }
    catch (const boost::system::system_error &e)
    {
        if (e.code() == boost::asio::error::eof)
        {
            throw exception::ConnectionClosed("Connection closed by remote host");
        }
        else
        {
            throw exception::ConnectionClosed(e.what());
        }
    }
}

boost::asio::awaitable<void> Connection::startKeepAlive(boost::asio::cancellation_slot cancellationSlot)
{
    const auto executor = co_await boost::asio::this_coro::executor;
    boost::asio::steady_timer pingTimer(executor);
    const auto pingInterval = std::chrono::seconds(20);

    cancellationSlot.assign([&]([[maybe_unused]] boost::asio::cancellation_type type) {
        if (type == boost::asio::cancellation_type::all)
        {
            pingTimer.cancel();
        }
    });

    while (true)
    {
        try
        {
            pingTimer.expires_after(pingInterval);
            co_await pingTimer.async_wait(boost::asio::use_awaitable);

            if (m_websocket.is_open())
            {
                co_await m_websocket.async_ping({}, boost::asio::use_awaitable);
            }
            else
            {
                break;
            }
        }
        catch (const boost::system::system_error &e)
        {
            throw exception::ConnectionClosed(e.what());
        }
    }
}

} // namespace internals
} // namespace xapi
