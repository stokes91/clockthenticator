
#ifndef DATE_UTIL_H
#define DATE_UTIL_H


class DateUtil {
 public:
  DateUtil(long seconds);

  int getCurrentYYYY();
  int getCurrentMMDD();
  int getCurrentHHMM();
  int getCurrentHHMMSS();

  private:
  long _seconds;
  int _day;
  int _month;
  int _year;

};

DateUtil::DateUtil(long seconds) {
  _seconds = seconds;
  uint32_t days = _seconds / 86400;
  // Shift to days since March 1, 0000 (Proleptic Gregorian Calendar)
  uint32_t z = days + 719468;
  uint32_t era = (z >= 0 ? z : z - 146096) / 146097;
  uint32_t doe = z - era * 146097;                            // day of era
  uint32_t yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365; // year of era
  uint32_t y = yoe + era * 400;
  uint32_t doy = doe - (365 * yoe + yoe / 4 - yoe / 100);     // day of year
  uint32_t mp = (5 * doy + 2) / 153;                          // month part
  _day = doy - (153 * mp + 2) / 5 + 1;
  _month = mp + (mp < 10 ? 3 : -9);
  _year = y + (_month <= 2);
}


// Returns the current date as the integer format YYYY
int DateUtil::getCurrentYYYY() {
  return _year % 10000;
}

// Returns the current date as the integer format MMDD
int DateUtil::getCurrentMMDD() {
  return _month * 100 + _day;
}

int DateUtil::getCurrentHHMM() {
  int time_coded_number = _seconds % 86400;

  time_coded_number = (time_coded_number / 3600) * 100 + 
    ((time_coded_number % 3600) / 60);

  return time_coded_number;
}

int DateUtil::getCurrentHHMMSS() {
  int time_coded_number = _seconds % 86400;

  time_coded_number = (time_coded_number / 3600) * 10000 + 
    ((time_coded_number % 3600) / 60) * 100 + 
    (time_coded_number % 60);

  return time_coded_number;
}
#endif // DATE_UTIL_H