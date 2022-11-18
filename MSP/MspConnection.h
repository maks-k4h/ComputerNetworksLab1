//
// Created by Maks Konevych on 12.11.2022.
//

#ifndef MSP_MSPCONNECTION_H_
#define MSP_MSPCONNECTION_H_

#include "connect/TcpIPv4Connection.h"
#include "../libs/StringStorage.h"
#include <chrono>

#define MAX_HEADER_L

class MspConnection : private TcpIPv4Connection
{
public:
    static const std::string HandshakeRequest;
    static const std::string HandshakeResponse;
    static const std::string WhoRequest;
    static const std::string WhoResponse;
    static const std::string UpdateRequest;
    static const std::string UpdateResponse;
    static const std::string NodeResponse;
    static const std::string CCRequest;
    static const std::string ICRequest;
    static const std::string RCRequest;
    static const std::string TransactionResponse;
    static const std::string UnknownCommandResponse;

    static const char SucceededTransaction;
    static const char FailedTransaction;

    enum Role {SERVER, CLIENT};

    // after initialization the TcpIPv4Connection base will be unusable
    explicit MspConnection(TcpIPv4Connection&&, Role);

    // returns a header of a Msp-package
    std::string receiveCommands();

    // ignores every byte received during 'ms'
    void ignore(std::chrono::milliseconds ms = std::chrono::milliseconds(500));

    void sendUnknownCommand();

    std::string requestWho();   // client only
    void responseWho();         // server only

    void update(StringStorage&);    // both client and server

    void sendNode(int id, const StringStorage::Node& node);     // server only
    // returns an id and a node
    std::pair<int, StringStorage::Node> receiveNode();          // client only

    bool receiveTransactionStatus();
    void sendTransactionStatus(bool);
    bool ccRequest(int id, StringStorage::Node, int index, char ch); // client only
    bool icRequest(int id, StringStorage::Node, int index, char ch); // client only
    bool rcRequest(int id, StringStorage::Node, int index); // client only


private:
    Role role_;

    void send(const std::string& commands, const std::string& data);
    void exchangeHandshakes();

};


#endif //MSP_MSPCONNECTION_H_
