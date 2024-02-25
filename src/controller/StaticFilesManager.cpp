#include "StaticFilesManager.hpp"

StaticFilesManager::StaticFilesManager(const oatpp::String& basePath): m_basePath(basePath) {}

oatpp::String StaticFilesManager::getFile(const oatpp::String& path)
{
    if (!path)    
        return nullptr;
    std::lock_guard mtx(m_cacheLock);
    auto& file = m_cache[path];
    if (file)
        return file;
    oatpp::String filename = m_basePath + path;
    file = oatpp::String::loadFromFile(filename->c_str());
    return file;

}
