//
// Created by Maks Konevych on 12.11.2022.
//

#include "MspException.h"

#include <utility>

MspException::MspException(std::string message)
: message_{std::move(message)}
{
}

const char *MspException::what() const _NOEXCEPT
{
    return message_.c_str();
}