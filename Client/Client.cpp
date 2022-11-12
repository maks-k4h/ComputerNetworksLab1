//
// Created by Maks Konevych on 04.11.2022.
//

#include "Client.h"

#include "../MSP/connect/TcpIPv4Connector.h"
#include "../MSP/MspConnection.h"

#include <iostream>
#include <utility>


Client::Client(std::string logFilePath)
: stringStorage_(0), logger_(std::move(logFilePath))
{
}

void Client::run()
{
    TcpIPv4Connector connector(&logger_);

    try
    {
        auto tcpIPv4Connection = connector.connect("localhost", "1026");
        auto mspConnection = MspConnection(std::move(tcpIPv4Connection), MspConnection::CLIENT);
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what();
    }

}
