#include "Connection.hpp"
#include "Exceptions.hpp"

#include <boost/url.hpp>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace ip = boost::asio::ip;

namespace xapi
{

Connection::Connection(boost::asio::io_context &ioContext)
    : m_ioContext(ioContext), m_sslContext(boost::asio::ssl::context::tlsv13_client),
      m_websocket(m_ioContext, m_sslContext), m_lastRequestTime(std::chrono::system_clock::now()),
      m_connectionEstablished(false)
{
}

boost::asio::awaitable<void> Connection::connect(const std::string &url)
{
    const auto executor = co_await asio::this_coro::executor;
    try
    {
        const auto parsedUrl = boost::urls::parse_uri(url).value();

        ip::tcp::resolver resolver(executor);
        auto const results =
            co_await resolver.async_resolve(parsedUrl.host(), m_websocketDefaultPort, asio::use_awaitable);

        co_await establishSSLConnection(results, parsedUrl.host().c_str());

        co_await m_websocket.async_handshake(parsedUrl.host(), parsedUrl.path(), asio::use_awaitable);
        m_connectionEstablished = true;
    }
    catch (const boost::system::system_error &e)
    {
        throw exception::ConnectionClosed(e.what());
    }
};

boost::asio::awaitable<void> Connection::establishSSLConnection(boost::asio::ip::tcp::resolver::results_type results,
                                                                const char *host)
{
    try
    {
        auto &tcpStream = beast::get_lowest_layer(m_websocket);
        co_await tcpStream.async_connect(results, asio::use_awaitable);
        tcpStream.expires_after(std::chrono::seconds(30));

        auto &sslStream = m_websocket.next_layer();
        if (!SSL_set_tlsext_host_name(sslStream.native_handle(), host))
        {
            beast::error_code ec(static_cast<int>(::ERR_get_error()), asio::error::get_ssl_category());
            throw beast::system_error{ec};
        }
        co_await sslStream.async_handshake(asio::ssl::stream_base::client, asio::use_awaitable);

        tcpStream.expires_never();
    }
    catch (const boost::system::system_error &e)
    {
        throw exception::ConnectionClosed(e.what());
    }
}

boost::asio::awaitable<void> Connection::disconnect()
{
    if (m_connectionEstablished)
    {
        co_await m_websocket.async_close(beast::websocket::close_code::normal, asio::use_awaitable);
        m_connectionEstablished = false;
    }
};

boost::asio::awaitable<void> Connection::makeRequest(const Json::Value &command)
{
    const auto currentTime = std::chrono::system_clock::now();
    const auto duration = currentTime - m_lastRequestTime;
    if (duration < m_requestTimeout)
    {
        std::this_thread::sleep_for(m_requestTimeout - duration);
    }

    try
    {
        const Json::StreamWriterBuilder writer;
        const std::string message = Json::writeString(writer, command);
        co_await m_websocket.async_write(asio::buffer(message), asio::use_awaitable);
        m_lastRequestTime = std::chrono::system_clock::now();
    }
    catch (const boost::system::system_error &e)
    {
        throw exception::ConnectionClosed(e.what());
    }
}

boost::asio::awaitable<Json::Value> Connection::waitResponse()
{
    boost::beast::flat_buffer buffer;
    try
    {
        co_await m_websocket.async_read(buffer, asio::use_awaitable);
        auto dataString = beast::buffers_to_string(buffer.data());
        buffer.consume(buffer.size());

        Json::Value jsonData;
        Json::Reader reader;
        reader.parse(dataString, jsonData);

        co_return jsonData;
    }
    catch (const boost::system::system_error &e)
    {
        throw exception::ConnectionClosed(e.what());
    }
}

} // namespace xapi
