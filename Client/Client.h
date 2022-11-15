//
// Created by Maks Konevych on 04.11.2022.
//

#ifndef CLIENT_CLIENT_H_
#define CLIENT_CLIENT_H_

#include <string>
#include <list>

#include "../libs/StringStorage.h"
#include "../libs/Logger.h"
#include "../MSP/MspConnection.h"

class Client {
public:
    explicit Client(std::string logFilePath);

    void run();

private:
    StringStorage stringStorage_;
    Logger logger_;

    static std::string readUsersCommand();
    void process(const std::string& command, MspConnection&);

    void renderStrings();

    static void helpCommand();
    void updateCommand(MspConnection&);
    void whoCommand(MspConnection&);
    void ccCommand(MspConnection&);
    void ccCommand(MspConnection&, int sId, char positionC, char c);
    void rcCommand(MspConnection&);
    void rcCommand(MspConnection&, int sId, char positionC);
    void icCommand(MspConnection&);
    void icCommand(MspConnection&, int sId, char positionC, char c, bool after);

    int getPositionInString(const std::string& s, char c);
};

#endif //CLIENT_CLIENT_H_
