#include "oatpp/core/utils/String.hpp"
#include "oatpp/core/base/Environment.hpp"
#include <optional>


namespace Utils {
  static std::optional<oatpp::String> loadFile(const char* filename) {
    auto buffer = oatpp::String::loadFromFile(filename);
    if (buffer == nullptr)
        return std::nullopt
    OATPP_ASSERT_HTTP(buffer, Status::CODE_404, "File Not Found:(");
    return std::make_optional<oatpp::String>(buffer);
  }

}