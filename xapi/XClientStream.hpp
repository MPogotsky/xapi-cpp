#pragma once

#include "Stream.hpp"

namespace xapi
{

class XClientStream final : public internals::Stream
{
  public:
    XClientStream() = delete;

    XClientStream(const XClientStream &) = delete;
    XClientStream &operator=(const XClientStream &) = delete;

    XClientStream(XClientStream &&other) = default;
    XClientStream &operator=(XClientStream &&other) = delete;

    /**
     * @brief Constructs a new XClientStream object.
     * @param ioContext The IO context for asynchronous operations.
     */
    explicit XClientStream(boost::asio::io_context &ioContext, const std::string &accountType, const std::string &streamSessionId);
    ~XClientStream() override = default;


    boost::asio::awaitable<void> open();

    boost::asio::awaitable<void> close();

  private:
    const std::string m_streamSessionId;
    const boost::url m_streamUrl;
};

} // namespace xapi
