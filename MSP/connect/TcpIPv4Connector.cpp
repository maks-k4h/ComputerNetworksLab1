//
// Created by Maks Konevych on 11.11.2022.
//

#include "TcpIPv4Connector.h"
#include "TcpIPv4Exception.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <utility>
#include <sys/socket.h>


TcpIPv4Connector::TcpIPv4Connector(Logger* logger)
: logger_{logger}
{
}

TcpIPv4Connector::~TcpIPv4Connector()
{
    stopListening();
}

TcpIPv4Connection TcpIPv4Connector::connect(const std::string& host, const std::string& service)
{
    if (listening)
        throw TcpIPv4Exception("connect(" + host + "," + service +
        ") error: You are already listening");

    auto aiw = getAddrInfo(service, host);
    auto ai = aiw.getAddrInfo();

    // getting a file descriptor
    auto fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);

    if (fd == -1)
    {
        auto m = "Cannot get file descriptor from socket("+
                 std::to_string(ai->ai_family) + "," +
                 std::to_string(ai->ai_socktype) + "," +
                 std::to_string(ai->ai_protocol) +
                 ").";

        logError(m);
        throw TcpIPv4Exception(m);
    }

    if (::connect(fd, ai->ai_addr, ai->ai_addrlen) == -1)
    {
        auto m = "connect(..) to " + host + ":" + service + " failed.";
        logError(m);
        throw TcpIPv4Exception(m);
    }

    logStatus("Connected to " + host + ":" + service + ". ");

    return TcpIPv4Connection(fd, logger_);
}

void TcpIPv4Connector::listen(const std::string& service,int backlog)
{
    if (listening)
        throw TcpIPv4Exception("listen(" + service + ","
        + std::to_string(backlog) + ") error: You are already listening");

    auto aiw = getAddrInfo(service);
    auto ai = aiw.getAddrInfo();

    // getting a file descriptor
    auto fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);

    if (fd == -1)
    {
        auto m = "Cannot get file descriptor from socket("+
                 std::to_string(ai->ai_family) + "," +
                 std::to_string(ai->ai_socktype) + "," +
                 std::to_string(ai->ai_protocol) +
                 ").";

        logError(m);
        throw TcpIPv4Exception(m);
    }

    if (::bind(fd, ai->ai_addr, ai->ai_addrlen) < 0)
    {
        auto m = "bind(..) failed.";
        logError(m);
        throw TcpIPv4Exception(m);
    }

    if (::listen(fd, 5) < 0)
    {
        auto m = "listen(..) failed.";
        logError(m);
        throw TcpIPv4Exception(m);
    }

    fd_ = fd;
    listening = true;

    logStatus("Listening on service " + service + " with backlog of "
    + std::to_string(backlog) + ", file descriptor is "
    + std::to_string(fd_) + ".");
}

TcpIPv4Connection TcpIPv4Connector::accept()
{
    if (!listening)
    {
        std::string m = "Cannot accept request without listening to them.";
        logError(m);
        throw TcpIPv4Exception(m);
    }

    sockaddr_storage theirAddr {};
    socklen_t addrLen = sizeof(theirAddr);
    int newFd;

    newFd = ::accept(fd_, (sockaddr*)&theirAddr, &addrLen);

    if (newFd == -1)
    {
        std::string m = "Cannot accept request on socket with file descriptor "
                + std::to_string(fd_) + ".";
        logError(m);
        throw TcpIPv4Exception(m);
    }

    char s[INET_ADDRSTRLEN];
    inet_ntop(theirAddr.ss_family, ((in_addr *)&theirAddr), s, sizeof s);

    logStatus("Accepted new request on socket with file descriptor "
    + std::to_string(fd_) + ". Requested from " + s + ". The created "
    "file descriptor is " + std::to_string(newFd) + ".");

    return TcpIPv4Connection(newFd, logger_);
}

void TcpIPv4Connector::stopListening()
{
    if (listening)
    {
        ::close(fd_);
        listening = false;
        logStatus("Stopped listening on fd " + std::to_string(fd_));
    }
}

TcpIPv4Connector::AddrInfoWrapper TcpIPv4Connector::getAddrInfo(const std::string& service, const std::string& host)
{
    addrinfo hints {};
    addrinfo* res;

    // setting hints
    hints.ai_family     = AF_INET;
    hints.ai_socktype   = SOCK_STREAM;
    hints.ai_protocol   = IPPROTO_TCP;
    hints.ai_flags      = host.length() > 0 ? AI_DEFAULT : AI_PASSIVE;

    // getting addrinfo from hints and host parameters
    int status;
    if ((status = getaddrinfo(host.length() > 0 ? host.c_str() : nullptr, service.c_str(), &hints, &res)) != 0)
    {
        logError(std::string("getaddrinfo error: ") + gai_strerror(status));
        throw TcpIPv4Exception(std::string("getaddrinfo error: ") + gai_strerror(status));
    }

    return AddrInfoWrapper(res);
}

void TcpIPv4Connector::logStatus(const std::string& s)
{
    if (logger_)
        logger_->logStatus(s);
}

void TcpIPv4Connector::logError(const std::string& s)
{
    if (logger_)
        logger_->logError(s);
}

TcpIPv4Connector::AddrInfoWrapper::AddrInfoWrapper(addrinfo *ai)
: ai_{ai}
{
}

TcpIPv4Connector::AddrInfoWrapper::~AddrInfoWrapper()
{
    freeaddrinfo(ai_);
}

addrinfo *TcpIPv4Connector::AddrInfoWrapper::getAddrInfo() const
{
    return ai_;
}
