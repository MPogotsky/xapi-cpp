#include <iostream>
#include <memory>
#include <string>

#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <xapi/Xapi.hpp>

boost::asio::awaitable<void> printCandles(xapi::Stream &stream)
{
    co_await stream.getCandles("US100");
    // Use getKeepAlive, to keep the connection alive
    co_await stream.getKeepAlive();

    // Print 4 candle results from the stream
    int counter = 0;
    while (counter < 4)
    {
        auto result = co_await stream.listen();

        // Check for result type, as you`ll get keep alive messages
        // every 3 seconds and candle messages every minute
        if (result["command"] == "candle")
        {
            std::cout << boost::json::serialize(result) << std::endl;
            counter += 1;
        }
    }

    co_await stream.stopCandles("US100");
}

boost::asio::awaitable<void> run(boost::asio::io_context &context)
{
    xapi::Socket socket(context);
    xapi::Stream stream(context);

    // Add your credentials here
    const std::string accountId = "accountId";
    const std::string password = "password";
    const std::string accountType = "demo";

    try
    {
        co_await socket.initSession(accountType);
        auto streamsessionId = co_await socket.login(accountId, password);
        co_await stream.initSession(accountType, streamsessionId);
        
        co_await printCandles(stream);

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
