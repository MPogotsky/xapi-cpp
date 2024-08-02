#include "Connection.hpp"
#include "Exceptions.hpp"

#include <boost/url.hpp>

namespace xapi
{

boost::asio::awaitable<void> Connection::connect(const std::string &url)
{
    namespace ip = boost::asio::ip;
    namespace asio = boost::asio;

    const auto executor = co_await asio::this_coro::executor;
    ip::tcp::resolver resolver(executor);

    const auto parsedUrl = boost::urls::parse_uri(url).value();

    try
    {
        auto const results = co_await resolver.async_resolve(parsedUrl.host(), parsedUrl.port(), asio::use_awaitable);
        co_await asio::async_connect(m_websocket.next_layer(), results, asio::use_awaitable);
        co_await m_websocket.async_handshake(parsedUrl.host(), parsedUrl.path(), asio::use_awaitable);
        m_connectionEstablished = true;
    }
    catch (const boost::system::system_error &e)
    {
        throw exception::ConnectionClosed(e.what());
    }
};

boost::asio::awaitable<void> Connection::disconnect()
{
    namespace asio = boost::asio;
    if (m_connectionEstablished)
    {
        co_await m_websocket.async_close(boost::beast::websocket::close_code::normal, asio::use_awaitable);
        m_connectionEstablished = false;
    }
};

boost::asio::awaitable<Json::Value> Connection::listen()
{
    namespace asio = boost::asio;
    boost::beast::flat_buffer buffer;
    try
    {
        co_await m_websocket.async_read(buffer, asio::use_awaitable);
        auto dataString = boost::beast::buffers_to_string(buffer.data());
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
    catch (const std::exception &e)
    {
        throw e;
    }
};

boost::asio::awaitable<void> Connection::request(const Json::Value &command)
{
    namespace asio = boost::asio;

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
    catch (const std::exception &e)
    {
        throw e;
    }
}

boost::asio::awaitable<Json::Value> Connection::transaction(const Json::Value &command)
{
    namespace asio = boost::asio;
    boost::beast::flat_buffer buffer;
    try
    {
        co_await request(command);
        co_await m_websocket.async_read(buffer, asio::use_awaitable);
        const auto dataString = boost::beast::buffers_to_string(buffer.data());
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
    catch (const std::exception &e)
    {
        throw e;
    }
}

} // namespace xapi
