//
// Created by Maks Konevych on 11.11.2022.
//

#include "TcpIPv4Exception.h"

#include <utility>

TcpIPv4Exception::TcpIPv4Exception(std::string  message)
: message_{std::move(message)}
{
}

const char *TcpIPv4Exception::what() const _NOEXCEPT
{
    return message_.c_str();
}
