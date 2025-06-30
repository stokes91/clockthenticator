// Copyright 2025 Alexander Stokes
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//

#ifndef DATE_UTIL_H
#define DATE_UTIL_H

enum class SundayModifier { First, Second, Last };

struct DSTRule {
  int month;
  int hour;
  int utcOffset; // hours
  SundayModifier modifier;
};

struct DSTBounds {
  long startUTC;
  long endUTC;
};

class DateUtil {
public:
  DateUtil(long seconds) : _seconds(seconds) {
    long days = _seconds / 86400;
    long z = days + 719468; // days since 0000-03-01
    long era = (z >= 0 ? z : z - 146096) / 146097;
    long doe = z - era * 146097;
    long yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    long y = yoe + era * 400;
    long doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    long mp = (5 * doy + 2) / 153;
    _day = static_cast<int>(doy - (153 * mp + 2) / 5 + 1);
    _month = static_cast<int>(mp < 10 ? mp + 3 : mp - 9);
    _year = static_cast<int>(y + (_month <= 2 ? 1 : 0));
  }

  int getCurrentYYYY() const {
    return _year;
  }

  int getCurrentMMDD() const {
    return _month * 100 + _day;
  }

  int getCurrentHHMM() const {
    int secondsInDay = _seconds % 86400;
    return (secondsInDay / 3600) * 100 + (secondsInDay % 3600) / 60;
  }

  int getCurrentHHMMSS() const {
    int secondsInDay = _seconds % 86400;
    return (secondsInDay / 3600) * 10000 +
           ((secondsInDay % 3600) / 60) * 100 +
           (secondsInDay % 60);
  }

  int getDayOfWeek() const {
    long days = _seconds / 86400;
    return static_cast<int>((days + 3) % 7); // ISO: 0=Mon, 6=Sun
  }

  int getWeekOfMonth() const {
    int day1 = toDayNumber(_year, _month, 1);
    int dow1 = (day1 + 3) % 7;
    return (_day + dow1 - 1) / 7 + 1;
  }

  int year() const { return _year; }
  int month() const { return _month; }
  int day() const { return _day; }
  long seconds() const { return _seconds; }

  static long toUnix(int year, int month, int day, int hour = 0, int min = 0, int sec = 0) {
    int dayNumber = toDayNumber(year, month, day);
    return static_cast<long>(dayNumber) * 86400 + hour * 3600 + min * 60 + sec;
  }

private:
  static int toDayNumber(int year, int month, int day) {
    int adjYear = (month > 2) ? year : year - 1;
    int adjMonth = (month > 2) ? month - 3 : month + 9;

    int era = adjYear / 400;
    int yoe = adjYear - era * 400;

    int doy = (153 * adjMonth + 2) / 5 + day - 1;
    int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
  }

  long _seconds;
  int _day;
  int _month;
  int _year;
};

// ---- DST LOGIC ----

inline long getModifiedSundayUTC(int year, const DSTRule& rule) {
  if (rule.modifier == SundayModifier::Last) {
    for (int day = 31; day > 0; --day) {
      long candidateTime = DateUtil::toUnix(year, rule.month, day, rule.hour);
      DateUtil candidate(candidateTime);
      if (candidate.getDayOfWeek() == 6) { // Sunday
        return candidateTime - rule.utcOffset * 3600;
      }
    }
  } else {
    long firstTime = DateUtil::toUnix(year, rule.month, 1, rule.hour);
    DateUtil first(firstTime);
    int firstWeekday = first.getDayOfWeek();
    int daysToSunday = (7 - firstWeekday + 6) % 7;
    int nth = (rule.modifier == SundayModifier::First) ? 1 : 2;
    int day = 1 + daysToSunday + 7 * (nth - 1);
    return DateUtil::toUnix(year, rule.month, day, rule.hour) - rule.utcOffset * 3600;
  }
  return 0;
}

inline DSTBounds updateDSTBounds(int year, const DSTRule& startRule, const DSTRule& endRule) {
  return {
    getModifiedSundayUTC(year, startRule),
    getModifiedSundayUTC(year, endRule)
  };
}

inline bool isInDST(long timestampUTC, const DSTBounds& bounds) {
  return timestampUTC >= bounds.startUTC && timestampUTC < bounds.endUTC;
}

inline int getUTCOffsetForRegion(long timestampUTC, int standardOffset, const DSTBounds& bounds) {
  return isInDST(timestampUTC, bounds) ? standardOffset + 1 : standardOffset;
}

#endif // DATE_UTIL_H