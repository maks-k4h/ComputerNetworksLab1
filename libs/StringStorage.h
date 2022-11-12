//
// Created by Maks Konevych on 04.11.2022.
//

#ifndef LIBS_STRINGSTORAGE_H_
#define LIBS_STRINGSTORAGE_H_

#define MAX_STRING_L 200

#include <string>
#include <vector>
#include <exception>

class StringStorage {
public:

    using IdType    = uint8_t;
    using HashType  = uint8_t;

    struct Node
    {
        IdType id;
        std::string data;
        HashType hash;
    };

    Node getById(IdType id)
    {
        auto v = std::find_if(nodes_.begin(), nodes_.end(),
        [id](Node& n)
        {
            return n.id == id;
        } );

        if (v == nodes_.end())
            throw std::exception();
    }

private:
    std::vector<Node> nodes_;
};


#endif //LIBS_STRINGSTORAGE_H_
