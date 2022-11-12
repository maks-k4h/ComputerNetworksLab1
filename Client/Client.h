//
// Created by Maks Konevych on 04.11.2022.
//

#ifndef CLIENT_CLIENT_H_
#define CLIENT_CLIENT_H_

#include <string>
#include <list>

#include "../libs/StringStorage.h"
#include "../MSP/connect/TcpIPv4Connector.h"

class Client {
public:

    void run();

private:
    StringStorage stringStorage_;
};


#endif //CLIENT_CLIENT_H_
