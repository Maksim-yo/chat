#include "Logging.hpp"
#include <iostream>

void Logging::addLogger(std::shared_ptr<ILogger> logger)
{
    loggers.emplace_back(logger);
}

void Logging::processLog(const Log& log)
{
    for (const auto& logger : loggers) {
        logger->processLog(log);
    }
}
