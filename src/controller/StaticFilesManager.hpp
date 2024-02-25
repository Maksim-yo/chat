#include <unordered_map>
#include <memory>
#include "oatpp/core/Types.hpp"

class StaticFilesManager {
private:
    std::unordered_map<oatpp::String, oatpp::String> m_cache;
    std::mutex m_cacheLock;
    oatpp::String m_basePath;

public:
    StaticFilesManager(const oatpp::String& basePath);
    oatpp::String getFile(const oatpp::String& path);
    
};