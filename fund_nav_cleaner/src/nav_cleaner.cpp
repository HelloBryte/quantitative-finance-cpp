#include "nav_cleaner.hpp"
#include <algorithm>
#include <iostream>

namespace fund_nav_cleaner {

NavCleaner::NavCleaner(
    const ValidationConfig& val_config,
    const CleaningConfig& clean_config)
    : val_config_(val_config),
      clean_config_(clean_config),
      validator_(val_config),
      knn_imputer_(clean_config.knn_neighbors) {}

CleaningResult NavCleaner::clean(std::vector<NavRecord>& records) {
    CleaningResult result;
    result.total_records = static_cast<int>(records.size());
    
    if (records.empty()) {
        result.status = CleaningStatus::NO_CHANGES_NEEDED;
        return result;
    }
    
    // Check if we have enough data
    if (!has_sufficient_data(records)) {
        result.status = CleaningStatus::INSUFFICIENT_DATA;
        result.add_warning("Insufficient data points for reliable cleaning");
        return result;
    }
    
    // Step 1: Validate records
    validate_records(records, result);
    
    // Step 2: Remove duplicates if configured
    if (clean_config_.remove_duplicates) {
        remove_duplicates(records, result);
    }
    
    // Step 3: Sort by date
    sort_by_date(records);
    
    // Step 4: Fill missing values
    fill_missing_values(records, result);
    
    // Step 5: Correct outliers
    correct_outliers(records, result);
    
    // Store cleaned data
    result.cleaned_data = records;
    
    // Determine final status
    if (result.missing_filled == 0 && result.outliers_corrected == 0) {
        result.status = CleaningStatus::NO_CHANGES_NEEDED;
    } else if (result.outliers_detected > result.outliers_corrected) {
        result.status = CleaningStatus::PARTIALLY_CLEANED;
        result.add_warning("Some outliers could not be corrected");
    } else {
        result.status = CleaningStatus::SUCCESS;
    }
    
    return result;
}

CleaningResult NavCleaner::clean_fund(
    std::vector<NavRecord>& records,
    const std::string& fund_id) {
    
    // Filter records for specific fund
    std::vector<NavRecord> fund_records;
    for (const auto& record : records) {
        if (record.fund_id == fund_id) {
            fund_records.push_back(record);
        }
    }
    
    // Clean the filtered records
    CleaningResult result = clean(fund_records);
    
    // Update original records with cleaned data
    size_t cleaned_idx = 0;
    for (auto& record : records) {
        if (record.fund_id == fund_id && cleaned_idx < fund_records.size()) {
            record = fund_records[cleaned_idx++];
        }
    }
    
    return result;
}

std::map<std::string, CleaningResult> NavCleaner::clean_multiple_funds(
    std::vector<NavRecord>& records) {
    
    std::map<std::string, CleaningResult> results;
    
    // Split records by fund
    auto fund_map = split_by_fund(records);
    
    // Clean each fund separately
    std::vector<NavRecord> all_cleaned;
    
    for (auto& [fund_id, fund_records] : fund_map) {
        CleaningResult result = clean(fund_records);
        results[fund_id] = result;
        
        // Collect cleaned records
        all_cleaned.insert(all_cleaned.end(), 
                          result.cleaned_data.begin(), 
                          result.cleaned_data.end());
    }
    
    // Replace original records with cleaned data
    records = std::move(all_cleaned);
    
    return results;
}

CleaningResult NavCleaner::validate_only(const std::vector<NavRecord>& records) {
    CleaningResult result;
    result.total_records = static_cast<int>(records.size());
    
    // Make a copy for validation
    std::vector<NavRecord> records_copy = records;
    
    // Run validation
    validate_records(records_copy, result);
    
    result.status = CleaningStatus::NO_CHANGES_NEEDED;
    result.cleaned_data = records;
    
    return result;
}

void NavCleaner::set_validation_config(const ValidationConfig& config) {
    val_config_ = config;
    validator_ = NavValidator(config);
}

void NavCleaner::set_cleaning_config(const CleaningConfig& config) {
    clean_config_ = config;
    knn_imputer_.set_k(config.knn_neighbors);
}

void NavCleaner::validate_records(std::vector<NavRecord>& records, CleaningResult& result) {
    auto statuses = validator_.validate_series(records);
    
    for (size_t i = 0; i < records.size(); ++i) {
        records[i].status = statuses[i];
        
        if (statuses[i] == ValidationStatus::MISSING_VALUE) {
            // Count will be updated in fill_missing_values
        } else if (statuses[i] == ValidationStatus::EXTREME_OUTLIER ||
                   statuses[i] == ValidationStatus::SUSPICIOUS_JUMP) {
            result.outliers_detected++;
        }
    }
}

void NavCleaner::remove_duplicates(std::vector<NavRecord>& records, CleaningResult& result) {
    auto duplicate_indices = validator_.detect_duplicates(records);
    
    if (!duplicate_indices.empty()) {
        // Remove duplicates from back to front to avoid index issues
        std::sort(duplicate_indices.rbegin(), duplicate_indices.rend());
        
        for (int idx : duplicate_indices) {
            records.erase(records.begin() + idx);
        }
        
        result.add_warning("Removed " + std::to_string(duplicate_indices.size()) + " duplicate entries");
    }
}

void NavCleaner::sort_by_date(std::vector<NavRecord>& records) {
    std::sort(records.begin(), records.end(), [](const NavRecord& a, const NavRecord& b) {
        return date_less_than(a.date, b.date);
    });
}

void NavCleaner::fill_missing_values(std::vector<NavRecord>& records, CleaningResult& result) {
    int filled = 0;
    
    switch (clean_config_.default_method) {
        case CleaningMethod::LINEAR_INTERPOLATION:
            filled = Interpolation::linear_interpolate(records);
            break;
        
        case CleaningMethod::TIME_WEIGHTED_INTERPOLATION:
            filled = Interpolation::time_weighted_interpolate(records);
            break;
        
        case CleaningMethod::KNN_IMPUTATION:
            filled = knn_imputer_.impute(records);
            break;
        
        case CleaningMethod::MEDIAN_FILL:
            filled = Interpolation::median_fill(records, 5);
            break;
        
        case CleaningMethod::FORWARD_FILL:
            filled = Interpolation::forward_fill(records);
            break;
        
        case CleaningMethod::BACKWARD_FILL:
            filled = Interpolation::backward_fill(records);
            break;
        
        default:
            break;
    }
    
    result.missing_filled = filled;
    
    if (filled > 0) {
        result.add_warning("Filled " + std::to_string(filled) + " missing values using " + 
                          cleaning_method_to_string(clean_config_.default_method));
    }
}

void NavCleaner::correct_outliers(std::vector<NavRecord>& records, CleaningResult& result) {
    // Use median fill to correct outliers
    for (auto& record : records) {
        if (record.status == ValidationStatus::EXTREME_OUTLIER ||
            record.status == ValidationStatus::SUSPICIOUS_JUMP) {
            
            // Temporarily mark as missing
            record.nav = std::nullopt;
        }
    }
    
    // Fill the "missing" outliers
    int corrected = Interpolation::median_fill(records, 7);
    result.outliers_corrected = corrected;
    
    if (corrected > 0) {
        result.add_warning("Corrected " + std::to_string(corrected) + " outlier values");
    }
}

std::map<std::string, std::vector<NavRecord>> NavCleaner::split_by_fund(
    const std::vector<NavRecord>& records) {
    
    std::map<std::string, std::vector<NavRecord>> fund_map;
    
    for (const auto& record : records) {
        fund_map[record.fund_id].push_back(record);
    }
    
    return fund_map;
}

bool NavCleaner::has_sufficient_data(const std::vector<NavRecord>& records) const {
    int valid_count = 0;
    
    for (const auto& record : records) {
        if (record.has_value()) {
            valid_count++;
        }
    }
    
    return valid_count >= clean_config_.min_data_points;
}

} // namespace fund_nav_cleaner
