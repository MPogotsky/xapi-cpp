#include <iostream>
#include <memory>
#include <string>

#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <xapi/Xapi.hpp>

boost::asio::awaitable<void> printCandles(xapi::XClientStream &stream)
{
    co_await stream.getCandles("US100");
    // Use getKeepAlive, to keep the connection alive
    co_await stream.getKeepAlive();

    // Print 5 candle results from the stream
    int counter = 0;
    while (counter < 5)
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
    co_await stream.stopKeepAlive();

    co_return;
}

boost::asio::awaitable<void> run(boost::asio::io_context &context)
{
    const boost::json::object accountCredentials = {
        {"accountId", "accountId"},
        {"password", "password"},
        {"accountType", "demo"}
    };

    xapi::XStationClient user(context, accountCredentials);

    try
    {
        co_await user.login();    
        auto userStream = user.getClientStream();
        co_await userStream.open();

        co_await printCandles(userStream);

        co_await userStream.close();
        co_await user.logout();
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

    boost::asio::co_spawn(context, run(context), boost::asio::detached);

    context.run();
    return 0;
}
