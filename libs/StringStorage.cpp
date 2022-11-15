//
// Created by Maks Konevych on 04.11.2022.
//

#include "StringStorage.h"

#include <utility>

StringStorage::StringStorage()
= default;

StringStorage::Node StringStorage::getById(int id)
{
    if (id < 0 || nodes_.size() <= id)
        throw StringStorageException("Cannot access string with id "
        + std::to_string(id) + ".");
    return nodes_[id];
}

void StringStorage::setNode(int i, const StringStorage::Node& node)
{
    if (i == nodes_.size())
        nodes_.emplace_back(node);
    else if (i >= 0 && i < nodes_.size())
        nodes_[i] = node;
    else
        throw StringStorageException("Cannot access requested node.");
}

void StringStorage::addString(std::string s)
{
    if (nodes_.size() >= MAX_STRING_N)
        throw StringStorageException("Number of strings overflow.");
    nodes_.emplace_back(Node{0, std::move(s)});
}

size_t StringStorage::size()
{
    return nodes_.size();
}

StringStorageException::StringStorageException(std::string message)
:message_{std::move(message)}
{
}

const char *StringStorageException::what() const _NOEXCEPT
{
    return message_.c_str();
}
