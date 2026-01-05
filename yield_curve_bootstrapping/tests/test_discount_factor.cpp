#include <gtest/gtest.h>
#include "discount_factor.hpp"
#include <cmath>

using namespace yield_curve;

TEST(DiscountFactorTest, ContinuousCompounding) {
    double time = 1.0;
    double rate = 0.05;
    
    double df = DiscountFactor::from_zero_rate(time, rate, CompoundingType::CONTINUOUS);
    EXPECT_NEAR(df, std::exp(-0.05), 1e-10);
    
    double recovered_rate = DiscountFactor::to_zero_rate(time, df, CompoundingType::CONTINUOUS);
    EXPECT_NEAR(recovered_rate, rate, 1e-10);
}

TEST(DiscountFactorTest, AnnualCompounding) {
    double time = 2.0;
    double rate = 0.05;
    
    double df = DiscountFactor::from_zero_rate(time, rate, CompoundingType::ANNUAL);
    EXPECT_NEAR(df, 1.0 / std::pow(1.05, 2.0), 1e-10);
    
    double recovered_rate = DiscountFactor::to_zero_rate(time, df, CompoundingType::ANNUAL);
    EXPECT_NEAR(recovered_rate, rate, 1e-10);
}

TEST(DiscountFactorTest, SemiAnnualCompounding) {
    double time = 1.0;
    double rate = 0.06;
    
    double df = DiscountFactor::from_zero_rate(time, rate, CompoundingType::SEMI_ANNUAL);
    EXPECT_NEAR(df, 1.0 / std::pow(1.03, 2.0), 1e-10);
}

TEST(DiscountFactorTest, ZeroTime) {
    double df = DiscountFactor::from_zero_rate(0.0, 0.05, CompoundingType::CONTINUOUS);
    EXPECT_NEAR(df, 1.0, 1e-10);
}

TEST(DiscountFactorTest, InvalidDiscountFactor) {
    EXPECT_FALSE(DiscountFactor::is_valid(0.0));
    EXPECT_FALSE(DiscountFactor::is_valid(-0.1));
    EXPECT_FALSE(DiscountFactor::is_valid(1.1));
    EXPECT_TRUE(DiscountFactor::is_valid(0.95));
}
