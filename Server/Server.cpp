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
    else
    {
        std::string em = "Unknown command";
        server->logger_.logError(em);
        connection.ignore();
    }
}

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
    logger_.addOutputStream(&std::cout);

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
