#include "nav_types.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>

namespace fund_nav_cleaner {

std::string date_to_string(const std::tm& date) {
    std::ostringstream oss;
    oss << std::setfill('0') 
        << std::setw(4) << (date.tm_year + 1900) << "-"
        << std::setw(2) << (date.tm_mon + 1) << "-"
        << std::setw(2) << date.tm_mday;
    return oss.str();
}

std::tm string_to_date(const std::string& date_str) {
    std::tm date = {};
    std::istringstream ss(date_str);
    ss >> std::get_time(&date, "%Y-%m-%d");
    return date;
}

int days_between(const std::tm& date1, const std::tm& date2) {
    std::time_t time1 = std::mktime(const_cast<std::tm*>(&date1));
    std::time_t time2 = std::mktime(const_cast<std::tm*>(&date2));
    double seconds = std::difftime(time2, time1);
    return static_cast<int>(std::abs(seconds) / 86400.0);
}

bool date_less_than(const std::tm& date1, const std::tm& date2) {
    if (date1.tm_year != date2.tm_year) {
        return date1.tm_year < date2.tm_year;
    }
    if (date1.tm_mon != date2.tm_mon) {
        return date1.tm_mon < date2.tm_mon;
    }
    return date1.tm_mday < date2.tm_mday;
}

std::string validation_status_to_string(ValidationStatus status) {
    switch (status) {
        case ValidationStatus::VALID:
            return "VALID";
        case ValidationStatus::MISSING_VALUE:
            return "MISSING_VALUE";
        case ValidationStatus::NEGATIVE_VALUE:
            return "NEGATIVE_VALUE";
        case ValidationStatus::EXTREME_OUTLIER:
            return "EXTREME_OUTLIER";
        case ValidationStatus::SUSPICIOUS_JUMP:
            return "SUSPICIOUS_JUMP";
        case ValidationStatus::INVALID_DATE:
            return "INVALID_DATE";
        case ValidationStatus::DUPLICATE_ENTRY:
            return "DUPLICATE_ENTRY";
        default:
            return "UNKNOWN";
    }
}

std::string cleaning_method_to_string(CleaningMethod method) {
    switch (method) {
        case CleaningMethod::NONE:
            return "NONE";
        case CleaningMethod::LINEAR_INTERPOLATION:
            return "LINEAR_INTERPOLATION";
        case CleaningMethod::TIME_WEIGHTED_INTERPOLATION:
            return "TIME_WEIGHTED_INTERPOLATION";
        case CleaningMethod::KNN_IMPUTATION:
            return "KNN_IMPUTATION";
        case CleaningMethod::MEDIAN_FILL:
            return "MEDIAN_FILL";
        case CleaningMethod::FORWARD_FILL:
            return "FORWARD_FILL";
        case CleaningMethod::BACKWARD_FILL:
            return "BACKWARD_FILL";
        default:
            return "UNKNOWN";
    }
}

} // namespace fund_nav_cleaner
