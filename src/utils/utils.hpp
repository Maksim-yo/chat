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

  // TODO: Replace
  std::string randomString(std::size_t length);
}
#endif // __UTILS_H__