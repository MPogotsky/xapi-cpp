#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <xapi/Xapi.hpp>

boost::asio::awaitable<void> printBalance(xapi::XStationClientStream &stream)
{
    co_await stream.getBalance();
    // Use getKeepAlive, to keep the connection alive
    co_await stream.getKeepAlive();

    int counter = 0;
    while (counter < 5)
    {
        std::cout << "Processing " << counter << std::endl;
        auto result = co_await stream.listen();
        std::cout << boost::json::serialize(result) << std::endl;
        counter += 1;
    }

    co_await stream.stopKeepAlive();
    co_await stream.stopBalance();

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

        co_await printBalance(userStream);

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
