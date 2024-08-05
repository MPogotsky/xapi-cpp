#pragma once

#include "Socket.hpp"
#include "Stream.hpp"

namespace xapi
{

class Xapi
{
  public:
    Xapi(boost::asio::io_context &context) : socket(context), stream(context) {};
    boost::asio::awaitable<void> initSession(const std::string &accountId, const std::string &password,
                                         const std::string &host = "ws.xtb.com", const std::string &type = "real",
                                         bool safeMode = false);
    boost::asio::awaitable<void> closeSession();

    Socket socket;
    Stream stream;
};

} // namespace xapi
