#include <gtest/gtest.h>
#include "bootstrapper.hpp"
#include <vector>

using namespace yield_curve;

TEST(BootstrapperTest, SimpleBonds) {
    std::vector<BondData> bonds = {
        BondData(1.0, 0.00, 1, 95.00),
        BondData(2.0, 0.00, 1, 90.00)
    };
    
    Bootstrapper bootstrapper(CompoundingType::CONTINUOUS, InterpolationType::LINEAR);
    YieldCurve curve = bootstrapper.bootstrap(bonds);
    
    EXPECT_EQ(curve.size(), 2);
    EXPECT_NEAR(curve.get_discount_factor(1.0), 0.95, 1e-6);
    EXPECT_NEAR(curve.get_discount_factor(2.0), 0.90, 1e-6);
}

TEST(BootstrapperTest, CouponBonds) {
    std::vector<BondData> bonds = {
        BondData(1.0, 0.05, 1, 100.00),
        BondData(2.0, 0.05, 1, 100.00)
    };
    
    Bootstrapper bootstrapper(CompoundingType::CONTINUOUS, InterpolationType::LOG_LINEAR);
    YieldCurve curve = bootstrapper.bootstrap(bonds);
    
    EXPECT_EQ(curve.size(), 2);
    EXPECT_GT(curve.get_zero_rate(1.0), 0.0);
    EXPECT_GT(curve.get_zero_rate(2.0), 0.0);
}

TEST(BootstrapperTest, ForwardRatesPositive) {
    std::vector<BondData> bonds = {
        BondData(1.0, 0.02, 1, 99.00),
        BondData(2.0, 0.03, 1, 99.00),
        BondData(3.0, 0.04, 1, 99.50)
    };
    
    Bootstrapper bootstrapper(CompoundingType::CONTINUOUS, InterpolationType::LOG_LINEAR);
    YieldCurve curve = bootstrapper.bootstrap(bonds);
    
    EXPECT_FALSE(curve.has_arbitrage());
    
    for (size_t i = 0; i < curve.size() - 1; ++i) {
        double fwd = curve.get_forward_rate(curve.times()[i], curve.times()[i + 1]);
        EXPECT_GT(fwd, -1e-6);
    }
}

TEST(BootstrapperTest, SplineSmoothing) {
    std::vector<BondData> bonds = {
        BondData(1.0, 0.02, 1, 99.00),
        BondData(2.0, 0.03, 1, 99.00),
        BondData(3.0, 0.04, 1, 99.50)
    };
    
    Bootstrapper bootstrapper(CompoundingType::CONTINUOUS, InterpolationType::LOG_LINEAR);
    YieldCurve curve = bootstrapper.bootstrap_with_spline(bonds);
    
    EXPECT_EQ(curve.size(), 3);
    
    double rate1 = curve.get_zero_rate(1.5);
    double rate2 = curve.get_zero_rate(2.5);
    EXPECT_GT(rate1, 0.0);
    EXPECT_GT(rate2, 0.0);
}
