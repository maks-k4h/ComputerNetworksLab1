//
// Created by Maks Konevych on 13.11.2022.
//

#include "TcpIPv4ClosedConnectionException.h"

#include <utility>

TcpIPv4ClosedConnectionException::TcpIPv4ClosedConnectionException(std::string message)
: TcpIPv4Exception(std::move(message))
{
}
