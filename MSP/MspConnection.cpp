//
// Created by Maks Konevych on 12.11.2022.
//

#include "MspConnection.h"

#include "MspException.h"

const std::string MspConnection::HandshakeRequest       = "MspHandshakeRequest";
const std::string MspConnection::HandshakeResponse      = "MspHandshakeResponse";
const std::string MspConnection::WhoRequest             = "Who";
const std::string MspConnection::WhoResponse            = "WhoResponse";
const std::string MspConnection::UpdateRequest          = "Update";
const std::string MspConnection::UpdateResponse         = "UpdateResponse";
const std::string MspConnection::NodeResponse           = "Node";
const std::string MspConnection::CCRequest              = "ChangeCharacterRequest";
const std::string MspConnection::ICRequest              = "InsertCharacterRequest";
const std::string MspConnection::RCRequest              = "RemoveCharacterRequest";
const std::string MspConnection::TransactionResponse    = "TransactionStatus";
const std::string MspConnection::UnknownCommandResponse = "UnknownCommand";

const char MspConnection::SucceededTransaction          = '0';
const char MspConnection::FailedTransaction             = '1';

MspConnection::MspConnection(TcpIPv4Connection &&that, Role role)
: TcpIPv4Connection(std::move(that)), role_{role}
{
    exchangeHandshakes();
}

void MspConnection::exchangeHandshakes()
{
    if (role_ == SERVER)
    {
        std::string command(HandshakeRequest);

        send(command, "");

        std::string expected = HandshakeResponse;

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
        std::string expected = HandshakeRequest;

        if (message.substr(1) != expected)
        {
            std::string em = "Handshake failed.";
            logger_->logError(em);
            throw MspException(em);
        }

        send(HandshakeResponse, "");
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
            sendNode(i, storage.getById(i));
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
            auto r = receiveNode();
            storage.setNode(r.first, r.second);
        }

        logStatus("Updated strings successfully.");
    }
}

void MspConnection::sendNode(int id, const StringStorage::Node& node)
{
    if (role_ != SERVER)
    {
        std::string em = "Cannot send nodes not from a server.";
        logError(em);
        throw MspException(em);
    }

    // Node command : Node<id><hash><length>
    auto commands = NodeResponse + "\1\1\1";
    commands[NodeResponse.length() + 0] = (char)id;
    commands[NodeResponse.length() + 1] = (char)node.hash_;
    commands[NodeResponse.length() + 2] = (char)node.data_.length();
    send(commands, node.data_);
}

std::pair<int, StringStorage::Node> MspConnection::receiveNode()
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
        (StringStorage::HashType)commands[4 + 1],
        receive((int)commands[4 + 2])
    };

    return {(int)commands[4 + 0], node};
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

    std::string data = "By Maksym Konevych, K-27. 1st variant: simple string editor with automatic synchronization.";

    char dl[1] = {(char)data.length()};

    std::string commands = WhoResponse + std::string(dl, 1);

    send(commands, data);
}

void MspConnection::ignore(std::chrono::milliseconds ms)
{
    receive(false, ms);
}

bool MspConnection::receiveTransactionStatus()
{
    auto response = receiveCommands();
    if (response.substr(0, TransactionResponse.length()) != TransactionResponse)
    {
        std::string em = "Wrong command received; TransactionResponse expected";
        logError(em);
        throw MspException(em);
    }

    char status = *(response.end() - 1);

    if (status == SucceededTransaction)
    {
        return true;
    }
    else if (status == FailedTransaction)
    {
        return false;
    }
    else
    {
        std::string em = "Wrong transaction status";
        logError(em);
        throw MspException(em);
    }
}

void MspConnection::sendTransactionStatus(bool b)
{
    char status[1];
    if (b)
    {
        status[0] = SucceededTransaction;
    }
    else
    {
        status[0] = FailedTransaction;
    }

    auto commands = TransactionResponse + std::string(status, 1);
    send(commands, "");
}

bool MspConnection::ccRequest(int id, StringStorage::Node node, int index, char ch)
{
    if (role_ != CLIENT)
    {
        std::string em = "Cannot send cc-request not from a client.";
        logError(em);
        throw MspException(em);
    }

    //  parameters: <str_id><hash><char_to_replace_position><char>
    char parameters[4] = {char(id), char(node.hash_), char(index), ch};
    auto commands = CCRequest + std::string(parameters, 4);

    send(commands, "");

    if (!receiveTransactionStatus())
    {
        logStatus("CC-request failed.");
        return false;
    }
    else
    {
        logStatus("CC transaction succeeded.");
        return true;
    }
}

bool MspConnection::icRequest(int id, StringStorage::Node node, int index, char ch)
{
    if (role_ != CLIENT)
    {
        std::string em = "Cannot send ic-request not from a client.";
        logError(em);
        throw MspException(em);
    }

    //  parameters: <str_id><hash><insertion_position><char>
    char parameters[4] = {char(id), char(node.hash_), char(index), ch};
    auto commands = ICRequest + std::string(parameters, 4);

    send(commands, "");

    if (!receiveTransactionStatus())
    {
        logStatus("IC-request failed.");
        return false;
    }
    else
    {
        logStatus("IC transaction succeeded.");
        return true;
    }
}

bool MspConnection::rcRequest(int id, StringStorage::Node node, int index)
{
    if (role_ != CLIENT)
    {
        std::string em = "Cannot send ic-request not from a client.";
        logError(em);
        throw MspException(em);
    }

    //  parameters: <str_id><hash><char_to_remove_position>
    char parameters[3] = {char(id), char(node.hash_), char(index)};
    auto commands = RCRequest + std::string(parameters, 3);

    send(commands, "");

    if (!receiveTransactionStatus())
    {
        logStatus("RC-request failed.");
        return false;
    }
    else
    {
        logStatus("RC transaction succeeded.");
        return true;
    }
}

void MspConnection::sendUnknownCommand()
{
    send(UnknownCommandResponse, "");
}


