#include <gtest/gtest.h>
#include "interpolation.hpp"

using namespace fund_nav_cleaner;

class InterpolationTest : public ::testing::Test {
protected:
    std::vector<NavRecord> create_test_records(const std::vector<std::optional<double>>& values) {
        std::vector<NavRecord> records;
        for (size_t i = 0; i < values.size(); ++i) {
            NavRecord record;
            record.fund_id = "000001";
            record.date = string_to_date("2024-01-" + std::to_string(i + 1));
            record.nav = values[i];
            record.accumulated_nav = 1.0;
            records.push_back(record);
        }
        return records;
    }
};

TEST_F(InterpolationTest, LinearInterpolation) {
    // Values: 1.0, missing, missing, 4.0
    auto records = create_test_records({1.0, std::nullopt, std::nullopt, 4.0});
    
    int filled = Interpolation::linear_interpolate(records);
    
    EXPECT_EQ(filled, 2);
    EXPECT_TRUE(records[1].has_value());
    EXPECT_TRUE(records[2].has_value());
    EXPECT_NEAR(records[1].get_value(), 2.0, 0.01);
    EXPECT_NEAR(records[2].get_value(), 3.0, 0.01);
}

TEST_F(InterpolationTest, LinearInterpolationSingleGap) {
    // Values: 1.0, missing, 3.0
    auto records = create_test_records({1.0, std::nullopt, 3.0});
    
    int filled = Interpolation::linear_interpolate(records);
    
    EXPECT_EQ(filled, 1);
    EXPECT_TRUE(records[1].has_value());
    EXPECT_NEAR(records[1].get_value(), 2.0, 0.01);
}

TEST_F(InterpolationTest, ForwardFill) {
    // Values: 1.0, missing, missing, 2.0
    auto records = create_test_records({1.0, std::nullopt, std::nullopt, 2.0});
    
    int filled = Interpolation::forward_fill(records);
    
    EXPECT_EQ(filled, 2);
    EXPECT_NEAR(records[1].get_value(), 1.0, 0.01);
    EXPECT_NEAR(records[2].get_value(), 1.0, 0.01);
}

TEST_F(InterpolationTest, BackwardFill) {
    // Values: missing, missing, 3.0, 4.0
    auto records = create_test_records({std::nullopt, std::nullopt, 3.0, 4.0});
    
    int filled = Interpolation::backward_fill(records);
    
    EXPECT_EQ(filled, 2);
    EXPECT_NEAR(records[0].get_value(), 3.0, 0.01);
    EXPECT_NEAR(records[1].get_value(), 3.0, 0.01);
}

TEST_F(InterpolationTest, MedianFill) {
    // Values: 1.0, 2.0, missing, 4.0, 5.0
    auto records = create_test_records({1.0, 2.0, std::nullopt, 4.0, 5.0});
    
    int filled = Interpolation::median_fill(records, 5);
    
    EXPECT_EQ(filled, 1);
    EXPECT_TRUE(records[2].has_value());
    // Median of [1.0, 2.0, 4.0, 5.0] = 3.0
    EXPECT_NEAR(records[2].get_value(), 3.0, 0.01);
}

TEST_F(InterpolationTest, NoMissingValues) {
    auto records = create_test_records({1.0, 2.0, 3.0, 4.0});
    
    int filled = Interpolation::linear_interpolate(records);
    
    EXPECT_EQ(filled, 0);
}

TEST_F(InterpolationTest, AllMissingValues) {
    auto records = create_test_records({std::nullopt, std::nullopt, std::nullopt});
    
    int filled = Interpolation::linear_interpolate(records);
    
    EXPECT_EQ(filled, 0);  // Cannot interpolate without any valid values
}

TEST_F(InterpolationTest, MissingAtEnds) {
    // Values: missing, 2.0, 3.0, missing
    auto records = create_test_records({std::nullopt, 2.0, 3.0, std::nullopt});
    
    int filled = Interpolation::linear_interpolate(records);
    
    EXPECT_EQ(filled, 0);  // Cannot interpolate at boundaries
    EXPECT_FALSE(records[0].has_value());
    EXPECT_FALSE(records[3].has_value());
}

TEST_F(InterpolationTest, TimeWeightedInterpolation) {
    std::vector<NavRecord> records;
    
    NavRecord r1;
    r1.fund_id = "000001";
    r1.date = string_to_date("2024-01-01");
    r1.nav = 1.0;
    records.push_back(r1);
    
    NavRecord r2;
    r2.fund_id = "000001";
    r2.date = string_to_date("2024-01-05");  // 4 days later
    r2.nav = std::nullopt;
    records.push_back(r2);
    
    NavRecord r3;
    r3.fund_id = "000001";
    r3.date = string_to_date("2024-01-11");  // 6 days later (total 10 days)
    r3.nav = 6.0;
    records.push_back(r3);
    
    int filled = Interpolation::time_weighted_interpolate(records);
    
    EXPECT_EQ(filled, 1);
    EXPECT_TRUE(records[1].has_value());
    // Time-weighted: 1.0 + (6.0 - 1.0) * 4/10 = 1.0 + 2.0 = 3.0
    EXPECT_NEAR(records[1].get_value(), 3.0, 0.1);
}
