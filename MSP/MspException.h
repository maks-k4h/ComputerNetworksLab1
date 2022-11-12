//
// Created by Maks Konevych on 12.11.2022.
//

#ifndef MSP_MSPEXCEPTION_H_
#define MSP_MSPEXCEPTION_H_

#include <exception>
#include <string>

class MspException : public std::exception
{
public:
    explicit MspException(std::string message);

    const char* what() const _NOEXCEPT override;

private:
    std::string message_;
};


#endif //MSP_MSPEXCEPTION_H_
