//
// Created by Maks Konevych on 11.11.2022.
//

#include "TcpIPv4Connection.h"

#include <utility>
#include <sys/socket.h>
#include <thread>
#include <sys/ioctl.h>
#include <unistd.h>

TcpIPv4Connection::TcpIPv4Connection(int fd, Logger *logger)
: fd_{fd}, connected_{true}, logger_{logger}
{
}

TcpIPv4Connection::~TcpIPv4Connection()
{
    close();
}

void TcpIPv4Connection::close()
{
    if (connected_)
    {
        ::close(fd_);
        connected_ = false;
        logStatus("Closed connection on " + std::to_string(fd_) + ".");
    }
}

void TcpIPv4Connection::send(const std::string& message)
{
    if (!connected_)
    {
        std::string em = "Attempt to send(" + message
                + ") with a closed connection.";
        logError(em);
        throw TcpIPv4Exception(em);
    }

    int sent = 0;
    while (sent < message.length())
    {
        int a = (int)::send(fd_, message.substr(sent).c_str(), message.length() - sent, 0);
        if (a < 0)
        {
            auto em = "sent(" + std::to_string(fd_) + "," + message.substr(sent)
                    + "," + std::to_string(message.length() - sent) + ", 0) failed.";
            logError(em);
            throw TcpIPv4Exception(em);
        }
        sent += a;
    }

    logStatus("Sent <<<" + message + ">>> from socked " + std::to_string(fd_) + ".");
}

std::string TcpIPv4Connection::receive(int len)
{
    if (!connected_)
    {
        std::string em = "Attempt to receive(" + std::to_string(len)
                         + ") with a closed connection.";
        logError(em);
        throw TcpIPv4Exception(em);
    }

    std::string res;
    const int buffSize = 1024;
    char buff[buffSize];

    while (len > 0)
    {
        int a = (int)::recv(fd_, buff, std::min(len, buffSize - 1), 0); // leave at leas one free byte in buff

        if (a == 0)
        {
            std::string em = "recv(" + std::to_string(fd_) + ", "
                    + "<buffer>, " + std::to_string(len) + ", 0 failed. "
                    + "Returned value =" + std::to_string(a) + " — the intended"
                    + " sender has closed the connection.";
            logError(em);
            throw TcpIPv4Exception(em);
        }
        if (a < 0)
        {
            std::string em = "recv(" + std::to_string(fd_) + ", "
                     + "<buffer>, " + std::to_string(len) + ", 0) failed. "
                     + "Returned value =" + std::to_string(a) + ".";
            logError(em);
            throw TcpIPv4Exception(em);
        }

        buff[a] = 0; // making a c-string of the proper length
        res += buff;
        len -= a;
    }

    logStatus("Received <<<" + res + ">>> from socked " + std::to_string(fd_) + ".");

    return res;
}

std::string TcpIPv4Connection::receive(bool blocking, std::chrono::nanoseconds timeout)
{
    if (!connected_)
    {
        std::string em = "Attempt to receive(<timeout>) with a closed connection.";
        logError(em);
        throw TcpIPv4Exception(em);
    }

    const int buffSize = 1024;
    char buff[buffSize];

    std::string res;

    std::this_thread::sleep_for(timeout); // waiting

    // checking if there is any data to receive
    int count;
    ioctl(fd_, FIONREAD, &count);

    if (count == 0) // no data to receive
    {
        if (blocking)
        {
            count = 1; // pretend that there is a byte in a stream
        }
        else
        {
            return res; // return empty string
        }
    }
    if (count < 0)
    {
        auto em = "Attempt to receive(<timeout>) failed due to an error "
                  "in ioctl(fd_, FIONREAD, &count).";
        logError(em);
        throw TcpIPv4Exception(em);
    }

    while (count > 0)
    {
        int a = (int)::recv(fd_, buff, buffSize - 1, 0); // leave at leas one free byte in buff

        if (a == 0)
        {
            std::string em = "recv(<timeout>) failed. "
                     "Returned value =" + std::to_string(a) + " — the intended "
                     "sender has closed the connection.";
            logError(em);
            throw TcpIPv4Exception(em);
        }
        if (a < 0)
        {
            std::string em = "recv(<timeout>) failed. "
                             "Returned value =" + std::to_string(a) + ".";
            logError(em);
            throw TcpIPv4Exception(em);
        }

        buff[a] = 0; // making a c-string of the proper length
        res += buff;

        ioctl(fd_, FIONREAD, &count); // checking if there is any data to receive
    }

    logStatus("Received <<<" + res + ">>> from socked " + std::to_string(fd_) + ".");

    return res;
}

void TcpIPv4Connection::logStatus(const std::string& s)
{
    if (logger_)
        logger_->logStatus(s);
}

void TcpIPv4Connection::logError(const std::string& s)
{
    if (logger_)
        logger_->logError(s);
}

