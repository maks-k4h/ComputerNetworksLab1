//

#include "Server.h"

#include <sstream>


std::string getCurrentThread()  // just helper
{
    auto id = std::this_thread::get_id();
    std::stringstream ss;
    ss << id;
    return ss.str();
}

// recognizes commands, processes them, and responses
void processRequest(Server* server, MspConnection& connection)
{
    auto commands = connection.receiveCommands();

    if (commands == MspConnection::WhoRequest)
    {
        connection.responseWho();
    }
    else if (commands == MspConnection::UpdateRequest)
    {
        connection.update(server->storage_);
    }
    else if (commands.length() == MspConnection::CCRequest.length() + 4
    && commands.substr(0, MspConnection::CCRequest.length()) == MspConnection::CCRequest)
    {
        server->processCCRequest(connection, commands);
    }
    else if (commands.length() == MspConnection::ICRequest.length() + 4
    && commands.substr(0, MspConnection::ICRequest.length()) == MspConnection::ICRequest)
    {
        server->processICRequest(connection, commands);
    }
    else if (commands.length() == MspConnection::RCRequest.length() + 3
    && commands.substr(0, MspConnection::RCRequest.length()) == MspConnection::RCRequest)
    {
        server->processRCRequest(connection, commands);
    }
    else
    {
        connection.sendUnknownCommand();

        std::string em = "Unknown command";
        server->logger_.logError(em);
        connection.ignore();
    }
}

// does all the server job with this particular connection
void processConnection(Server* server, TcpIPv4Connection tcpIPv4Connection)
{
    server->logger_.logStatus("New connection will be processed on thread "
                              + getCurrentThread());

    try
    {
        MspConnection mspConnection(std::move(tcpIPv4Connection), MspConnection::SERVER);

        while (true)
        {
            processRequest(server, mspConnection);
        }
    }
    catch (TcpIPv4ClosedConnectionException& e)
    {
        // ignored.
    }
    catch (const std::exception& e)
    {
        server->logger_.logError(e.what());
    }
}

//
// Created by Maks Konevych on 04.11.2022.
Server::Server(const std::string& logFilePath)
: logger_(logFilePath), storage_()
{
    storage_.addString("But the stars that marked our starting fall away.");
    storage_.addString("We must go deeper into greater pain");
    storage_.addString("for it is not permitted that we stay.");
    storage_.addString("Hope not ever to see Heaven.");
    storage_.addString("I have come to lead you to the other shore;");
    storage_.addString("into eternal darkness; into fire and into ice.");
    storage_.addString("Before me there were no created things");
    storage_.addString("But those that last forever—as do I.");
    storage_.addString("Abandon all hope you who enter here.");
}

void Server::run()
{
    // logger_.addOutputStream(&std::cout);

    auto connector = TcpIPv4Connector(&logger_);

    try
    {
        logger_.logStatus("Starting server on thread " + getCurrentThread() + ".");
        connector.listen("1026", 5);

        while (true)
        {
            auto connection = connector.accept();
            std::thread th(processConnection, this, std::move(connection));
            th.detach();
        }
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << '\n';
    }
}

void Server::processCCRequest(MspConnection &connection, std::string commands)
{
    // can be called by several threads at a time
    std::lock_guard<std::mutex> lockGuard(mutex_);

    int sId = (int)*(commands.end() - 4);
    StringStorage::HashType hash = (StringStorage::HashType)*(commands.end() - 3);
    int pos = (int)*(commands.end() - 2);
    char ch = (char)*(commands.end() - 1);

    try
    {
        if (storage_.getById(sId).hash_ != hash)
            throw std::exception(); // outdated hash

        auto s = storage_.getById(sId).data_;
        s[pos] = ch;    // updating the string
        ++hash;         // updating the hash

        storage_.setNode(sId, StringStorage::Node{hash, s});
        connection.sendTransactionStatus(true);
    }
    catch (...)
    {
        connection.sendTransactionStatus(false);
    }
}

void Server::processICRequest(MspConnection &connection, std::string commands)
{
    // can be called by several threads at a time
    std::lock_guard<std::mutex> lockGuard(mutex_);

    int sId = (int)*(commands.end() - 4);
    StringStorage::HashType hash = (StringStorage::HashType)*(commands.end() - 3);
    int pos = (int)*(commands.end() - 2);
    char ch = (char)*(commands.end() - 1);

    try
    {
        if (storage_.getById(sId).hash_ != hash)
            throw std::exception(); // outdated hash

        if (storage_.getById(sId).data_.length() >= MAX_STRING_L)
            throw std::exception(); // max string length reached

        auto s = storage_.getById(sId).data_;
        char in[1] = {ch};
        s.insert(pos, std::string(in, 1));  // inserting the character
        ++hash;                             // updating the hash

        storage_.setNode(sId, StringStorage::Node{hash, s});
        connection.sendTransactionStatus(true);
    }
    catch (...)
    {
        connection.sendTransactionStatus(false);
    }
}

void Server::processRCRequest(MspConnection &connection, std::string commands)
{
    // can be called by several threads at a time
    std::lock_guard<std::mutex> lockGuard(mutex_);

    int sId = (int)*(commands.end() - 3);
    StringStorage::HashType hash = (StringStorage::HashType)*(commands.end() - 2);
    int pos = (int)*(commands.end() - 1);

    try
    {
        if (storage_.getById(sId).hash_ != hash)
            throw std::exception(); // outdated hash

        auto s = storage_.getById(sId).data_;
        s = s.erase(pos, 1);    // removing one character
        ++hash;                 // updating the hash

        storage_.setNode(sId, StringStorage::Node{hash, s});
        connection.sendTransactionStatus(true);
    }
    catch (...)
    {
        connection.sendTransactionStatus(false);
    }
}
