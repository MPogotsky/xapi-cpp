#include "XClientStream.hpp"
#include "Exceptions.hpp"

namespace xapi
{

XClientStream::XClientStream(boost::asio::io_context &ioContext, const std::string &accountType, const std::string &streamSessionId)
: Stream(ioContext, streamSessionId), m_streamUrl(boost::urls::format("wss://ws.xtb.com/{}Stream", accountType))
{
}

boost::asio::awaitable<void> XClientStream::open()
{
    co_await connect(m_streamUrl);
}

boost::asio::awaitable<void> XClientStream::close()
{
    co_await disconnect();
}

} // namespace xapi
