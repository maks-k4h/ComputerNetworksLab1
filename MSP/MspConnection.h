//
// Created by Maks Konevych on 12.11.2022.
//

#ifndef MSP_MSPCONNECTION_H_
#define MSP_MSPCONNECTION_H_

#include "connect/TcpIPv4Connection.h"

#define MAX_HEADER_L

class MspConnection : private TcpIPv4Connection
{
public:
    enum Role {SERVER, CLIENT};

    // after initialization the TcpIPv4Connection base will be unusable
    explicit MspConnection(TcpIPv4Connection&&, Role);


private:
    Role role_;

    void send(std::string commands, std::string data);
    void exchangeHandshakes();

};


#endif //MSP_MSPCONNECTION_H_
