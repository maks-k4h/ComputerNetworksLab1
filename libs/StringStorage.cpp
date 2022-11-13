//
// Created by Maks Konevych on 04.11.2022.
//

#include "StringStorage.h"

#include <utility>

StringStorage::StringStorage()
= default;

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

void StringStorage::setNode(const StringStorage::Node& node)
{
    try
    {
        getById(node.id_);
        setDataById(node.id_, node.data_);
    }
    catch (...)
    {
        if (nodes_.size() >= MAX_STRING_N)
            throw StringStorageException("Number of strings overflow.");
        nodes_.emplace_back(node);
    }
}

void StringStorage::addString(std::string s)
{
    if (nodes_.empty())
    {
        nodes_.emplace_back(Node{0, 0, s});
    }
    else
    {
        nodes_.emplace_back(Node{(nodes_.end() - 1)->id_ + 1, 0, s});
    }
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
