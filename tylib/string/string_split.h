#ifndef TYLIB_STRING_STRING_SPLIT_H_
#define TYLIB_STRING_STRING_SPLIT_H_

#include <string>
#include <vector>

namespace tylib {
inline std::vector<std::string> StringSplit(const std::string& str,
                                            const std::string& spStr) {
  std::vector<std::string> vecString;
  int spStrLen = spStr.size();
  int lastPosition = 0;
  int index = -1;

  while (-1 != (index = str.find(spStr, lastPosition))) {
    vecString.push_back(str.substr(lastPosition, index - lastPosition));
    lastPosition = index + spStrLen;
  }

  std::string lastStr = str.substr(lastPosition);

  if (!lastStr.empty()) vecString.push_back(lastStr);

  return vecString;
}

}  // namespace tylib

#endif  // TYLIB_STRING_STRING_SPLIT_H_
