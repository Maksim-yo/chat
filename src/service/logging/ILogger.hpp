#ifndef __ILOGGER_H__
#define __ILOGGER_H__

#include <memory>
#include <sstream>

#include "oatpp/core/macro/component.hpp"

class Logging;

enum class Severity
{
    ERR,
    WARNING,
    INFO,
    DEBUG,
};

const char* getSeverityName(Severity sev);

class ILogger;
class Log
{
public:
    Log(Logging& logger, std::string module, Severity severity);
    ~Log();

    std::string getModule() const { return _module; }
    Severity getSeverity() const { return _severity; }
    std::string getMessage() const;

    std::ostringstream& getOstream() { return _oss; }

private:
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;

    Logging& _logging;
    std::string _module;
    Severity _severity;
    std::ostringstream _oss;
};

class ILogger
{
public:
    ILogger();
    virtual ~ILogger() = default;
    virtual void processLog(const Log& log) = 0;
};

#define LOG(module, severity, message) do {\
        if (auto logger_ {OATPP_GET_COMPONENT(std::shared_ptr<Logging>)}) \
            ::Log( *logger_, module, ::Severity::severity ).getOstream() << message; \
    } while(0)

#define LOG_ERR(module, message) \
    LOG(module, ERR, message)

#define LOG_INFO(module, message) \
    LOG(module, INFO, message)

#define LOG_WARNING(module, message) \
    LOG(module, WARNING, message)

#define LOG_DEBUG(module, message) \
    LOG(module, DEBUG, message)
    
#endif // __ILOGGER_H__