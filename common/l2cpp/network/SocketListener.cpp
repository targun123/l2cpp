/// @author    Chnossos
/// @date      Created on 2026-02-16

#include "SocketListener.hpp"

#include <spdlog/spdlog.h>

#include <functional>
#include <unordered_map>

namespace ip = boost::asio::ip;

using boost::system::error_code;

using SL = l2cpp::Network::SocketListener;
using TcpAcceptor = ip::tcp::acceptor;

struct SL::SocketListenerImpl
{
    boost::asio::io_context &               ioContext;
    std::unordered_map<u16, TcpAcceptor>    acceptors;
    std::unordered_map<u16, AcceptCallback> callbacks;

    explicit SocketListenerImpl(boost::asio::io_context & ioc)
        : ioContext(ioc)
    {}
    ~SocketListenerImpl() = default;

    bool listen(ip::address addr, u16 port, AcceptCallback cb);
    void onAccept(u16 port, error_code const & ec, ip::tcp::socket socket);
    bool handleError(u16 port, error_code const & ec);
};

SL::SocketListener(boost::asio::io_context & ioc)
    : impl(std::make_unique<SocketListenerImpl>(ioc))
{}

SL::~SocketListener() = default;

bool SL::listen(std::string_view const host, u16 const port, AcceptCallback cb)
{
    error_code ec;
    auto addr = ip::make_address(host.data(), ec);
    return ec ? impl->handleError(port, ec) : impl->listen(std::move(addr), port, std::move(cb));
}

void SL::shutdown()
{
    SPDLOG_INFO("[SocketListener] Shutting down all ports");

    error_code ec;
    for (auto & [port, acceptor] : impl->acceptors)
        impl->handleError(port, acceptor.cancel(ec));

    impl->acceptors.clear();
}

bool SL::SocketListenerImpl::listen(ip::address addr, u16 const port, AcceptCallback cb)
{
    ip::tcp::acceptor acceptor { ioContext };
    try
    {
        ip::tcp::endpoint const endpoint { addr, port };
        acceptor.open(endpoint.protocol());
        acceptor.set_option(ip::tcp::acceptor::reuse_address{true});
        acceptor.bind(endpoint);
        acceptor.listen(boost::asio::socket_base::max_listen_connections);
    }
    catch (error_code const & ec)
    {
        return handleError(port, ec);
    }

    using namespace std::placeholders;
    acceptor.async_accept(std::bind(&SocketListenerImpl::onAccept, this, port, _1, _2));

    acceptors.emplace(port, std::move(acceptor));
    callbacks.emplace(port, std::move(cb));

    SPDLOG_INFO("[SocketListener] Listening on {}:{}", addr.to_string(), port);
    return true;
}

void SL::SocketListenerImpl::onAccept(u16 port, error_code const & ec, ip::tcp::socket socket)
{
    if (!handleError(port, ec)) return;

    using namespace std::placeholders;
    std::invoke(callbacks.at(port), std::move(socket));
    acceptors.at(port).async_accept(std::bind(&SocketListenerImpl::onAccept, this, port, _1, _2));
}

bool SL::SocketListenerImpl::handleError(u16 port, error_code const & ec)
{
    switch (auto const code = ec.default_error_condition().value())
    {
        namespace errc = boost::system::errc;

        case errc::success:
            return true;

        case errc::operation_canceled:
            SPDLOG_INFO("[SocketListener] [{}] shut down successfully", port);
            break;

        default:
            SPDLOG_ERROR("[SocketListener] [{}] error {}: {}", port, code, ec.message());
            break;
    }
    return false;
}
