#include <gtest/gtest.h>
#include "cubic_spline.hpp"
#include <vector>

using namespace yield_curve;

TEST(CubicSplineTest, BasicFit) {
    std::vector<double> x = {0.0, 1.0, 2.0, 3.0};
    std::vector<double> y = {0.0, 1.0, 4.0, 9.0};
    
    CubicSpline spline;
    spline.fit(x, y);
    
    EXPECT_TRUE(spline.is_fitted());
    
    EXPECT_NEAR(spline.evaluate(0.0), 0.0, 1e-6);
    EXPECT_NEAR(spline.evaluate(1.0), 1.0, 1e-6);
    EXPECT_NEAR(spline.evaluate(2.0), 4.0, 1e-6);
    EXPECT_NEAR(spline.evaluate(3.0), 9.0, 1e-6);
}

TEST(CubicSplineTest, InterpolatedValues) {
    std::vector<double> x = {0.0, 1.0, 2.0};
    std::vector<double> y = {0.0, 1.0, 0.0};
    
    CubicSpline spline;
    spline.fit(x, y);
    
    double val = spline.evaluate(0.5);
    EXPECT_GT(val, 0.0);
    EXPECT_LT(val, 1.0);
}

TEST(CubicSplineTest, Smoothness) {
    std::vector<double> x = {0.0, 1.0, 2.0, 3.0};
    std::vector<double> y = {0.0, 1.0, 0.5, 2.0};
    
    CubicSpline spline;
    spline.fit(x, y);
    
    double deriv1 = spline.derivative(1.5);
    double deriv2 = spline.derivative(1.50001);
    
    EXPECT_NEAR(deriv1, deriv2, 1e-3);
}
