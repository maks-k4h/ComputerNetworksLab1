//
// Created by Maks Konevych on 04.11.2022.
//

#ifndef SERVER_SERVER_H_
#define SERVER_SERVER_H_

#include <iostream>
#include <thread>

#include "../MSP/connect/TcpIPv4Connector.h"
#include "../MSP/MspConnection.h"
#include "../libs/StringStorage.h"

class Server {  // TODO: make sure it's thread-save.
public:
    explicit Server(const std::string& logFilePath);

    void run();

private:
    Logger logger_;
    StringStorage storage_;

    friend void ProcessConnection(Server*, TcpIPv4Connection); // helper

};


#endif //SERVER_SERVER_H_
