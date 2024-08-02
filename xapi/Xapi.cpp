#include "Xapi.hpp"
#include "Exceptions.hpp"
#include <optional>

namespace xapi
{

boost::asio::awaitable<void> Xapi::connect(const std::string &accountId, const std::string &password,
                                     const std::string &host, const std::string &type,
                                     bool safeMode)
{
    m_socket.safeMode = safeMode;
    m_stream.safeMode = safeMode;

    const auto validHost = [&host]() -> std::optional<std::string>
    {
        if(host.starts_with("wss://") or host.starts_with("ws://")) {
            return {};
        }
        return std::string("wss://" + host);
    };
    

    const std::string socketUrl = validHost().value_or(host) + "/" + type;
    const std::string streamUrl = validHost().value_or(host) + "/" + type + "Stream";

    co_await m_socket.connect(socketUrl);
    co_await m_stream.connect(streamUrl);

    const auto result = co_await m_socket.login(accountId, password);

    if(result["status"].asBool() == true) {
        Json::StreamWriterBuilder writer;
        throw exception::LoginFailed(Json::writeString(writer, result));
    }
    m_stream.streamSessionId = result["streamSessionId"].asString();
}

boost::asio::awaitable<void> Xapi::disconnect()
{
    co_await m_socket.disconnect();
    co_await m_stream.disconnect();
}

} // namespace xapi
