//
// Created by Maks Konevych on 04.11.2022.
//

#include "StringStorage.h"

#include <utility>

StringStorage::StringStorage(int stringNumber)
{
    for (int i = 0; i < stringNumber; ++i)
        nodes_.emplace_back(Node{(IdType)i, (HashType)0, ""});
}

StringStorage::Node StringStorage::getById(StringStorage::IdType id)
{
    auto v = std::find_if(nodes_.begin(), nodes_.end(),
                          [id](Node& n)
                          {
                              return n.id_ == id;
                          } );

    if (v == nodes_.end())
        throw StringStorageException("No node with id " + std::to_string(id) + "found");

    return *v;
}

void StringStorage::setDataById(StringStorage::IdType id, const std::string& data)
{
    if (data.size() > MAX_STRING_L)
        throw StringStorageException("Attempt to set too long string as a node's value.");

    auto v = std::find_if(nodes_.begin(), nodes_.end(),
                          [id](Node& n)
                          {
                              return n.id_ == id;
                          } );

    if (v == nodes_.end())
        throw StringStorageException("No node with id " + std::to_string(id) + "found");

    v->data_ = data;
    ++v->hash_;
}

StringStorageException::StringStorageException(std::string message)
:message_{std::move(message)}
{
}

const char *StringStorageException::what() const _NOEXCEPT
{
    return message_.c_str();
}
