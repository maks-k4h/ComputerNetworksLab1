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
        std::cout << "> The connection is closed\n";
    }
    catch (std::exception& e)
    {
        std::cout << "> Exception: " << e.what();
    }
}

void Client::updateCommand(MspConnection& connection)
{
    connection.update(stringStorage_);
    renderStrings();
}

void Client::renderStrings()
{
    std::cout << "> Number of strings: " << stringStorage_.size() << "\n";

    for (int i = 0; i < stringStorage_.size(); ++i)
    {
        auto node = stringStorage_.getById(i);
        std::cout << '\t' << i + 1 << ": " << node.data_ << '\n';
    }
}


void Client::process(const std::string& command, MspConnection& connection)
{
    if (command == "h" || command == "help")
    {
        helpCommand();
    }
    else if (command == "u" || command == "update")
    {
        updateCommand(connection);
    }
    else if (command == "w" || command == "who")
    {
        whoCommand(connection);
    }
    else if (command.substr(0, 2) == "cc")
    {
        ccCommand(connection);
    }
    else if (command.substr(0, 2) == "ic")
    {
        icCommand(connection);
    }
    else if (command.substr(0, 2) == "rc")
    {
        rcCommand(connection);
    }
    else
    {
        std::cout << "> Unknown command!\n";
    }

    // cleaning the input
    while (std::cin.good() && std::cin.get() != '\n');
}

std::string Client::readUsersCommand()
{
    std::cout << "> ";
    std::string s;
    if (std::cin >> s) {
        return s;
    }

    std::cin.clear();
    while (std::cin.good() && std::cin.get() != '\n');

    return "";
}

void Client::helpCommand()
{
    std::cout
    << "> Xml Viewer (v1.0).\n"
    << "> Available commands:\n"
    << "    u       | update        update strings;\n"
    << "    w       | who           update strings;\n"
    << "    h       | help          get help;\n"
    << "    cc <sId> <pc> <c>       change character <pc> in string <sId> to <c>;\n"
    << "    ic <sId> <pc> <c> <ba>  insert character <c> after <pc> in string <sId>,\n"
    << "                            <c> will be inserted after <pc> if <ba> is 'a'\n"
    << "                            and before if it's 'b'.\n"
    << "    rc <sId> <pc>           remove character <pc> form string <sId>;\n";
}

void Client::whoCommand(MspConnection& connection)
{
    auto s = connection.requestWho();
    std::cout << "> " << s << '\n';
}

int Client::getPositionInString(const std::string& s, char c)
{
    // finding all candidates
    std::vector<int> candidates;
    for (int i = 0; i < s.length(); ++i)
    {
        if (tolower(s[i]) == tolower(c)) candidates.push_back(i);
    }

    if (candidates.empty())
    {
        std::cout << "> no such character is found\n";
        return -1;
    }

    if (candidates.size() == 1)
    {
        return candidates[0];
    }

    std::cout << "> Select the appropriate character by entering the option's number:\n";

    for (int i = 0; i < candidates.size(); ++i)
    {
        std::cout << '\t' << i + 1 << ") " << candidates[i] << ";\n";
    }

    int i;

    while (true)
    {
        std::cout << "> ";
        std::cin >> i;
        if (i < 1 || i > candidates.size())
        {
            std::cout << "> No such option, try again.\n";
            continue;
        }

        return candidates[i - 1];
    }
}

void Client::ccCommand(MspConnection &connection, int sId, char positionC, char c)
{
    if (sId < 0 || sId >= stringStorage_.size())
    {
        std::cout << "Wrong string identifier.\n";
        return;
    }

    auto pos = getPositionInString(stringStorage_.getById(sId).data_, positionC);

    if (pos < 0)  // cannot localize the character's position
        return;

    std::cout << "> Requesting the server to make changes...\n";


    if (connection.ccRequest(sId, stringStorage_.getById(sId), pos, c))
    {
        std::cout << "> Succeeded.\n";
        updateCommand(connection);
    }
    else
    {
        std::cout << "> Transaction failed, update the strings.\n";
    }
}

void Client::ccCommand(MspConnection &connection)
{
    int sId {};
    char pc {};
    char c  {};
    if (!(std::cin >> sId && std::cin >> pc && std::cin >> c))
    {
        std::cout << "> Wrong 'cc' command format. Enter h to get help.\n";
        return;
    }
    ccCommand(connection, sId - 1, pc, c);
}

void Client::rcCommand(MspConnection &connection, int sId, char positionC)
{
    if (sId < 0 || sId >= stringStorage_.size())
    {
        std::cout << "Wrong string identifier.\n";
        return;
    }

    auto pos = getPositionInString(stringStorage_.getById(sId).data_, positionC);

    if (pos < 0)  // cannot localize the character's position
        return;

    std::cout << "> Requesting the server to make changes...\n";


    if (connection.rcRequest(sId, stringStorage_.getById(sId), pos))
    {
        std::cout << "> Succeeded.\n";
        updateCommand(connection);
    }
    else
    {
        std::cout << "> Transaction failed, update the strings.\n";
    }
}

void Client::rcCommand(MspConnection &connection)
{
    int sId {};
    char pc {};
    if (!(std::cin >> sId && std::cin >> pc))
    {
        std::cout << "> Wrong 'rc' command format. Enter h to get help.\n";
        return;
    }
    rcCommand(connection, sId - 1, pc);
}

void Client::icCommand(MspConnection &connection, int sId, char positionC, char c, bool after)
{
    if (sId < 0 || sId >= stringStorage_.size())
    {
        std::cout << "Wrong string identifier.\n";
        return;
    }

    auto pos = getPositionInString(stringStorage_.getById(sId).data_, positionC);

    if (pos < 0)  // cannot localize the character's position
        return;

    if (after)
        ++pos;

    std::cout << "> Requesting the server to make changes...\n";


    if (connection.icRequest(sId, stringStorage_.getById(sId), pos, c))
    {
        std::cout << "> Succeeded.\n";
        updateCommand(connection);
    }
    else
    {
        std::cout << "> Transaction failed, update the strings.\n";
    }
}

void Client::icCommand(MspConnection &connection)
{
    int sId {};
    char pc {};
    char c  {};
    char ba {};
    if (!(std::cin >> sId && std::cin >> pc && std::cin >> c && std::cin >> ba)
    || (tolower(ba) != 'b' && tolower(ba) != 'a'))
    {
        std::cout << "> Wrong 'ic' command format. Enter h to get help.\n";
        return;
    }
    icCommand(connection, sId - 1, pc, c, ba == 'a');
}



