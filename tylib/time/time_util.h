
#ifndef TYLIB_TIME_TIME_UTIL_H_
#define TYLIB_TIME_TIME_UTIL_H_

#include <string>

namespace tylib {

// local timezone is of server, OPT: use UTC0 for internationalization
inline std::string MilliSecondToLocalTimeString(int64_t ms) {
  char szTime[64];
  struct tm stTime;
  time_t tTime = ms / 1000;
  int fraction = ms % 1000;
  localtime_r(&tTime, &stTime);
  strftime(szTime, sizeof(szTime), "%Y-%m-%d %H:%M:%S", &stTime);

  char fractionBuf[5];
  sprintf(fractionBuf, "%03d", fraction);

  return std::string(szTime) + "." + fractionBuf;
}

// local timezone is of server, OPT: use UTC0
inline std::string SecondToLocalTimeString(int64_t second) {
  char szTime[64];
  struct tm stTime;
  localtime_r(&second, &stTime);
  strftime(szTime, sizeof(szTime), "%Y-%m-%d %H:%M:%S", &stTime);
  return szTime;
}

}  // namespace tylib

#endif  // TYLIB_TIME_TIME_UTIL_H_