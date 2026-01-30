#pragma once

#include "nav_types.hpp"
#include "nav_validator.hpp"
#include "interpolation.hpp"
#include "knn_imputer.hpp"
#include <vector>
#include <string>

namespace fund_nav_cleaner {

// Main class for cleaning NAV data
class NavCleaner {
public:
    NavCleaner(
        const ValidationConfig& val_config = ValidationConfig(),
        const CleaningConfig& clean_config = CleaningConfig()
    );
    
    // Clean a vector of NAV records
    CleaningResult clean(std::vector<NavRecord>& records);
    
    // Clean data from a specific fund
    CleaningResult clean_fund(
        std::vector<NavRecord>& records,
        const std::string& fund_id
    );
    
    // Process multiple funds
    std::map<std::string, CleaningResult> clean_multiple_funds(
        std::vector<NavRecord>& records
    );
    
    // Validate data without cleaning
    CleaningResult validate_only(const std::vector<NavRecord>& records);
    
    // Get configurations
    const ValidationConfig& get_validation_config() const { return val_config_; }
    const CleaningConfig& get_cleaning_config() const { return clean_config_; }
    
    // Update configurations
    void set_validation_config(const ValidationConfig& config);
    void set_cleaning_config(const CleaningConfig& config);
    
private:
    ValidationConfig val_config_;
    CleaningConfig clean_config_;
    NavValidator validator_;
    KnnImputer knn_imputer_;
    
    // Step 1: Validate and mark issues
    void validate_records(std::vector<NavRecord>& records, CleaningResult& result);
    
    // Step 2: Remove duplicates
    void remove_duplicates(std::vector<NavRecord>& records, CleaningResult& result);
    
    // Step 3: Sort by date
    void sort_by_date(std::vector<NavRecord>& records);
    
    // Step 4: Fill missing values
    void fill_missing_values(std::vector<NavRecord>& records, CleaningResult& result);
    
    // Step 5: Correct outliers
    void correct_outliers(std::vector<NavRecord>& records, CleaningResult& result);
    
    // Helper: Split records by fund_id
    std::map<std::string, std::vector<NavRecord>> split_by_fund(
        const std::vector<NavRecord>& records
    );
    
    // Helper: Check if enough data for cleaning
    bool has_sufficient_data(const std::vector<NavRecord>& records) const;
};

} // namespace fund_nav_cleaner
