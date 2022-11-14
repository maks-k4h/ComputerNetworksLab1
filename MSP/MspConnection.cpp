//
// Created by Maks Konevych on 12.11.2022.
//

#include "MspConnection.h"

#include "MspException.h"

const std::string MspConnection::WhoRequest = "Who";
const std::string MspConnection::WhoResponse = "WhoResponse";
const std::string MspConnection::UpdateRequest = "Update";
const std::string MspConnection::UpdateResponse = "UpdateResponse";
const std::string MspConnection::NodeResponse = "Node";

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
            std::string em = "Handshake failed.";
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
            std::string em = "Handshake failed.";
            logger_->logError(em);
            throw MspException(em);
        }

        send("HandshakeResponse", "");
    }
}

void MspConnection::send(const std::string& commands, const std::string& data)
{
    if (commands.length() > MAX_HEADER_L - 1)
    {
        std::string em = "Cannot send the Msp message since the header is too long.";
        logger_->logError(em);
        throw MspException(em);
    }

    std::string message = "\1" + commands + data;
    message[0] = (char)commands.length();
    TcpIPv4Connection::send(message);
}

void MspConnection::update(StringStorage& storage)
{
    if (role_ == SERVER)
    {
        auto commands = UpdateResponse + "\1";
        *(commands.end() - 1) = char(storage.size());
        send(commands, "");

        for (int i = 0; i < storage.size(); ++i)
        {
            sendNode(storage.getById(i));
        }
        logStatus("Sent strings successfully.");
    }
    else if (role_ == CLIENT)
    {
        std::string commands = UpdateRequest;
        send(commands, "");

        commands = receiveCommands();
        if (commands.length() != UpdateResponse.length() + 1
        || commands.substr(0, UpdateResponse.length()) != UpdateResponse)
        {
            std::string em = "Wrong command received; " + UpdateResponse + "<num> expected";
            logError(em);
            throw MspException(em);
        }

        int nodesNumber = (int)*(commands.end() - 1);
        logStatus("Receiving " + std::to_string(nodesNumber) + " nodes.");

        for (int i = 0; i < nodesNumber; ++i)
        {
            storage.setNode(receiveNode());
        }

        logStatus("Updated strings successfully.");
    }
}

void MspConnection::sendNode(const StringStorage::Node& node)
{
    if (role_ != SERVER)
    {
        std::string em = "Cannot send nodes not from a server.";
        logError(em);
        throw MspException(em);
    }

    // Node command : Node<id><hash><length>
    auto commands = NodeResponse + "\1\1\1";
    commands[NodeResponse.length() + 0] = (char)node.id_;
    commands[NodeResponse.length() + 1] = (char)node.hash_;
    commands[NodeResponse.length() + 2] = (char)node.data_.length();
    send(commands, node.data_);
}

StringStorage::Node MspConnection::receiveNode()
{
    if (role_ != CLIENT)
    {
        std::string em = "Cannot receive nodes not from a client.";
        logError(em);
        throw MspException(em);
    }

    auto commands = receiveCommands();
    if (commands.length() != NodeResponse.length() + 3
    || commands.substr(0, NodeResponse.length()) != NodeResponse)
    {
        std::string em = "Wrong command received; " + NodeResponse + "<id><hash><length> expected";
        logError(em);
        throw MspException(em);
    }

    StringStorage::Node node {
        (int)commands[4 + 0],
        (int)commands[4 + 1],
        receive((int)commands[4 + 2])
    };

    return node;
}

std::string MspConnection::receiveCommands()
{
    int hLen = (int)receive(1)[0];
    return receive(hLen);
}

std::string MspConnection::requestWho()
{
    if (role_ != CLIENT)
    {
        std::string em = "Cannot request 'Who' not from a client.";
        logError(em);
        throw MspException(em);
    }

    send(WhoRequest, "");

    // Expected response: WhoResponse<length><data>, where
    // length takes 1 byte and <data> has length of <length>

    auto commands = receiveCommands();

    if (commands.length() != WhoResponse.length() + 1 ||
    commands.substr(0, WhoResponse.length()) != WhoResponse)
    {
        std::string em = "Wrong command received; WhoResponse<length> expected";
        logError(em);
        throw MspException(em);
    }

    int length = (int)(u_char)commands[WhoResponse.length()];

    return receive(length);
}

void MspConnection::responseWho()
{
    if (role_ != SERVER)
    {
        std::string em = "Cannot response to 'Who' not from a server.";
        logError(em);
        throw MspException(em);
    }

    std::string data = "Not boring at all 'who' response...";

    char dl[1] = {(char)data.length()};

    std::string commands = WhoResponse + std::string(dl, 1);

    send(commands, data);
}

void MspConnection::ignore(std::chrono::milliseconds ms)
{
    receive(false, ms);
}


