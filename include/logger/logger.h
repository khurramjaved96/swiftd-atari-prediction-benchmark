//
// Created by Khurram Javed on 2024-11-04.
//

#ifndef LOGGER_H
#define LOGGER_H

#include <string>


class Logger
{
public:
    virtual void log(std::string json) = 0;
};

class FileLogger : public Logger
{
    std::string filename;

public:
    FileLogger(std::string filename);
    void log(std::string json) override;
};

#endif //LOGGER_H
