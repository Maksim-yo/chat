#ifndef __LOGGING_H__
#define __LOGGING_H__
#include <memory>
#include <vector>

#include "ILogger.hpp"

class Logging
{
public:
    void processLog(const Log& log);
    void addLogger(std::shared_ptr<ILogger> logger);

private:
    std::vector<std::shared_ptr<ILogger>> loggers;
};
#endif // __LOGGING_H__