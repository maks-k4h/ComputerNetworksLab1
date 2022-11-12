//
// Created by Maks Konevych on 11.11.2022.
//

#ifndef LIBS_CONNECT_TCPIPV4CONNECTOR_H_
#define LIBS_CONNECT_TCPIPV4CONNECTOR_H_

#include "TcpIPv4Connection.h"
#include "../../libs/Logger.h"

#include <string>
#include <netdb.h>

/*
 * TcpIPv4Connector class encapsulates the logic of both connecting to a host
 * and listening for new connections.
 * Every instance of the class can work in both listener or connector mode.
 * If you first called listen(..) that succeed and try to call connect(..)
 * an exception will be thrown. Be sure to call stopListening(..) priorly.
 *
 * Both connect and accept methods return TcpIPv4Connection object, that
 * represent this connection.
 *
 * To connect to a host use connect() method with proper parameters.
 *
 * To listen to connection requests first call listen(), it will try to open
 * the asked port and listen for requests on it. Then you call accept() which
 * accepts a connection request and returns TcpIPv4Connection object to work
 * specifically with this connection. The connector is still listening for new
 * connections. To stop listening call stopListening().
 *
 * */

class TcpIPv4Connector
{
public:
    explicit TcpIPv4Connector(Logger* logger = nullptr);
    ~TcpIPv4Connector();

    TcpIPv4Connection connect(const std::string& host, const std::string& service);
    void stopListening();
    void listen(const std::string& service, int backlog);

    TcpIPv4Connection accept();

private:
    int fd_ {};
    bool listening {};
    Logger* logger_;

    class AddrInfoWrapper
    {
    public:
        explicit AddrInfoWrapper(addrinfo* ai);
        ~AddrInfoWrapper();

        addrinfo* getAddrInfo() const;

    private:
        addrinfo* ai_;
    };

    // host is not necessary if addrinfo will be used for listening
    AddrInfoWrapper getAddrInfo(const std::string& service, const std::string& host = "");

    void logStatus(const std::string& s);
    void logError(const std::string& s);
};


#endif //LIBS_CONNECT_TCPIPV4CONNECTOR_H_
