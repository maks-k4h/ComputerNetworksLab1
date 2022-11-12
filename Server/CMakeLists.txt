cmake_minimum_required(VERSION 3.10)

project(LabServer)

add_executable(LabServer
        main.cpp
        Server.cpp
        ../libs/StringStorage.cpp
        ../MSP/connect/TcpIPv4Connector.cpp
        ../MSP/connect/TcpIPv4Connection.cpp
        ../MSP/connect/TcpIPv4Exception.cpp
        ../libs/Logger.cpp
        ../MSP/MspConnection.cpp
        ../MSP/MspConnection.h
        ../MSP/MspException.cpp
        ../MSP/MspException.h)
