#include <gtest/gtest.h>
#include "interpolation.hpp"
#include <cmath>

using namespace yield_curve;

TEST(InterpolationTest, LinearInterpolation) {
    std::vector<double> times = {1.0, 2.0, 3.0};
    std::vector<double> dfs = {0.95, 0.90, 0.85};
    
    LinearInterpolator interp;
    double df = interp.interpolate(1.5, times, dfs);
    
    EXPECT_NEAR(df, 0.925, 1e-10);
}

TEST(InterpolationTest, LogLinearInterpolation) {
    std::vector<double> times = {1.0, 2.0};
    std::vector<double> dfs = {0.95, 0.90};
    
    LogLinearInterpolator interp;
    double df = interp.interpolate(1.5, times, dfs);
    
    double log_df = 0.5 * (std::log(0.95) + std::log(0.90));
    EXPECT_NEAR(df, std::exp(log_df), 1e-10);
}

TEST(InterpolationTest, FlatForwardInterpolation) {
    std::vector<double> times = {1.0, 2.0};
    std::vector<double> dfs = {0.95, 0.90};
    
    FlatForwardInterpolator interp;
    double df = interp.interpolate(1.5, times, dfs);
    
    EXPECT_GT(df, 0.90);
    EXPECT_LT(df, 0.95);
}

TEST(InterpolationTest, ExtrapolationBounds) {
    std::vector<double> times = {1.0, 2.0, 3.0};
    std::vector<double> dfs = {0.95, 0.90, 0.85};
    
    LinearInterpolator interp;
    
    EXPECT_NEAR(interp.interpolate(0.5, times, dfs), 0.95, 1e-10);
    EXPECT_NEAR(interp.interpolate(3.5, times, dfs), 0.85, 1e-10);
}
