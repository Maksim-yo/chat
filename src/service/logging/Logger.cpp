
#include "oatpp/core/macro/component.hpp"

#include "ILogger.hpp"
#include "Logging.hpp"

const char* getSeverityName(Severity sev)
{
    switch (sev) {
    case Severity::ERR:
        return "error";
    case Severity::WARNING:
        return "warning";
    case Severity::INFO:
        return "info";
    case Severity::DEBUG:
        return "debug";
    }
    return "";
}

ILogger::ILogger()
{
}

Log::Log(Logging& logging, std::string module, Severity severity)
    : _logging{logging}, _severity{severity}, _module{module}
{
}

Log::~Log()
{
    _logging.processLog(*this);
}

std::string Log::getMessage() const
{
    return _oss.str();
}