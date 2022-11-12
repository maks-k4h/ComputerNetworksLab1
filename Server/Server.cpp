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

void ProcessConnection(Server* server, TcpIPv4Connection tcpIPv4Connection)
{
    server->logger_.logStatus("New connection will be processed on thread "
                              + getCurrentThread());

    try
    {
        MspConnection mspConnection(std::move(tcpIPv4Connection), MspConnection::SERVER);
    }
    catch (const std::exception& e)
    {
        server->logger_.logError(e.what());
        return;
    }

}

//
// Created by Maks Konevych on 04.11.2022.
Server::Server(const std::string& logFilePath)
: logger_(logFilePath), storage_(9)
{
    storage_.setDataById(0, "But the stars that marked our starting fall away.");
    storage_.setDataById(1, "We must go deeper into greater pain");
    storage_.setDataById(2, "for it is not permitted that we stay.");
    storage_.setDataById(3, "Hope not ever to see Heaven.");
    storage_.setDataById(4, "I have come to lead you to the other shore;");
    storage_.setDataById(5, "into eternal darkness; into fire and into ice.");
    storage_.setDataById(6, "Before me there were no created things");
    storage_.setDataById(7, "But those that last forever—as do I.");
    storage_.setDataById(8, "Abandon all hope you who enter here.");
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
            std::thread th(ProcessConnection, this, std::move(connection));
            th.detach();
        }
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << '\n';
    }
}
