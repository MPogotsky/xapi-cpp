#pragma once

#include "Socket.hpp"
#include "Stream.cpp"

namespace xapi
{

class Xapi
{
  public:
    boost::asio::awaitable<void> connect(const std::string &accountId, const std::string &password,
                                         const std::string &host = "ws.xtb.com", const std::string &type = "real",
                                         bool safeMode = false);
    boost::asio::awaitable<void> disconnect();

  private:
    Socket m_socket;
    Stream m_stream;
};

} // namespace xapi
