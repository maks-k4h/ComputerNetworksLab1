//
// Created by Maks Konevych on 04.11.2022.
//

#ifndef LIBS_STRINGSTORAGE_H_
#define LIBS_STRINGSTORAGE_H_

#define MAX_STRING_L 69
#define MAX_STRING_N 255

#include <string>
#include <vector>
#include <exception>

class StringStorage {   // TODO: make sure it's thread-save
public:

    explicit StringStorage();

    using HashType  = uint8_t;

    struct Node
    {
        HashType hash_;
        std::string data_;
    };


    void setNode(int i, const Node& node); // if a node with the same id exists it'll be overridden
    void addString(std::string s);

    Node getById(int i);

    size_t size();

private:
    std::vector<Node> nodes_;
};


class StringStorageException : public std::exception
{
public:
    explicit StringStorageException(std::string  message);

    const char* what() const _NOEXCEPT override;

private:
    std::string message_;
};


#endif //LIBS_STRINGSTORAGE_H_
