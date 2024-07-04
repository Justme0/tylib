
#ifndef TYLIB_TIME_TIME_UTIL_H_
#define TYLIB_TIME_TIME_UTIL_H_

#include <ctime>

#include <string>

namespace tylib {

// local timezone is of server, OPT: use UTC0 for internationalization
// NOTE: strftime use static var

inline std::string SecondToLocalTimeString(time_t second) {
  char szTime[64];
  tm* p = localtime(&second);
  strftime(szTime, sizeof(szTime), "%Y-%m-%d %H:%M:%S", p);

  return szTime;
}

inline std::string MilliSecondToLocalTimeString(int64_t milliSecond) {
  char szTime[64];
  time_t tTime = milliSecond / 1000;
  int fraction = milliSecond % 1000;
  tm* p = localtime(&tTime);
  strftime(szTime, sizeof(szTime), "%Y-%m-%d %H:%M:%S", p);

  char fractionBuf[5];
  sprintf(fractionBuf, "%03d", fraction);

  return std::string(szTime) + "." + fractionBuf;
}

inline std::string MicroSecondToLocalTimeString(int64_t microSecond) {
  char szTime[64];
  time_t tTime = microSecond / 1000000;
  int fraction = microSecond % 1000000;
  tm* p = localtime(&tTime);
  strftime(szTime, sizeof(szTime), "%Y-%m-%d %H:%M:%S", p);

  char fractionBuf[8];
  sprintf(fractionBuf, "%06d", fraction);

  return std::string(szTime) + "." + fractionBuf;
}

}  // namespace tylib

#endif  // TYLIB_TIME_TIME_UTIL_H_
