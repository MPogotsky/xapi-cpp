#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <xapi/Xapi.hpp>


boost::asio::awaitable<void> run(boost::asio::io_context &context)
{
    xapi::Socket socket(context);

    // Replace with your actual credentials
    const std::string accountId = "accountId";
    const std::string password = "password";

    // If you are using real account, replace with "real"
    const std::string accountType = "demo";

    try
    {
        co_await socket.initSession(accountType);
        auto streamsessionId = co_await socket.login(accountId, password);
        
        std::cout << "Stream session id: " << streamsessionId << std::endl;

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

    boost::asio::co_spawn(context, run(context), boost::asio::detached);

    context.run();
    return 0;
}
