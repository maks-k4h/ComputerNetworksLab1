//
// Created by Maks Konevych on 04.11.2022.
//

#include "Client.h"
#include <iostream>

void Client::run()
{
    auto logger = Logger("/Users/makskonevych/Documents/Cpp/Labs/CNLab/clientlogs.txt");
    TcpIPv4Connector connector(&logger);

    try
    {
        auto connection = connector.connect("localhost", "1026");
        connection.send("Hello, server!");
        auto m = connection.receive();
        std::cout << "Received: " << m;
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what();
    }

}
