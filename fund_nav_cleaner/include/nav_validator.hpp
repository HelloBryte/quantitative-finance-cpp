#pragma once

#include "nav_types.hpp"
#include <vector>
#include <map>

namespace fund_nav_cleaner {

// Class for validating NAV data quality
class NavValidator {
public:
    explicit NavValidator(const ValidationConfig& config = ValidationConfig());
    
    // Validate a single record
    ValidationStatus validate_record(const NavRecord& record) const;
    
    // Validate a series of records (checking for suspicious jumps)
    std::vector<ValidationStatus> validate_series(const std::vector<NavRecord>& records) const;
    
    // Detect outliers using Z-score method
    std::vector<int> detect_outliers_zscore(const std::vector<NavRecord>& records) const;
    
    // Detect suspicious daily changes
    std::vector<int> detect_suspicious_jumps(const std::vector<NavRecord>& records) const;
    
    // Check for duplicate entries
    std::vector<int> detect_duplicates(const std::vector<NavRecord>& records) const;
    
    // Get configuration
    const ValidationConfig& get_config() const { return config_; }
    
    // Update configuration
    void set_config(const ValidationConfig& config) { config_ = config; }
    
private:
    ValidationConfig config_;
    
    // Calculate mean of valid NAV values
    double calculate_mean(const std::vector<NavRecord>& records) const;
    
    // Calculate standard deviation of valid NAV values
    double calculate_std_dev(const std::vector<NavRecord>& records, double mean) const;
    
    // Calculate daily percentage change
    double calculate_daily_change(double prev_nav, double curr_nav) const;
    
    // Validate individual field values
    bool is_valid_nav_value(double nav) const;
    bool is_valid_date(const std::tm& date) const;
};

} // namespace fund_nav_cleaner
