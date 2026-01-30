#include <gtest/gtest.h>
#include "nav_validator.hpp"

using namespace fund_nav_cleaner;

class NavValidatorTest : public ::testing::Test {
protected:
    ValidationConfig config;
    
    void SetUp() override {
        config.max_daily_change_pct = 0.5;
        config.z_score_threshold = 3.0;
        config.min_valid_nav = 0.0;
        config.max_valid_nav = 100.0;
        config.allow_negative = false;
    }
};

TEST_F(NavValidatorTest, ValidateValidRecord) {
    NavValidator validator(config);
    
    NavRecord record;
    record.fund_id = "000001";
    record.date = string_to_date("2024-01-01");
    record.nav = 1.5;
    record.accumulated_nav = 1.5;
    
    ValidationStatus status = validator.validate_record(record);
    EXPECT_EQ(status, ValidationStatus::VALID);
}

TEST_F(NavValidatorTest, DetectMissingValue) {
    NavValidator validator(config);
    
    NavRecord record;
    record.fund_id = "000001";
    record.date = string_to_date("2024-01-01");
    record.nav = std::nullopt;
    
    ValidationStatus status = validator.validate_record(record);
    EXPECT_EQ(status, ValidationStatus::MISSING_VALUE);
}

TEST_F(NavValidatorTest, DetectNegativeValue) {
    NavValidator validator(config);
    
    NavRecord record;
    record.fund_id = "000001";
    record.date = string_to_date("2024-01-01");
    record.nav = -1.0;
    
    ValidationStatus status = validator.validate_record(record);
    EXPECT_EQ(status, ValidationStatus::NEGATIVE_VALUE);
}

TEST_F(NavValidatorTest, DetectExtremeOutlier) {
    NavValidator validator(config);
    
    NavRecord record;
    record.fund_id = "000001";
    record.date = string_to_date("2024-01-01");
    record.nav = 150.0;  // Beyond max_valid_nav
    
    ValidationStatus status = validator.validate_record(record);
    EXPECT_EQ(status, ValidationStatus::EXTREME_OUTLIER);
}

TEST_F(NavValidatorTest, DetectSuspiciousJump) {
    NavValidator validator(config);
    
    std::vector<NavRecord> records;
    for (int i = 0; i < 5; ++i) {
        NavRecord record;
        record.fund_id = "000001";
        record.date = string_to_date("2024-01-0" + std::to_string(i + 1));
        record.nav = (i == 2) ? 5.0 : 1.0;  // Huge jump at index 2
        records.push_back(record);
    }
    
    auto jumps = validator.detect_suspicious_jumps(records);
    EXPECT_EQ(jumps.size(), 2);  // Both jump up and jump down
}

TEST_F(NavValidatorTest, DetectOutliersZScore) {
    NavValidator validator(config);
    
    std::vector<NavRecord> records;
    // Create 10 records with values around 1.0, and one outlier at 10.0
    for (int i = 0; i < 10; ++i) {
        NavRecord record;
        record.fund_id = "000001";
        record.date = string_to_date("2024-01-" + std::to_string(i + 1));
        record.nav = (i == 5) ? 10.0 : 1.0 + i * 0.01;
        records.push_back(record);
    }
    
    auto outliers = validator.detect_outliers_zscore(records);
    EXPECT_GT(outliers.size(), 0);
    EXPECT_EQ(outliers[0], 5);  // Index 5 should be detected
}

TEST_F(NavValidatorTest, DetectDuplicates) {
    NavValidator validator(config);
    
    std::vector<NavRecord> records;
    
    NavRecord record1;
    record1.fund_id = "000001";
    record1.date = string_to_date("2024-01-01");
    record1.nav = 1.0;
    records.push_back(record1);
    
    NavRecord record2 = record1;  // Duplicate
    records.push_back(record2);
    
    NavRecord record3;
    record3.fund_id = "000001";
    record3.date = string_to_date("2024-01-02");
    record3.nav = 1.05;
    records.push_back(record3);
    
    auto duplicates = validator.detect_duplicates(records);
    EXPECT_EQ(duplicates.size(), 1);
    EXPECT_EQ(duplicates[0], 1);
}

TEST_F(NavValidatorTest, ValidateSeries) {
    NavValidator validator(config);
    
    std::vector<NavRecord> records;
    for (int i = 0; i < 5; ++i) {
        NavRecord record;
        record.fund_id = "000001";
        record.date = string_to_date("2024-01-0" + std::to_string(i + 1));
        
        if (i == 2) {
            record.nav = std::nullopt;  // Missing value
        } else {
            record.nav = 1.0 + i * 0.01;
        }
        records.push_back(record);
    }
    
    auto statuses = validator.validate_series(records);
    EXPECT_EQ(statuses.size(), 5);
    EXPECT_EQ(statuses[2], ValidationStatus::MISSING_VALUE);
}
