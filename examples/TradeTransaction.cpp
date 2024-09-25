#include <iostream>
#include <memory>
#include <string>

#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <xapi/Xapi.hpp>

boost::asio::awaitable<int64_t> executeTransaction(xapi::Socket &socket)
{
    auto response = co_await socket.tradeTransaction("US100", xapi::TradeCmd::BUY_LIMIT, xapi::TradeType::OPEN, 18000.00f,
                                                        0.5f, 0.0f, 0.0f, 0.0f, 0, 0, "");

    if (response["status"].as_bool() != true)
    {
        std::cout << "Failed to execute a transaction: " << boost::json::serialize(response) << std::endl;
        co_return -1;
    }
    auto returnData = boost::json::value_to<boost::json::object>(response.at("returnData"));
    auto order = boost::json::value_to<int64_t>(returnData.at("order"));
    std::cout << "Order: " << order << std::endl;

    co_return order;
}

boost::asio::awaitable<void> checkTransactionStatus(xapi::Socket &socket, int64_t order)
{
    auto response = co_await socket.tradeTransactionStatus(order);
    if (response["status"].as_bool() != true)
    {
        std::cout << "Failed to trade a transaction: " << response << std::endl;
        co_return;
    }

    auto returnData = boost::json::value_to<boost::json::object>(response.at("returnData"));
    auto returnStatus = boost::json::value_to<int>(returnData.at("requestStatus"));
    const auto tradeStatus = static_cast<xapi::TradeStatus>(returnStatus);

    switch (tradeStatus)
    {
        case xapi::TradeStatus::ERROR:
            std::cout << "The transaction finished with error: " << returnData["message"].as_string() << std::endl;
            break;
        case xapi::TradeStatus::PENDING:
            std::cout << "The transaction is pending" << std::endl;
            break;
        case xapi::TradeStatus::ACCEPTED:
            std::cout << "The transaction has been executed successfully" << std::endl;
            break;
        case xapi::TradeStatus::REJECTED:
            std::cout << "The transaction has been rejected: " << returnData["message"].as_string() << std::endl;
            break;
        default:
            std::cout << "Unknown transaction status" << std::endl;
            break;
    }
}

boost::asio::awaitable<void> run(boost::asio::io_context &context)
{
    xapi::Socket socket(context);

    // Add your credentials here
    const std::string accountId = "accountId";
    const std::string password = "password";
    const std::string accountType = "demo";

    socket.safeMode = false;

    try
    {
        co_await socket.initSession(accountType);
        auto streamsessionId = co_await socket.login(accountId, password);

        const auto order_id = co_await executeTransaction(socket);
        co_await checkTransactionStatus(socket, order_id);

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
