//
// Created by Maks Konevych on 04.11.2022.
//

#ifndef CLIENT_CLIENT_H_
#define CLIENT_CLIENT_H_

#include <string>
#include <list>

#include "../libs/StringStorage.h"
#include "../libs/Logger.h"

class Client {
public:
    explicit Client(std::string logFilePath);

    void run();

private:
    StringStorage stringStorage_;
    Logger logger_;
};


#endif //CLIENT_CLIENT_H_
