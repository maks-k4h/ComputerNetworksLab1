//
// Created by Maks Konevych on 12.11.2022.
//

#include "MspConnection.h"

#include "MspException.h"

MspConnection::MspConnection(TcpIPv4Connection &&that, Role role)
: TcpIPv4Connection(std::move(that)), role_{role}
{
    exchangeHandshakes();
}

void MspConnection::exchangeHandshakes()
{
    if (role_ == SERVER)
    {
        std::string command("MspHandshake");

        send(command, "");

        std::string expected = "HandshakeResponse";

        std::string response = receive(1 + (int)expected.length());

        if (response.substr(1) != expected)
        {
            std::string em = "Handshake failed on socket " + std::to_string(fd_) + ".";
            logger_->logError(em);
            throw MspException(em);
        }
    }
    if (role_ == CLIENT)
    {
        std::string message = receive();
        std::string expected = "MspHandshake";

        if (message.substr(1) != expected)
        {
            std::string em = "Handshake failed on socket " + std::to_string(fd_) + ".";
            logger_->logError(em);
            throw MspException(em);
        }

        send("HandshakeResponse", "");
    }
}

void MspConnection::send(std::string commands, std::string data)
{
    if (commands.length() > MAX_HEADER_L - 1)
    {
        std::string em = "Cannot send the Msp message since the header is too long.";
        logger_->logError(em);
        throw MspException(em);
    }

    char fb[1] = {(char)commands.length()};

    std::string message = fb + commands + data;
    TcpIPv4Connection::send(message);
}


