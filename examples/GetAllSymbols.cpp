#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <xapi/Xapi.hpp>


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

        auto result = co_await user.getAllSymbols();
        std::cout << boost::json::serialize(result) << std::endl;

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
