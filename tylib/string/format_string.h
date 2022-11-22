#ifndef TYLIB_STRING_FORMAT_STRING_H_
#define TYLIB_STRING_FORMAT_STRING_H_

#include <cstdarg>
#include <string>

namespace tylib {
// taylor to use c++20 std::format
// https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
static inline std::string format_string(const char* szFmt, ...)
    __attribute__((format(printf, 1, 2)));
static inline std::string format_string(const char* szFmt, ...) {
  int n = 0;

  va_list ap;

  // max string allowed
  const int iLargeSize = 65536;
  // use static, not thread safe
  static char szLargeBuff[iLargeSize];

  va_start(ap, szFmt);
  n = vsnprintf(szLargeBuff, sizeof(szLargeBuff), szFmt, ap);
  va_end(ap);

  if (n >= iLargeSize) {
    n = iLargeSize - 1;
  }

  return std::string(szLargeBuff, n);
}

}  // namespace tylib

#endif  // TYLIB_STRING_FORMAT_STRING_H_
