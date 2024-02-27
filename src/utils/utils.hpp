#ifndef __UTILS_H__
#define __UTILS_H__

#include "oatpp/core/Types.hpp"
#include <optional>

namespace Utils {

    std::optional<oatpp::String> loadFile(const char* filename);

    int getCurrentTimeInSeconds();

    // return seconds
    int getDateFromCurrent(int days);

    bool isDateExpired(int date);

    std::string randomString(std::size_t length);

    template <typename T, typename... Arguments>
    void hash_combine(size_t& seed, const T& arg, const Arguments&... args)
    {
        seed ^= std::hash<T>{}(arg) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        (hash_combine(seed, args), ...);
    }
} // namespace Utils
#endif // __UTILS_H__