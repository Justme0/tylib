#ifndef TYLIB_TIME_TIMER_H_
#define TYLIB_TIME_TIMER_H_

#include <sys/time.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <mutex>
#include <vector>

// extern Time g_now;
// #define g_now_ms g_now.MilliSeconds()

// https://stackoverflow.com/questions/19555121/how-to-get-current-timestamp-in-milliseconds-since-1970-just-the-way-java-gets
#define g_now_ms                                           \
  std::chrono::duration_cast<std::chrono::milliseconds>(   \
      std::chrono::system_clock::now().time_since_epoch()) \
      .count()

class Time {
 public:
  Time();

  // OPT: year month day, and Effective cpp item 18
  Time(int hour, int min, int sec);

  void ComputeNow();
  int64_t MilliSeconds() const { return m_ms; }
  int64_t MicroSeconds() const { return m_us; }
  const char* FormatTime(char* buf, int size) const;
  void AddDelay(uint64_t delay);

  int GetYear() const {
    _UpdateTm();
    return m_tm.tm_year + 1900;
  }
  int GetMonth() const {
    _UpdateTm();
    return m_tm.tm_mon + 1;
  }
  int GetDay() const {
    _UpdateTm();
    return m_tm.tm_mday;
  }
  int GetWDay() const {
    _UpdateTm();
    return m_tm.tm_wday;
  }
  int GetHour() const {
    _UpdateTm();
    return m_tm.tm_hour;
  }
  int GetMinute() const {
    _UpdateTm();
    return m_tm.tm_min;
  }
  int GetSecond() const {
    _UpdateTm();
    return m_tm.tm_sec;
  }

 private:
  int64_t m_ms;  // milliseconds from 1970
  int64_t m_us;
  mutable tm m_tm;
  mutable bool m_valid;

  void _UpdateTm() const;
};

extern Time g_now;  // for compatibility

class Timer {
  friend class TimerManager;

 public:
  explicit Timer(uint32_t interval = uint32_t(-1), int32_t count = -1);
  virtual ~Timer() {}
  bool OnTimer();
  void SetRemainCnt(int32_t remain) { m_count = remain; }

 private:
  // if return false, never execute the timer task
  virtual bool _OnTimer() { return false; }

  Timer* m_next;
  Timer* m_prev;
  Time m_triggerTime;
  uint32_t m_interval;
  int32_t m_count;
};

class TimerManager {
 public:
  ~TimerManager();

  bool UpdateTimers(const Time& now);
  void ScheduleAt(Timer* pTimer, const Time& triggerTime);
  void AddTimer(Timer* pTimer);
  void AsyncAddTimer(Timer* pTimer);
  void KillTimer(Timer* pTimer);

  static TimerManager* Instance() {
    static TimerManager mgr;
    return &mgr;
  }

 private:
  TimerManager() : m_count(0) {}

  bool _Cacsade(Timer pList[], int index);
  int _Index(int level);

  static const int LIST1_BITS = 8;
  static const int LIST_BITS = 6;
  static const int LIST1_SIZE = 1 << LIST1_BITS;
  static const int LIST_SIZE = 1 << LIST_BITS;

  Time m_lastCheckTime;

  Timer m_list1[LIST1_SIZE];  // 256 ms
  Timer m_list2[LIST_SIZE];   // 64 * 256ms = 16秒
  Timer m_list3[LIST_SIZE];   // 64 * 64 * 256ms = 17分钟
  Timer m_list4[LIST_SIZE];   // 64 * 64 * 64 * 256ms = 18 小时
  Timer m_list5[LIST_SIZE];   // 64 * 64 * 64 * 64 * 256ms = 49 天

  std::mutex m_lock;  // protect m_timers
  std::vector<Timer*> m_timers;
  volatile std::atomic_int m_count;  // count size of m_timers
};

inline int TimerManager::_Index(int level) {
  int64_t current = m_lastCheckTime.MilliSeconds();
  current >>= (LIST1_BITS + level * LIST_BITS);
  return current & (LIST_SIZE - 1);
}

#endif  // TYLIB_TIME_TIMER_H_
