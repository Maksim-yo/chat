#include "StreamLogger.hpp"
#include "oatpp/core/base/Environment.hpp"

void StreamLogger::processLog(const Log& log)
{
    auto severity = log.getSeverity();
    if (severity == Severity::ERR) {
        OATPP_LOGE(log.getModule().c_str(), log.getMessage().c_str());
    } else if (severity == Severity::DEBUG) {
        OATPP_LOGD(log.getModule().c_str(), log.getMessage().c_str());
    } else if (severity == Severity::INFO) {
        OATPP_LOGI(log.getModule().c_str(), log.getMessage().c_str());
    } else if (severity == Severity::WARNING) {
        OATPP_LOGW(log.getModule().c_str(), log.getMessage().c_str());
    }
}
