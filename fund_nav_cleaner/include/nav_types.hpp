#pragma once

#include <string>
#include <vector>
#include <optional>
#include <ctime>

namespace fund_nav_cleaner {

// Validation status for data quality checks
enum class ValidationStatus {
    VALID,
    MISSING_VALUE,
    NEGATIVE_VALUE,
    EXTREME_OUTLIER,      // 超过3-sigma
    SUSPICIOUS_JUMP,      // 日变化超过阈值
    INVALID_DATE,
    DUPLICATE_ENTRY
};

// Cleaning method applied to data
enum class CleaningMethod {
    NONE,
    LINEAR_INTERPOLATION,
    TIME_WEIGHTED_INTERPOLATION,
    KNN_IMPUTATION,
    MEDIAN_FILL,
    FORWARD_FILL,
    BACKWARD_FILL
};

// Result status for cleaning operations
enum class CleaningStatus {
    SUCCESS,
    NO_CHANGES_NEEDED,
    PARTIALLY_CLEANED,
    FAILED,
    INSUFFICIENT_DATA
};

// Structure to hold a single NAV (Net Asset Value) record
struct NavRecord {
    std::string fund_id;          // 基金代码
    std::tm date;                 // 日期
    std::optional<double> nav;    // 净值 (optional for missing values)
    double accumulated_nav;       // 累计净值
    ValidationStatus status;      // 验证状态
    
    NavRecord() 
        : fund_id(""), 
          date{}, 
          nav(std::nullopt), 
          accumulated_nav(0.0),
          status(ValidationStatus::VALID) {}
    
    bool has_value() const { return nav.has_value(); }
    double get_value() const { return nav.value_or(0.0); }
};

// Structure to track cleaning results
struct CleaningResult {
    std::vector<NavRecord> cleaned_data;
    int total_records;
    int missing_filled;
    int outliers_detected;
    int outliers_corrected;
    CleaningStatus status;
    std::vector<std::string> warnings;
    
    CleaningResult() 
        : total_records(0),
          missing_filled(0),
          outliers_detected(0),
          outliers_corrected(0),
          status(CleaningStatus::SUCCESS) {}
    
    void add_warning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

// Configuration for validation rules
struct ValidationConfig {
    double max_daily_change_pct;    // 最大日变化百分比 (e.g., 0.5 = 50%)
    double z_score_threshold;       // Z-score阈值 (e.g., 3.0 = 3-sigma)
    double min_valid_nav;           // 最小有效净值
    double max_valid_nav;           // 最大有效净值
    bool allow_negative;            // 是否允许负值
    
    ValidationConfig()
        : max_daily_change_pct(0.5),
          z_score_threshold(3.0),
          min_valid_nav(0.0),
          max_valid_nav(1000.0),
          allow_negative(false) {}
};

// Configuration for cleaning operations
struct CleaningConfig {
    CleaningMethod default_method;
    int knn_neighbors;              // KNN填补时的邻居数
    int min_data_points;            // 最少需要的数据点数
    bool remove_duplicates;         // 是否删除重复数据
    bool forward_fill_first;        // 是否优先使用前向填充
    
    CleaningConfig()
        : default_method(CleaningMethod::LINEAR_INTERPOLATION),
          knn_neighbors(5),
          min_data_points(3),
          remove_duplicates(true),
          forward_fill_first(false) {}
};

// Helper function: Convert std::tm to string (YYYY-MM-DD)
std::string date_to_string(const std::tm& date);

// Helper function: Parse string (YYYY-MM-DD) to std::tm
std::tm string_to_date(const std::string& date_str);

// Helper function: Calculate days difference between two dates
int days_between(const std::tm& date1, const std::tm& date2);

// Helper function: Compare two dates
bool date_less_than(const std::tm& date1, const std::tm& date2);

// Helper function: Convert ValidationStatus to string
std::string validation_status_to_string(ValidationStatus status);

// Helper function: Convert CleaningMethod to string
std::string cleaning_method_to_string(CleaningMethod method);

} // namespace fund_nav_cleaner
