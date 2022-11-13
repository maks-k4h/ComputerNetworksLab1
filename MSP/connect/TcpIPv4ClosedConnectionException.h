//
// Created by Maks Konevych on 13.11.2022.
//

#ifndef MSP_CONNECT_TCPIPV4CLOSEDCONNECTIONEXCEPTION_H_
#define MSP_CONNECT_TCPIPV4CLOSEDCONNECTIONEXCEPTION_H_

#include "TcpIPv4Exception.h"

class TcpIPv4ClosedConnectionException : public TcpIPv4Exception
{
public:
    explicit TcpIPv4ClosedConnectionException(std::string message);
};


#endif //MSP_CONNECT_TCPIPV4CLOSEDCONNECTIONEXCEPTION_H_
