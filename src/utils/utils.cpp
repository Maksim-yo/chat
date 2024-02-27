#include "utils.hpp"

#include "oatpp/core/Types.hpp"
#include "oatpp/core/base/Environment.hpp"

#include <chrono>
#include <optional>
#include <random>

namespace Utils {
    std::optional<oatpp::String> loadFile(const char* filename)
    {
        auto buffer = oatpp::String::loadFromFile(filename);
        if (buffer == nullptr)
            return std::nullopt;
        return std::make_optional<oatpp::String>(buffer);
    }

    int getCurrentTimeInSeconds()
    {
        auto cur_time = std::chrono::system_clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::seconds>(cur_time).count();
    }
    // return seconds
    int getDateFromCurrent(int days)
    {
        auto cur_time = std::chrono::system_clock::now();
        auto expiry_time = cur_time + std::chrono::hours(days * 24);
        auto expiry_time_ms = std::chrono::time_point_cast<std::chrono::seconds>(expiry_time).time_since_epoch();
        return std::chrono::duration_cast<std::chrono::seconds>(expiry_time_ms).count();
    }

    bool isDateExpired(int date)
    {
        int cur_time = getCurrentTimeInSeconds();
        return cur_time > date;
    }

    std::string randomString(std::size_t length)
    {
        const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

        std::random_device random_device;
        std::mt19937 generator(random_device());
        std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

        std::string random_string;

        for (std::size_t i = 0; i < length; ++i) {
            random_string += CHARACTERS[distribution(generator)];
        }

        return random_string;
    }

} // namespace Utils