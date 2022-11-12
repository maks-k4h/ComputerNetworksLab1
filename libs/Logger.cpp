//
// Created by Maks Konevych on 12.11.2022.
//

#include "Logger.h"
#include <fstream>
#include <utility>

Logger::Logger(std::string logFilePath_)
: logFilePath_{std::move(logFilePath_)}
{
}

void Logger::logStatus(const std::string& m)
{
    log("STATUS ", m);
}

void Logger::logError(const std::string& m)
{
    log("ERROR ", m);
}

void Logger::log(const std::string& h, const std::string& m)
{
    auto t = time(nullptr);
    auto message = h + std::string(ctime(&t)) + "\t" + m + '\n';

    sessionLogs_ += message;

    if (!logFilePath_.empty())
    {
        std::ofstream logFile;
        logFile.open(logFilePath_, std::ios_base::app);
        if (!logFile.is_open())
        {
            sessionLogs_ += "ERROR " + std::string(ctime(&t)) + "\t"
                    + "Cannot open log file " + logFilePath_;
        }
        else
        {
            logFile << message;
            logFile.close();
        }
    }
}


