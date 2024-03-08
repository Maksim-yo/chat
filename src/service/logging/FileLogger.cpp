#include "oatpp/core/base/Environment.hpp"
#include <chrono>
#include <iomanip>
#include <stdlib.h>
#include <time.h>

#include "FileLogger.hpp"

FileLogger::FileLogger(std::string filepath)
{
    file = std::ofstream(filepath);
}

void FileLogger::processLog(const Log& log)
{
    std::lock_guard lck(mtx);
    auto time = std::chrono::system_clock::now().time_since_epoch();
    time_t seconds = std::chrono::duration_cast<std::chrono::seconds>(time).count();
    struct tm now;
    localtime_s(&now, &seconds);
    file << getSeverityName(log.getSeverity()) << "|" << std::put_time(&now, "%Y-%m-%d %H:%M:%S") << "| " << log.getMessage();
}
