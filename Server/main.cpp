//
// Created by Maks Konevych on 03.11.2022.
//

#include <iostream>
#include "Server.h"

int main()
{
    // it's almost hardcoding, though it appears only here, so be free to change;
    std::string logFilePath = "/Users/makskonevych/Documents/Cpp/Labs/CNLab/serverlogs.txt";

    std::cout << "Log file: " << logFilePath << "\n\n";
    Server server(logFilePath);
    server.run();
}