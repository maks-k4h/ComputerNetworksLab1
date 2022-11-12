//
// Created by Maks Konevych on 04.11.2022.
//

#ifndef LIBS_STRINGSTORAGE_H_
#define LIBS_STRINGSTORAGE_H_

#define MAX_STRING_L 69

#include <string>
#include <vector>
#include <exception>

class StringStorage {
public:

    explicit StringStorage(int stringNumber);

    using IdType    = uint8_t;
    using HashType  = uint8_t;

    struct Node
    {
        IdType id_;
        HashType hash_;
        std::string data_;
    };

    Node getById(IdType id);
    void setDataById(IdType id, const std::string& data);

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
