#include "stdafx.h"
#include "julian_date.h"

class CDate {
 private:
  int year;   // Äê
  int month;  // ÔÂ
  int date;   // ÈÕ

 public:
  CDate(int year, int month, int date) {
    this->year = year;
    this->month = month;
    this->date = date;
  }

  CDate(long JdNum) {
    long i, j, l, n;

    l = JdNum + 68569;
    n = (4 * l) / 146097;
    l = l - (146097 * n + 3) / 4;
    i = (4000 * (l + 1)) / 1461001;
    l = l - (1461 * i) / 4 + 31;
    j = (80 * l) / 2447;
    date = l - (2447 * j) / 80;
    l = j / 11;
    month = j + 2 - (12 * l);
    year = 100 * (n - 49) + i + l;
  }

  int GetYear() const {
    return year;
  }

  int GetMonth() const {
    return month;
  }

  int GetDate() const {
    return date;
  }

  int GetDay() const {
    return (GetJdNum() + 1) % 7;
  }

  long GetJdNum() const {
    long a = (month - 14) / 12;
    return (1461 * (year + 4800 + a)) / 4 + (367 * (month - 2 - 12 * (a))) / 12 -
           (3 * ((year + 4900 + a) / 100)) / 4 + date - 32075;
  }

  CDate& operator+=(int days2add) {
    long JdNum = GetJdNum() + days2add;
    long i, j, l, n;

    l = JdNum + 68569;
    n = (4 * l) / 146097;
    l = l - (146097 * n + 3) / 4;
    i = (4000 * (l + 1)) / 1461001;
    l = l - (1461 * i) / 4 + 31;
    j = (80 * l) / 2447;
    date = l - (2447 * j) / 80;
    l = j / 11;
    month = j + 2 - (12 * l);
    year = 100 * (n - 49) + i + l;
    return *this;
  }

  CDate& operator-=(int days2sub) {
    return operator+=(-days2sub);
  }
};