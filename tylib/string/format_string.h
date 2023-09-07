#ifndef TYLIB_STRING_FORMAT_STRING_H_
#define TYLIB_STRING_FORMAT_STRING_H_

#include <cstdarg>
#include <string>

namespace tylib {

// OPT: use c++20 std::format
// https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
template <size_t N = 32 * 1024>
std::string format_string(const char* fmt, ...)
    __attribute__((format(printf, 1, 2)));

template <size_t N>
inline std::string format_string(const char* fmt, ...) {
  int n = 0;
  va_list ap;
  static thread_local char buffer[N];

  va_start(ap, fmt);
  n = vsnprintf(buffer, sizeof(buffer), fmt, ap);
  va_end(ap);
  if (n >= static_cast<int>(sizeof(buffer))) {
    n = sizeof(buffer) - 1;
  }
  return std::string(buffer, n);
}

}  // namespace tylib

#endif  // TYLIB_STRING_FORMAT_STRING_H_
