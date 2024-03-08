#ifndef __FILELOGGER_H__
#define __FILELOGGER_H__

#include <fstream>
#include <mutex>

#include "ILogger.hpp"

class FileLogger : public ILogger
{
public:
    FileLogger(std::string filename);
    void processLog(const Log& log) override;

private:
    std::mutex mtx;
    std::string filepath;
    std::ofstream file;
};
#endif // __FILELOGGER_H__