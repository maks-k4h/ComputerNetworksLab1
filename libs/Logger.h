//
// Created by Maks Konevych on 12.11.2022.
//

#ifndef LIBS_LOGGER_H_
#define LIBS_LOGGER_H_

#include <string>
#include <list>
#include <ostream>
#include <mutex>

class Logger
{
public:
    explicit Logger(std::string logFilePath_ = "");

    void logStatus(const std::string&);
    void logError(const std::string&);

    void addOutputStream(std::ostream* os);

private:
    std::string logFilePath_;
    std::string sessionLogs_;

    std::mutex mutex_;

    std::list<std::ostream*> additionalStreams_;

    void log(const std::string& header, const std::string& message);
};


#endif //LIBS_LOGGER_H_
