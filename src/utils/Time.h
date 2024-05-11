#pragma once
#include <iomanip>
#include <sstream>
#include <string>

namespace lbm::utils::time {

using string = std::string;

struct mTimeStructure {
    int mYear;
    int mMonth;
    int mDay;
    int mHour;
    int mMinute;
    int mSecond;

    mTimeStructure() : mYear(0), mMonth(0), mDay(0), mHour(0), mMinute(0), mSecond(0) {}
    mTimeStructure(int year, int month, int day, int hour, int minute, int second)
    : mYear(year),
      mMonth(month),
      mDay(day),
      mHour(hour),
      mMinute(minute),
      mSecond(second) {}

    std::string toString() const {
        std::ostringstream ss;
        ss << std::setw(4) << std::setfill('0') << mYear << "-" << std::setw(2) << std::setfill('0') << mMonth << "-"
           << std::setw(2) << std::setfill('0') << mDay << " " << std::setw(2) << std::setfill('0') << mHour << ":"
           << std::setw(2) << std::setfill('0') << mMinute << ":" << std::setw(2) << std::setfill('0') << mSecond;
        return ss.str();
    }
};

inline mTimeStructure getCurrentTime() {
    std::time_t t = std::time(nullptr);
    std::tm     now;
    localtime_s(&now, &t);
    return mTimeStructure{now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec};
}

inline std::string getCurrentTimeString() { return getCurrentTime().toString(); }

inline std::time_t getCurrentTimestamp() { return std::time(nullptr); }

inline std::string timestampToDateString(std::time_t timestamp) {
    std::tm timeinfo;
    localtime_s(&timeinfo, &timestamp);
    mTimeStructure ts{
        timeinfo.tm_year + 1900,
        timeinfo.tm_mon + 1,
        timeinfo.tm_mday,
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec
    };
    return ts.toString();
}

inline int getYear() { return getCurrentTime().mYear; }

inline int getMonth() { return getCurrentTime().mMonth; }

inline int getDay() { return getCurrentTime().mDay; }

inline int getHour() { return getCurrentTime().mHour; }

inline int getMinute() { return getCurrentTime().mMinute; }

inline int getSecond() { return getCurrentTime().mSecond; }

inline bool isDate1AfterDate2(const mTimeStructure& date1, const mTimeStructure& date2) {
    if (date1.mYear != date2.mYear) return date1.mYear > date2.mYear;
    if (date1.mMonth != date2.mMonth) return date1.mMonth > date2.mMonth;
    if (date1.mDay != date2.mDay) return date1.mDay > date2.mDay;
    if (date1.mHour != date2.mHour) return date1.mHour > date2.mHour;
    if (date1.mMinute != date2.mMinute) return date1.mMinute > date2.mMinute;
    return date1.mSecond > date2.mSecond;
}

inline bool isDate1EqualToDate2(const mTimeStructure& date1, const mTimeStructure& date2) {
    return date1.mYear == date2.mYear && date1.mMonth == date2.mMonth && date1.mDay == date2.mDay
        && date1.mHour == date2.mHour && date1.mMinute == date2.mMinute && date1.mSecond == date2.mSecond;
}

inline bool isDate1BeforeDate2(const mTimeStructure& date1, const mTimeStructure& date2) {
    return !isDate1AfterDate2(date1, date2) && !isDate1EqualToDate2(date1, date2);
}

inline mTimeStructure stringToTimeStructure(const std::string& timeStr) {
    std::istringstream ss(timeStr);
    mTimeStructure     ts;
    char               delimiter;
    ss >> ts.mYear >> delimiter >> ts.mMonth >> delimiter >> ts.mDay >> ts.mHour >> delimiter >> ts.mMinute >> delimiter
        >> ts.mSecond;
    return ts;
}


} // namespace lbm::utils::time