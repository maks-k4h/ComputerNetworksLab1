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

class Server {
public:
    explicit Server(const std::string& logFilePath);

    void run();

private:
    Logger logger_;
    StringStorage storage_;

    friend void processConnection(Server*, TcpIPv4Connection);
    friend void processRequest(Server*, MspConnection&);

    void processCCRequest(MspConnection& connection, std::string commands);
    void processICRequest(MspConnection& connection, std::string commands);
    void processRCRequest(MspConnection& connection, std::string commands);
    std::mutex mutex_;
};


#endif //SERVER_SERVER_H_
