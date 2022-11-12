//
// Created by Maks Konevych on 11.11.2022.
//

#ifndef MSP_CONNECT_TCPIPV4EXCEPTION_H_
#define MSP_CONNECT_TCPIPV4EXCEPTION_H_

#include <exception>
#include <string>

class TcpIPv4Exception : public std::exception
{
public:
    explicit TcpIPv4Exception(std::string  message);

    const char* what() const _NOEXCEPT override;

private:
    std::string message_;
};


#endif //MSP_CONNECT_TCPIPV4EXCEPTION_H_
