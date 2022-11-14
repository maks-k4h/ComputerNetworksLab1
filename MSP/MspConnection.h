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
    static const std::string WhoRequest;
    static const std::string WhoResponse;
    static const std::string UpdateRequest;
    static const std::string UpdateResponse;
    static const std::string NodeResponse;

    enum Role {SERVER, CLIENT};

    // after initialization the TcpIPv4Connection base will be unusable
    explicit MspConnection(TcpIPv4Connection&&, Role);

    std::string receiveCommands();

    // ignores every byte received during 'ms'
    void ignore(std::chrono::milliseconds ms = std::chrono::milliseconds(500));

    std::string requestWho();   // client only
    void responseWho();  // server only

    void update(StringStorage&);    // both client and server

    void sendNode(const StringStorage::Node& node);    // server only
    StringStorage::Node receiveNode();          // client only
private:
    Role role_;

    void send(const std::string& commands, const std::string& data);
    void exchangeHandshakes();

};


#endif //MSP_MSPCONNECTION_H_
