//
// Created by Maks Konevych on 11.11.2022.
//

#ifndef LIBS_CONNECT_TCPIPV4CONNECTION_H_
#define LIBS_CONNECT_TCPIPV4CONNECTION_H_

#include <string>
#include "../../libs/Logger.h"
#include "TcpIPv4Exception.h"
#include <chrono>


class TcpIPv4Connection
{
public:
    explicit TcpIPv4Connection(int fd, Logger* logger = nullptr);
    ~TcpIPv4Connection();

    TcpIPv4Connection(const TcpIPv4Connection&) = delete;
    TcpIPv4Connection& operator=(const TcpIPv4Connection&) = delete;


    TcpIPv4Connection(TcpIPv4Connection&&) noexcept;

    void close();

    void send(const std::string& m);
    std::string receive(int len);

    // If blocking is turned on the method will not return until at least one byte is
    // received or an error has occurred; it's turned on by default;
    // Timeout can be specified; the method will ask to suspend the execution for
    // given timeout; it's .3 seconds by default;
    std::string receive(bool blocking = true, std::chrono::nanoseconds timeout = std::chrono::milliseconds(300));

protected:
    bool connected_;
    int fd_;

    Logger* logger_;

    void logStatus(const std::string& s);
    void logError(const std::string& s);
};



#endif //LIBS_CONNECT_TCPIPV4CONNECTION_H_
