//
// Created by Maks Konevych on 03.11.2022.
//

#include <iostream>
#include "Client.h"

int main()
{
    // it's almost hardcoding, though it appears only here, so be free to change;
    std::string logFilePath = "/Users/makskonevych/Documents/Cpp/Labs/CNLab/clientlogs.txt";
    Client client(logFilePath);
    client.run();
}



