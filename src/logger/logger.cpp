//
// Created by Khurram Javed on 2024-11-04.
//

#include "../../include/logger/logger.h"
#include <fstream>


FileLogger::FileLogger(std::string filename)
{
    this->filename = filename;
}

void FileLogger::log(std::string json)
{
    std::ofstream file;
    file.open(this->filename, std::ios_base::out); // Overwrite the file
    file << json << std::endl;
    file.close();
}
