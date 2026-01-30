#include "nav_validator.hpp"
#include <cmath>
#include <algorithm>
#include <set>

namespace fund_nav_cleaner {

NavValidator::NavValidator(const ValidationConfig& config)
    : config_(config) {}

ValidationStatus NavValidator::validate_record(const NavRecord& record) const {
    // Check for missing value
    if (!record.has_value()) {
        return ValidationStatus::MISSING_VALUE;
    }
    
    double nav = record.get_value();
    
    // Check for negative value
    if (!config_.allow_negative && nav < 0) {
        return ValidationStatus::NEGATIVE_VALUE;
    }
    
    // Check range
    if (!is_valid_nav_value(nav)) {
        return ValidationStatus::EXTREME_OUTLIER;
    }
    
    // Check date validity
    if (!is_valid_date(record.date)) {
        return ValidationStatus::INVALID_DATE;
    }
    
    return ValidationStatus::VALID;
}

std::vector<ValidationStatus> NavValidator::validate_series(
    const std::vector<NavRecord>& records) const {
    
    std::vector<ValidationStatus> statuses;
    statuses.reserve(records.size());
    
    // First pass: validate individual records
    for (const auto& record : records) {
        statuses.push_back(validate_record(record));
    }
    
    // Second pass: check for suspicious jumps
    auto jump_indices = detect_suspicious_jumps(records);
    for (int idx : jump_indices) {
        if (statuses[idx] == ValidationStatus::VALID) {
            statuses[idx] = ValidationStatus::SUSPICIOUS_JUMP;
        }
    }
    
    // Third pass: check for outliers using Z-score
    auto outlier_indices = detect_outliers_zscore(records);
    for (int idx : outlier_indices) {
        if (statuses[idx] == ValidationStatus::VALID) {
            statuses[idx] = ValidationStatus::EXTREME_OUTLIER;
        }
    }
    
    return statuses;
}

std::vector<int> NavValidator::detect_outliers_zscore(
    const std::vector<NavRecord>& records) const {
    
    std::vector<int> outliers;
    
    if (records.empty()) {
        return outliers;
    }
    
    // Calculate mean and standard deviation
    double mean = calculate_mean(records);
    double std_dev = calculate_std_dev(records, mean);
    
    // Avoid division by zero
    if (std_dev < 1e-10) {
        return outliers;
    }
    
    // Find outliers
    for (size_t i = 0; i < records.size(); ++i) {
        if (records[i].has_value()) {
            double nav = records[i].get_value();
            double z_score = std::abs((nav - mean) / std_dev);
            
            if (z_score > config_.z_score_threshold) {
                outliers.push_back(static_cast<int>(i));
            }
        }
    }
    
    return outliers;
}

std::vector<int> NavValidator::detect_suspicious_jumps(
    const std::vector<NavRecord>& records) const {
    
    std::vector<int> jumps;
    
    if (records.size() < 2) {
        return jumps;
    }
    
    for (size_t i = 1; i < records.size(); ++i) {
        if (records[i-1].has_value() && records[i].has_value()) {
            double prev_nav = records[i-1].get_value();
            double curr_nav = records[i].get_value();
            
            double change_pct = calculate_daily_change(prev_nav, curr_nav);
            
            if (std::abs(change_pct) > config_.max_daily_change_pct) {
                jumps.push_back(static_cast<int>(i));
            }
        }
    }
    
    return jumps;
}

std::vector<int> NavValidator::detect_duplicates(
    const std::vector<NavRecord>& records) const {
    
    std::vector<int> duplicates;
    std::set<std::string> seen_dates;
    
    for (size_t i = 0; i < records.size(); ++i) {
        std::string date_str = date_to_string(records[i].date);
        std::string key = records[i].fund_id + "_" + date_str;
        
        if (seen_dates.find(key) != seen_dates.end()) {
            duplicates.push_back(static_cast<int>(i));
        } else {
            seen_dates.insert(key);
        }
    }
    
    return duplicates;
}

double NavValidator::calculate_mean(const std::vector<NavRecord>& records) const {
    double sum = 0.0;
    int count = 0;
    
    for (const auto& record : records) {
        if (record.has_value()) {
            sum += record.get_value();
            count++;
        }
    }
    
    return count > 0 ? sum / count : 0.0;
}

double NavValidator::calculate_std_dev(
    const std::vector<NavRecord>& records, double mean) const {
    
    double sum_squared_diff = 0.0;
    int count = 0;
    
    for (const auto& record : records) {
        if (record.has_value()) {
            double diff = record.get_value() - mean;
            sum_squared_diff += diff * diff;
            count++;
        }
    }
    
    return count > 1 ? std::sqrt(sum_squared_diff / (count - 1)) : 0.0;
}

double NavValidator::calculate_daily_change(double prev_nav, double curr_nav) const {
    if (std::abs(prev_nav) < 1e-10) {
        return 0.0;
    }
    return (curr_nav - prev_nav) / prev_nav;
}

bool NavValidator::is_valid_nav_value(double nav) const {
    return nav >= config_.min_valid_nav && nav <= config_.max_valid_nav;
}

bool NavValidator::is_valid_date(const std::tm& date) const {
    // Basic date validation
    if (date.tm_year < 0 || date.tm_year > 200) {  // 1900-2100
        return false;
    }
    if (date.tm_mon < 0 || date.tm_mon > 11) {
        return false;
    }
    if (date.tm_mday < 1 || date.tm_mday > 31) {
        return false;
    }
    return true;
}

} // namespace fund_nav_cleaner
