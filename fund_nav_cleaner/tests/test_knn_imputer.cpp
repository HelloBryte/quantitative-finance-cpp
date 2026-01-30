#include <gtest/gtest.h>
#include "knn_imputer.hpp"

using namespace fund_nav_cleaner;

class KnnImputerTest : public ::testing::Test {
protected:
    std::vector<NavRecord> create_test_records() {
        std::vector<NavRecord> records;
        
        // Create a pattern: values increase by 0.1 each day
        for (int i = 0; i < 10; ++i) {
            NavRecord record;
            record.fund_id = "000001";
            record.date = string_to_date("2024-01-" + std::to_string(i + 1));
            record.nav = 1.0 + i * 0.1;
            record.accumulated_nav = 1.0 + i * 0.15;
            records.push_back(record);
        }
        
        // Create gaps at indices 3 and 7
        records[3].nav = std::nullopt;
        records[7].nav = std::nullopt;
        
        return records;
    }
};

TEST_F(KnnImputerTest, BasicImputation) {
    KnnImputer imputer(3);
    auto records = create_test_records();
    
    int filled = imputer.impute(records);
    
    EXPECT_EQ(filled, 2);
    EXPECT_TRUE(records[3].has_value());
    EXPECT_TRUE(records[7].has_value());
    
    // Values should be reasonable (between neighbors)
    EXPECT_GT(records[3].get_value(), 1.0);
    EXPECT_LT(records[3].get_value(), 2.0);
    EXPECT_GT(records[7].get_value(), 1.5);
    EXPECT_LT(records[7].get_value(), 2.5);
}

TEST_F(KnnImputerTest, ChangeKValue) {
    KnnImputer imputer(3);
    
    EXPECT_EQ(imputer.get_k(), 3);
    
    imputer.set_k(5);
    EXPECT_EQ(imputer.get_k(), 5);
}

TEST_F(KnnImputerTest, InvalidKValue) {
    EXPECT_THROW(KnnImputer imputer(0), std::invalid_argument);
    EXPECT_THROW(KnnImputer imputer(-1), std::invalid_argument);
    
    KnnImputer imputer(3);
    EXPECT_THROW(imputer.set_k(0), std::invalid_argument);
}

TEST_F(KnnImputerTest, InsufficientDataForKNN) {
    KnnImputer imputer(5);
    
    std::vector<NavRecord> records;
    for (int i = 0; i < 3; ++i) {
        NavRecord record;
        record.fund_id = "000001";
        record.date = string_to_date("2024-01-0" + std::to_string(i + 1));
        record.nav = 1.0;
        records.push_back(record);
    }
    records[1].nav = std::nullopt;
    
    int filled = imputer.impute(records);
    
    EXPECT_EQ(filled, 0);  // Not enough data points for k=5
}

TEST_F(KnnImputerTest, NoMissingValues) {
    KnnImputer imputer(3);
    
    std::vector<NavRecord> records;
    for (int i = 0; i < 10; ++i) {
        NavRecord record;
        record.fund_id = "000001";
        record.date = string_to_date("2024-01-" + std::to_string(i + 1));
        record.nav = 1.0 + i * 0.1;
        records.push_back(record);
    }
    
    int filled = imputer.impute(records);
    
    EXPECT_EQ(filled, 0);
}

TEST_F(KnnImputerTest, AllMissingValues) {
    KnnImputer imputer(3);
    
    std::vector<NavRecord> records;
    for (int i = 0; i < 10; ++i) {
        NavRecord record;
        record.fund_id = "000001";
        record.date = string_to_date("2024-01-" + std::to_string(i + 1));
        record.nav = std::nullopt;
        records.push_back(record);
    }
    
    int filled = imputer.impute(records);
    
    EXPECT_EQ(filled, 0);  // No valid values to use as neighbors
}

TEST_F(KnnImputerTest, ImputationAccuracy) {
    KnnImputer imputer(3);
    
    std::vector<NavRecord> records;
    // Create linear pattern: 1.0, 2.0, 3.0, missing, 5.0
    for (int i = 0; i < 5; ++i) {
        NavRecord record;
        record.fund_id = "000001";
        record.date = string_to_date("2024-01-0" + std::to_string(i + 1));
        record.nav = (i == 3) ? std::nullopt : static_cast<double>(i + 1);
        records.push_back(record);
    }
    
    int filled = imputer.impute(records);
    
    EXPECT_EQ(filled, 1);
    EXPECT_TRUE(records[3].has_value());
    
    // Should be close to 4.0 (the expected value in linear pattern)
    EXPECT_NEAR(records[3].get_value(), 4.0, 0.5);
}
