//
// Created by Maks Konevych on 04.11.2022.
//

#include "Client.h"

#include "../MSP/connect/TcpIPv4Connector.h"


#include <iostream>
#include <utility>


Client::Client(std::string logFilePath)
: stringStorage_(), logger_(std::move(logFilePath))
{
}

void Client::run()
{
    TcpIPv4Connector connector(&logger_);

    try
    {
        auto tcpIPv4Connection = connector.connect("localhost", "1026");
        auto mspConnection = MspConnection(std::move(tcpIPv4Connection), MspConnection::CLIENT);

        update(mspConnection);
        renderStrings();

    }
    catch (TcpIPv4ClosedConnectionException& e)
    {
        std::cout << "The connection is closed\n";
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what();
    }
}

void Client::update(MspConnection& connection)
{
    connection.update(stringStorage_);
}

void Client::renderStrings()
{
    std::cout << "Number of strings: " << stringStorage_.size() << "\n";

    for (int i = 0; i < stringStorage_.size(); ++i)
    {
        auto node = stringStorage_.getById(i);
        std::cout << "Id " << node.id_ << ": " << node.data_ << std::endl;
    }
}


