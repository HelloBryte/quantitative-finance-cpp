#include <gtest/gtest.h>
#include "nav_cleaner.hpp"
#include "interpolation.hpp"

using namespace fund_nav_cleaner;

// Test edge case: Empty dataset
TEST(EdgeCasesTest, EmptyDataset) {
    std::vector<NavRecord> records;
    
    NavCleaner cleaner;
    CleaningResult result = cleaner.clean(records);
    
    EXPECT_EQ(result.status, CleaningStatus::NO_CHANGES_NEEDED);
    EXPECT_EQ(result.total_records, 0);
}

// Test edge case: Single record
TEST(EdgeCasesTest, SingleRecord) {
    std::vector<NavRecord> records;
    
    NavRecord record;
    record.fund_id = "000001";
    record.date = string_to_date("2024-01-01");
    record.nav = 1.0;
    records.push_back(record);
    
    NavCleaner cleaner;
    CleaningResult result = cleaner.clean(records);
    
    // Should fail due to insufficient data
    EXPECT_EQ(result.status, CleaningStatus::INSUFFICIENT_DATA);
}

// Test edge case: All missing values
TEST(EdgeCasesTest, AllMissingValues) {
    std::vector<NavRecord> records;
    
    for (int i = 0; i < 10; ++i) {
        NavRecord record;
        record.fund_id = "000001";
        record.date = string_to_date("2024-01-" + std::to_string(i + 1));
        record.nav = std::nullopt;
        records.push_back(record);
    }
    
    NavCleaner cleaner;
    CleaningResult result = cleaner.clean(records);
    
    EXPECT_EQ(result.status, CleaningStatus::INSUFFICIENT_DATA);
}

// Test edge case: Extremely large values
TEST(EdgeCasesTest, ExtremeLargeValues) {
    std::vector<NavRecord> records;
    
    for (int i = 0; i < 5; ++i) {
        NavRecord record;
        record.fund_id = "000001";
        record.date = string_to_date("2024-01-0" + std::to_string(i + 1));
        record.nav = 1e10 + i;  // Very large values
        records.push_back(record);
    }
    
    ValidationConfig config;
    config.max_valid_nav = 1e12;  // Allow large values
    
    NavCleaner cleaner(config);
    CleaningResult result = cleaner.clean(records);
    
    // Should not detect as outliers if within configured range
    EXPECT_TRUE(result.status == CleaningStatus::SUCCESS || 
                result.status == CleaningStatus::NO_CHANGES_NEEDED);
}

// Test edge case: Zero values
TEST(EdgeCasesTest, ZeroValues) {
    std::vector<NavRecord> records;
    
    for (int i = 0; i < 5; ++i) {
        NavRecord record;
        record.fund_id = "000001";
        record.date = string_to_date("2024-01-0" + std::to_string(i + 1));
        record.nav = 0.0;
        records.push_back(record);
    }
    
    NavCleaner cleaner;
    CleaningResult result = cleaner.clean(records);
    
    EXPECT_EQ(result.status, CleaningStatus::NO_CHANGES_NEEDED);
}

// Test edge case: Identical values (no variance)
TEST(EdgeCasesTest, IdenticalValues) {
    std::vector<NavRecord> records;
    
    for (int i = 0; i < 10; ++i) {
        NavRecord record;
        record.fund_id = "000001";
        record.date = string_to_date("2024-01-" + std::to_string(i + 1));
        record.nav = 1.5;  // All same value
        records.push_back(record);
    }
    
    // Add one missing value
    records[5].nav = std::nullopt;
    
    NavCleaner cleaner;
    CleaningResult result = cleaner.clean(records);
    
    EXPECT_GT(result.missing_filled, 0);
    EXPECT_NEAR(result.cleaned_data[5].get_value(), 1.5, 0.01);
}

// Test edge case: Consecutive missing values at start
TEST(EdgeCasesTest, MissingAtStart) {
    std::vector<NavRecord> records;
    
    for (int i = 0; i < 10; ++i) {
        NavRecord record;
        record.fund_id = "000001";
        record.date = string_to_date("2024-01-" + std::to_string(i + 1));
        record.nav = (i < 3) ? std::nullopt : 1.0 + i * 0.1;
        records.push_back(record);
    }
    
    int filled = Interpolation::linear_interpolate(records);
    
    // Cannot interpolate at start
    EXPECT_EQ(filled, 0);
}

// Test edge case: Consecutive missing values at end
TEST(EdgeCasesTest, MissingAtEnd) {
    std::vector<NavRecord> records;
    
    for (int i = 0; i < 10; ++i) {
        NavRecord record;
        record.fund_id = "000001";
        record.date = string_to_date("2024-01-" + std::to_string(i + 1));
        record.nav = (i >= 7) ? std::nullopt : 1.0 + i * 0.1;
        records.push_back(record);
    }
    
    int filled = Interpolation::linear_interpolate(records);
    
    // Cannot interpolate at end
    EXPECT_EQ(filled, 0);
}

// Test edge case: Alternating missing values
TEST(EdgeCasesTest, AlternatingMissing) {
    std::vector<NavRecord> records;
    
    for (int i = 0; i < 10; ++i) {
        NavRecord record;
        record.fund_id = "000001";
        record.date = string_to_date("2024-01-" + std::to_string(i + 1));
        record.nav = (i % 2 == 0) ? (1.0 + i * 0.1) : std::nullopt;
        records.push_back(record);
    }
    
    int filled = Interpolation::linear_interpolate(records);
    
    EXPECT_GT(filled, 0);
}

// Test edge case: Very long gap
TEST(EdgeCasesTest, VeryLongGap) {
    std::vector<NavRecord> records;
    
    NavRecord r1;
    r1.fund_id = "000001";
    r1.date = string_to_date("2024-01-01");
    r1.nav = 1.0;
    records.push_back(r1);
    
    // 50 missing values
    for (int i = 2; i <= 51; ++i) {
        NavRecord record;
        record.fund_id = "000001";
        record.date = string_to_date("2024-01-" + std::to_string(i));
        record.nav = std::nullopt;
        records.push_back(record);
    }
    
    NavRecord r2;
    r2.fund_id = "000001";
    r2.date = string_to_date("2024-02-21");
    r2.nav = 2.0;
    records.push_back(r2);
    
    int filled = Interpolation::linear_interpolate(records);
    
    EXPECT_EQ(filled, 50);
}

// Test edge case: Negative accumulation
TEST(EdgeCasesTest, NegativeAccumulatedNav) {
    std::vector<NavRecord> records;
    
    for (int i = 0; i < 5; ++i) {
        NavRecord record;
        record.fund_id = "000001";
        record.date = string_to_date("2024-01-0" + std::to_string(i + 1));
        record.nav = 1.0 + i * 0.1;
        record.accumulated_nav = -1.0;  // Negative accumulated
        records.push_back(record);
    }
    
    NavCleaner cleaner;
    CleaningResult result = cleaner.clean(records);
    
    // Should still process (accumulated_nav doesn't affect NAV cleaning)
    EXPECT_TRUE(result.status == CleaningStatus::SUCCESS || 
                result.status == CleaningStatus::NO_CHANGES_NEEDED);
}
