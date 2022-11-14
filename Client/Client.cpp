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

        std::cout << "> Enter 'h' to get help.\n";

        while (true)
        {
            process(readUsersCommand(), mspConnection);
        }

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

void Client::updateCommand(MspConnection& connection)
{
    connection.update(stringStorage_);
    renderStrings();
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


void Client::process(const std::string& command, MspConnection& connection)
{
    if (command == "h" || command == "help")
    {
        helpCommand();
    }
    if (command == "u" || command == "update")
    {
        updateCommand(connection);
    }
    if (command == "who")
    {
        whoCommand(connection);
    }
}

std::string Client::readUsersCommand()
{
    std::cout << "> ";
    std::string s;

    // trimming whitespaces... (c++ mofo)
    int start {}, end {(int)s.length() - 1};
    while (start < s.length() && isspace(s[start])) ++start;
    while (end >=0 && isspace(s[end])) --end;
    s = s.substr(start, int(s.length()) - start);

    // here we go again...
    for (auto& c : s)
        c = tolower(c);

    getline(std::cin, s);
    return s;
}

void Client::helpCommand()
{
    std::cout
    << "> Xml Viewer (v1.0).\n"
    << "> Available commands:\n"
    << "    u       | update        update strings;\n"
    << "    who                     update strings;\n"
    << "    h       | help          get help;\n";
}

void Client::whoCommand(MspConnection& connection)
{
    auto s = connection.requestWho();
    std::cout << "> " << s << '\n';
}



