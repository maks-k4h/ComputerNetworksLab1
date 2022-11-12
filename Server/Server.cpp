//
// Created by Maks Konevych on 04.11.2022.
//

#include "Server.h"


void Server::run()
{
    auto logger = Logger("/Users/makskonevych/Documents/Cpp/Labs/CNLab/serverlogs.txt");

    auto connector = TcpIPv4Connector(&logger);

    try
    {
        connector.listen("1026", 5);
        auto connection = connector.accept();
        auto m = connection.receive();
        std::cout << "Received: " + m;
        connection.send("Hello form server!");
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << '\n';
    }
}