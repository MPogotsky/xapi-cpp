#include <iostream>
#include <memory>
#include <string>

#include <boost/asio.hpp>
#include <jsoncpp/json/json.h>
#include <xapi/Xapi.hpp>

boost::asio::awaitable<void> run(boost::asio::io_context &context)
{
    xapi::Socket socket(context);
    xapi::Stream stream(context);

    // Add your credentials here
    std::string accountId = "accountId";
    std::string password = "password";
    std::string host = "ws.xtb.com";
    std::string type = "demo";
    bool safe = true;

    try
    {
        co_await socket.initSession(host, type);
        auto streamsessionId = co_await socket.login(accountId, password);

        co_await stream.initSession(host, type, streamsessionId);
        co_await stream.getBalance();
        int counter = 0;
        while (counter < 5)
        {
            std::cout << "Processing " << counter << std::endl;
            auto result = co_await stream.listen();
            Json::StreamWriterBuilder writer;
            auto resultStr = Json::writeString(writer, result);
            std::cout << resultStr << std::endl;

            counter += 1;
        }

        co_await stream.closeSession();
        co_await socket.closeSession();
    }
    catch (xapi::exception::ConnectionClosed &e)
    {
        std::cout << "Connection failed: " << e.what() << std::endl;
    }
    catch (xapi::exception::LoginFailed &e)
    {
        std::cout << "Logging failed: " << e.what() << std::endl;
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    boost::asio::io_context context;

    boost::asio::co_spawn(context, run(std::ref(context)), boost::asio::detached);

    context.run();
    return 0;
}
