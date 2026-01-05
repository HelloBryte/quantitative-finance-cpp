#include <gtest/gtest.h>
#include "normal_distribution.hpp"
#include <cmath>

using namespace implied_vol;

TEST(NormalDistributionTest, PDFAtZero) {
    double result = NormalDistribution::pdf(0.0);
    double expected = 1.0 / std::sqrt(2.0 * M_PI);
    EXPECT_NEAR(result, expected, 1e-10);
}

TEST(NormalDistributionTest, PDFSymmetry) {
    double x = 1.5;
    EXPECT_NEAR(NormalDistribution::pdf(x), NormalDistribution::pdf(-x), 1e-10);
}

TEST(NormalDistributionTest, CDFAtZero) {
    double result = NormalDistribution::cdf(0.0);
    EXPECT_NEAR(result, 0.5, 1e-6);
}

TEST(NormalDistributionTest, CDFSymmetry) {
    double x = 1.0;
    double cdf_pos = NormalDistribution::cdf(x);
    double cdf_neg = NormalDistribution::cdf(-x);
    EXPECT_NEAR(cdf_pos + cdf_neg, 1.0, 1e-6);
}

TEST(NormalDistributionTest, CDFKnownValues) {
    EXPECT_NEAR(NormalDistribution::cdf(1.0), 0.8413447, 1e-6);
    EXPECT_NEAR(NormalDistribution::cdf(2.0), 0.9772499, 1e-6);
    EXPECT_NEAR(NormalDistribution::cdf(-1.0), 0.1586553, 1e-6);
}

TEST(NormalDistributionTest, CDFBounds) {
    EXPECT_NEAR(NormalDistribution::cdf(-5.0), 0.0, 1e-6);
    EXPECT_NEAR(NormalDistribution::cdf(5.0), 1.0, 1e-6);
}
