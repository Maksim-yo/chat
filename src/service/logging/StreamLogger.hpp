#ifndef __STREAMLOGGER_H__
#define __STREAMLOGGER_H__

#include "ILogger.hpp"

class StreamLogger : public ILogger
{
public:
    using ILogger::ILogger;
    void processLog(const Log& log) override;
};
#endif // __STREAMLOGGER_H__