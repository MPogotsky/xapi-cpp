#include "Xapi.hpp"
#include "Exceptions.hpp"
#include <optional>

namespace xapi
{

boost::asio::awaitable<void> Xapi::connect(const std::string &accountId, const std::string &password,
                                     const std::string &host, const std::string &type,
                                     bool safeMode)
{
    socket.safeMode = safeMode;
    stream.safeMode = safeMode;

    const auto validHost = [&host]() -> std::optional<std::string>
    {
        if(host.starts_with("wss://") or host.starts_with("ws://")) {
            return {};
        }
        return std::string("wss://" + host);
    };
    

    const std::string socketUrl = validHost().value_or(host) + "/" + type;
    const std::string streamUrl = validHost().value_or(host) + "/" + type + "Stream";

    co_await socket.connect(socketUrl);
    co_await stream.connect(streamUrl);

    const auto result = co_await socket.login(accountId, password);

    if(result["status"].asBool() == true) {
        Json::StreamWriterBuilder writer;
        throw exception::LoginFailed(Json::writeString(writer, result));
    }
    stream.streamSessionId = result["streamSessionId"].asString();
}

boost::asio::awaitable<void> Xapi::disconnect()
{
    co_await socket.disconnect();
    co_await stream.disconnect();
}

} // namespace xapi
