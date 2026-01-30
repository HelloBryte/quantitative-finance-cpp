#include <gtest/gtest.h>
#include "nav_cleaner.hpp"

using namespace fund_nav_cleaner;

class NavCleanerTest : public ::testing::Test {
protected:
    ValidationConfig val_config;
    CleaningConfig clean_config;
    
    void SetUp() override {
        val_config.max_daily_change_pct = 0.5;
        val_config.z_score_threshold = 3.0;
        val_config.min_valid_nav = 0.0;
        val_config.max_valid_nav = 100.0;
        
        clean_config.default_method = CleaningMethod::LINEAR_INTERPOLATION;
        clean_config.knn_neighbors = 3;
        clean_config.min_data_points = 3;
    }
    
    std::vector<NavRecord> create_sample_data() {
        std::vector<NavRecord> records;
        
        for (int i = 0; i < 10; ++i) {
            NavRecord record;
            record.fund_id = "000001";
            record.date = string_to_date("2024-01-" + std::to_string(i + 1));
            
            if (i == 3 || i == 7) {
                record.nav = std::nullopt;  // Missing values
            } else if (i == 5) {
                record.nav = 10.0;  // Outlier
            } else {
                record.nav = 1.0 + i * 0.1;
            }
            
            record.accumulated_nav = 1.0 + i * 0.15;
            records.push_back(record);
        }
        
        return records;
    }
};

TEST_F(NavCleanerTest, BasicCleaning) {
    NavCleaner cleaner(val_config, clean_config);
    auto records = create_sample_data();
    
    CleaningResult result = cleaner.clean(records);
    
    EXPECT_EQ(result.status, CleaningStatus::SUCCESS);
    EXPECT_GT(result.missing_filled, 0);
    EXPECT_EQ(result.cleaned_data.size(), records.size());
}

TEST_F(NavCleanerTest, FillMissingValues) {
    NavCleaner cleaner(val_config, clean_config);
    auto records = create_sample_data();
    
    CleaningResult result = cleaner.clean(records);
    
    EXPECT_EQ(result.missing_filled, 2);
    
    // Check that previously missing values are now filled
    EXPECT_TRUE(result.cleaned_data[3].has_value());
    EXPECT_TRUE(result.cleaned_data[7].has_value());
}

TEST_F(NavCleanerTest, DetectAndCorrectOutliers) {
    NavCleaner cleaner(val_config, clean_config);
    auto records = create_sample_data();
    
    CleaningResult result = cleaner.clean(records);
    
    EXPECT_GT(result.outliers_detected, 0);
    EXPECT_GT(result.outliers_corrected, 0);
}

TEST_F(NavCleanerTest, CleanWithKNN) {
    clean_config.default_method = CleaningMethod::KNN_IMPUTATION;
    NavCleaner cleaner(val_config, clean_config);
    
    auto records = create_sample_data();
    CleaningResult result = cleaner.clean(records);
    
    EXPECT_EQ(result.status, CleaningStatus::SUCCESS);
    EXPECT_GT(result.missing_filled, 0);
}

TEST_F(NavCleanerTest, InsufficientData) {
    NavCleaner cleaner(val_config, clean_config);
    
    std::vector<NavRecord> records;
    for (int i = 0; i < 2; ++i) {  // Only 2 records, less than min_data_points
        NavRecord record;
        record.fund_id = "000001";
        record.date = string_to_date("2024-01-0" + std::to_string(i + 1));
        record.nav = 1.0;
        records.push_back(record);
    }
    
    CleaningResult result = cleaner.clean(records);
    
    EXPECT_EQ(result.status, CleaningStatus::INSUFFICIENT_DATA);
}

TEST_F(NavCleanerTest, NoChangesNeeded) {
    NavCleaner cleaner(val_config, clean_config);
    
    std::vector<NavRecord> records;
    for (int i = 0; i < 10; ++i) {
        NavRecord record;
        record.fund_id = "000001";
        record.date = string_to_date("2024-01-" + std::to_string(i + 1));
        record.nav = 1.0 + i * 0.01;  // Clean data, no issues
        records.push_back(record);
    }
    
    CleaningResult result = cleaner.clean(records);
    
    EXPECT_EQ(result.status, CleaningStatus::NO_CHANGES_NEEDED);
    EXPECT_EQ(result.missing_filled, 0);
    EXPECT_EQ(result.outliers_corrected, 0);
}

TEST_F(NavCleanerTest, RemoveDuplicates) {
    clean_config.remove_duplicates = true;
    NavCleaner cleaner(val_config, clean_config);
    
    std::vector<NavRecord> records;
    
    NavRecord r1;
    r1.fund_id = "000001";
    r1.date = string_to_date("2024-01-01");
    r1.nav = 1.0;
    records.push_back(r1);
    records.push_back(r1);  // Duplicate
    
    NavRecord r2;
    r2.fund_id = "000001";
    r2.date = string_to_date("2024-01-02");
    r2.nav = 1.1;
    records.push_back(r2);
    
    NavRecord r3;
    r3.fund_id = "000001";
    r3.date = string_to_date("2024-01-03");
    r3.nav = 1.2;
    records.push_back(r3);
    
    CleaningResult result = cleaner.clean(records);
    
    EXPECT_LT(result.cleaned_data.size(), records.size());
    EXPECT_FALSE(result.warnings.empty());
}

TEST_F(NavCleanerTest, CleanSpecificFund) {
    NavCleaner cleaner(val_config, clean_config);
    
    std::vector<NavRecord> records;
    
    // Add records for two funds
    for (int i = 0; i < 5; ++i) {
        NavRecord r1;
        r1.fund_id = "000001";
        r1.date = string_to_date("2024-01-0" + std::to_string(i + 1));
        r1.nav = (i == 2) ? std::nullopt : 1.0 + i * 0.1;
        records.push_back(r1);
        
        NavRecord r2;
        r2.fund_id = "000002";
        r2.date = string_to_date("2024-01-0" + std::to_string(i + 1));
        r2.nav = 2.0 + i * 0.1;
        records.push_back(r2);
    }
    
    CleaningResult result = cleaner.clean_fund(records, "000001");
    
    EXPECT_GT(result.missing_filled, 0);
    EXPECT_EQ(result.total_records, 5);
}

TEST_F(NavCleanerTest, CleanMultipleFunds) {
    NavCleaner cleaner(val_config, clean_config);
    
    std::vector<NavRecord> records;
    
    // Add records for two funds with missing values
    for (int i = 0; i < 5; ++i) {
        NavRecord r1;
        r1.fund_id = "000001";
        r1.date = string_to_date("2024-01-0" + std::to_string(i + 1));
        r1.nav = (i == 2) ? std::nullopt : 1.0 + i * 0.1;
        records.push_back(r1);
        
        NavRecord r2;
        r2.fund_id = "000002";
        r2.date = string_to_date("2024-01-0" + std::to_string(i + 1));
        r2.nav = (i == 3) ? std::nullopt : 2.0 + i * 0.1;
        records.push_back(r2);
    }
    
    auto results = cleaner.clean_multiple_funds(records);
    
    EXPECT_EQ(results.size(), 2);
    EXPECT_TRUE(results.find("000001") != results.end());
    EXPECT_TRUE(results.find("000002") != results.end());
    EXPECT_GT(results["000001"].missing_filled, 0);
    EXPECT_GT(results["000002"].missing_filled, 0);
}

TEST_F(NavCleanerTest, ValidateOnly) {
    NavCleaner cleaner(val_config, clean_config);
    auto records = create_sample_data();
    
    CleaningResult result = cleaner.validate_only(records);
    
    EXPECT_EQ(result.status, CleaningStatus::NO_CHANGES_NEEDED);
    EXPECT_EQ(result.missing_filled, 0);
    EXPECT_GT(result.outliers_detected, 0);
}

TEST_F(NavCleanerTest, ConfigurationUpdate) {
    NavCleaner cleaner(val_config, clean_config);
    
    ValidationConfig new_val_config;
    new_val_config.max_daily_change_pct = 0.3;
    cleaner.set_validation_config(new_val_config);
    
    EXPECT_DOUBLE_EQ(cleaner.get_validation_config().max_daily_change_pct, 0.3);
    
    CleaningConfig new_clean_config;
    new_clean_config.knn_neighbors = 7;
    cleaner.set_cleaning_config(new_clean_config);
    
    EXPECT_EQ(cleaner.get_cleaning_config().knn_neighbors, 7);
}
