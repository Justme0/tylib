#include "timer.h"

#include <cassert>
#include <cstdlib>

static bool IsLeapYear(int year) {
  return (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0));
}

static int DaysOfMonth(int year, int month) {
  const int monthDay[13] = {-1, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  if (2 == month && IsLeapYear(year)) return 29;

  return monthDay[month];
}

Time g_now;  // for compatibility

Time::Time() : m_ms(0), m_us(0), m_valid(false) {
  m_tm.tm_year = 0;
  this->ComputeNow();
}

Time::Time(int hour, int min, int sec) {
  this->ComputeNow();

  // 如果hour小于当前的hour，则换天了，如果当前天数是本月最后一天，则换月；如果本月是12月，则换年
  int day = GetDay();
  int mon = GetMonth();
  int year = GetYear();

  bool tomorrow = false;
  if (hour < GetHour() || (hour == GetHour() && min < GetMinute()) ||
      (hour == GetHour() && min == GetMinute() && sec < GetSecond()))
    tomorrow = true;

  if (tomorrow) {
    if (DaysOfMonth(year, mon) == day) {
      day = 1;

      if (12 == mon) {
        mon = 1;
        ++year;
      } else
        ++mon;
    } else {
      ++day;
    }
  }

  // 构造tm
  struct tm stm;
  stm.tm_sec = sec;
  stm.tm_min = min;
  stm.tm_hour = hour;
  stm.tm_mday = day;
  stm.tm_mon = mon - 1;
  stm.tm_year = year - 1900;
  stm.tm_yday = 0;
  stm.tm_isdst = 0;

  time_t tt = mktime(&stm);
  m_ms = tt * 1000UL;
  m_us = m_ms * 1000UL;
  m_valid = false;
}

void Time::_UpdateTm() const {
  if (m_valid) return;

  m_valid = true;
  const time_t now(m_ms / 1000UL);
  ::localtime_r(&now, &m_tm);
}

// 调系统调用，有一定开销
void Time::ComputeNow() {
  struct timeval now;
  ::gettimeofday(&now, 0);
  m_ms = static_cast<int64_t>(now.tv_sec) * 1000UL + now.tv_usec / 1000UL;
  m_us = static_cast<int64_t>(now.tv_sec) * 1000000UL + now.tv_usec;
  m_valid = false;
}

const char* Time::FormatTime(char* buf, int maxSize) const {
  if (buf && maxSize > 1) {
    _UpdateTm();
    snprintf(buf, static_cast<size_t>(maxSize),
             "%04d-%02d-%02d[%02d:%02d:%02d]", m_tm.tm_year + 1900,
             m_tm.tm_mon + 1, m_tm.tm_mday, m_tm.tm_hour, m_tm.tm_min,
             m_tm.tm_sec);

    return buf;
  }

  return nullptr;
}

void Time::AddDelay(uint64_t delay) {
  m_ms += delay;
  m_us += delay * 1000UL;
  m_valid = false;
}

// interval 单位：毫秒
Timer::Timer(uint32_t interval, int32_t count)
    : m_interval(interval), m_count(count) {
  m_next = m_prev = nullptr;
  m_triggerTime.AddDelay(interval);
}

bool Timer::OnTimer() {
  if (m_count < 0 || --m_count >= 0) {
    m_triggerTime.AddDelay(m_interval);
    return _OnTimer();
  }

  return false;
}

TimerManager::~TimerManager() {
  Timer* pTimer;
  for (int i = 0; i < LIST1_SIZE; ++i) {
    while ((pTimer = m_list1[i].m_next) != nullptr) {
      KillTimer(pTimer);
    }
  }

  for (int i = 0; i < LIST_SIZE; ++i) {
    while ((pTimer = m_list2[i].m_next) != nullptr) {
      KillTimer(pTimer);
    }

    while ((pTimer = m_list3[i].m_next) != nullptr) {
      KillTimer(pTimer);
    }

    while ((pTimer = m_list4[i].m_next) != nullptr) {
      KillTimer(pTimer);
    }

    while ((pTimer = m_list5[i].m_next) != nullptr) {
      KillTimer(pTimer);
    }
  }
}

bool TimerManager::UpdateTimers(const Time& now) {
  if (m_count.load() > 0 && m_lock.try_lock()) {
    std::vector<Timer*> tmp;
    tmp.swap(m_timers);
    m_count = 0;
    m_lock.unlock();

    for (Timer* t : tmp) {
      AddTimer(t);
    }
  }

  const bool hasUpdated(m_lastCheckTime.MilliSeconds() <= now.MilliSeconds());

  while (m_lastCheckTime.MilliSeconds() <= now.MilliSeconds()) {
    int index = m_lastCheckTime.MilliSeconds() & (LIST1_SIZE - 1);
    if (index == 0 && !_Cacsade(m_list2, _Index(0)) &&
        !_Cacsade(m_list3, _Index(1)) && !_Cacsade(m_list4, _Index(2))) {
      _Cacsade(m_list5, _Index(3));
    }

    m_lastCheckTime.AddDelay(1);

    Timer* pTimer;
    while ((pTimer = m_list1[index].m_next) != nullptr) {
      KillTimer(pTimer);
      if (pTimer->OnTimer()) {
        AddTimer(pTimer);
      };
    }
  }

  return hasUpdated;
}

void TimerManager::AddTimer(Timer* pTimer) {
  KillTimer(pTimer);

  int64_t diff =
      pTimer->m_triggerTime.MilliSeconds() - m_lastCheckTime.MilliSeconds();
  Timer* pListHead = nullptr;
  int64_t trigTime = pTimer->m_triggerTime.MilliSeconds();

  if (diff < 0) {
    pListHead = &m_list1[m_lastCheckTime.MilliSeconds() & (LIST1_SIZE - 1)];
  } else if (diff < LIST1_SIZE) {
    pListHead = &m_list1[trigTime & (LIST1_SIZE - 1)];
  } else if (diff < 1 << (LIST1_BITS + LIST_BITS)) {
    pListHead = &m_list2[(trigTime >> LIST1_BITS) & (LIST_SIZE - 1)];
  } else if (diff < 1 << (LIST1_BITS + 2 * LIST_BITS)) {
    pListHead =
        &m_list3[(trigTime >> (LIST1_BITS + LIST_BITS)) & (LIST_SIZE - 1)];
  } else if (diff < 1 << (LIST1_BITS + 3 * LIST_BITS)) {
    pListHead =
        &m_list4[(trigTime >> (LIST1_BITS + 2 * LIST_BITS)) & (LIST_SIZE - 1)];
  } else {
    pListHead =
        &m_list5[(trigTime >> (LIST1_BITS + 3 * LIST_BITS)) & (LIST_SIZE - 1)];
  }

  assert(!pListHead->m_prev);
  pTimer->m_prev = pListHead;
  pTimer->m_next = pListHead->m_next;
  if (pListHead->m_next != nullptr) {
    pListHead->m_next->m_prev = pTimer;
  }
  pListHead->m_next = pTimer;
}

void TimerManager::AsyncAddTimer(Timer* pTimer) {
  std::lock_guard<std::mutex> guard(m_lock);
  m_timers.push_back(pTimer);
  ++m_count;

  assert(m_count == static_cast<int>(m_timers.size()));
}

void TimerManager::ScheduleAt(Timer* pTimer, const Time& triggerTime) {
  if (!pTimer) return;

  pTimer->m_triggerTime = triggerTime;
  AddTimer(pTimer);
}

// if pTimer is never added, no effect
void TimerManager::KillTimer(Timer* pTimer) {
  if (pTimer && pTimer->m_prev) {
    pTimer->m_prev->m_next = pTimer->m_next;

    if (nullptr != pTimer->m_next) {
      pTimer->m_next->m_prev = pTimer->m_prev;
    }

    pTimer->m_prev = nullptr;
    pTimer->m_next = nullptr;
  }
}

bool TimerManager::_Cacsade(Timer pList[], int index) {
  if (index < 0 || index >= LIST_SIZE || !pList || !pList[index].m_next) {
    return false;
  }

  Timer* tmpListHead = pList[index].m_next;
  pList[index].m_next = nullptr;

  while (tmpListHead != nullptr) {
    Timer* next = tmpListHead->m_next;
    tmpListHead->m_prev = tmpListHead->m_next = nullptr;
    AddTimer(tmpListHead);
    tmpListHead = next;
  }

  return true;
}
