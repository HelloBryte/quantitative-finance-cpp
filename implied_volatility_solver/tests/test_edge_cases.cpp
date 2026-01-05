#include <gtest/gtest.h>
#include "implied_vol_solver.hpp"
#include <cmath>

using namespace implied_vol;

class EdgeCasesTest : public ::testing::Test {
protected:
    ImpliedVolSolver solver;
    BlackScholesEngine engine;
};

TEST_F(EdgeCasesTest, NearExpiryOption) {
    double S = 100.0, K = 100.0, T = 1.0 / 365.0, r = 0.05;
    double true_vol = 0.25;
    
    OptionSpec spec(S, K, T, r, OptionType::CALL);
    double market_price = engine.price(spec, true_vol);
    
    ImpliedVolResult result = solver.solve_with_fallback(spec, market_price);
    
    EXPECT_TRUE(result.is_success());
    EXPECT_NEAR(result.implied_vol, true_vol, 1e-4);
}

TEST_F(EdgeCasesTest, HighVolatility) {
    double S = 100.0, K = 100.0, T = 1.0, r = 0.05;
    double true_vol = 1.5;
    
    OptionSpec spec(S, K, T, r, OptionType::CALL);
    double market_price = engine.price(spec, true_vol);
    
    ImpliedVolResult result = solver.solve_with_fallback(spec, market_price);
    
    EXPECT_TRUE(result.is_success());
    EXPECT_NEAR(result.implied_vol, true_vol, 1e-4);
}

TEST_F(EdgeCasesTest, LowVolatility) {
    double S = 100.0, K = 100.0, T = 1.0, r = 0.05;
    double true_vol = 0.05;
    
    OptionSpec spec(S, K, T, r, OptionType::CALL);
    double market_price = engine.price(spec, true_vol);
    
    ImpliedVolResult result = solver.solve_with_fallback(spec, market_price);
    
    EXPECT_TRUE(result.is_success());
    EXPECT_NEAR(result.implied_vol, true_vol, 1e-5);
}

TEST_F(EdgeCasesTest, DeepITMCall) {
    double S = 150.0, K = 100.0, T = 1.0, r = 0.05;
    double true_vol = 0.25;
    
    OptionSpec spec(S, K, T, r, OptionType::CALL);
    double market_price = engine.price(spec, true_vol);
    
    ImpliedVolResult result = solver.solve_with_fallback(spec, market_price);
    
    EXPECT_TRUE(result.is_success());
    EXPECT_NEAR(result.implied_vol, true_vol, 1e-3);
}

TEST_F(EdgeCasesTest, DeepOTMCall) {
    double S = 50.0, K = 100.0, T = 1.0, r = 0.05;
    double true_vol = 0.25;
    
    OptionSpec spec(S, K, T, r, OptionType::CALL);
    double market_price = engine.price(spec, true_vol);
    
    if (market_price > 1e-6) {
        ImpliedVolResult result = solver.solve_with_fallback(spec, market_price);
        EXPECT_TRUE(result.is_success() || result.status == ConvergenceStatus::VEGA_TOO_SMALL);
    }
}

TEST_F(EdgeCasesTest, ZeroInterestRate) {
    double S = 100.0, K = 100.0, T = 1.0, r = 0.0;
    double true_vol = 0.25;
    
    OptionSpec spec(S, K, T, r, OptionType::CALL);
    double market_price = engine.price(spec, true_vol);
    
    ImpliedVolResult result = solver.solve_with_fallback(spec, market_price);
    
    EXPECT_TRUE(result.is_success());
    EXPECT_NEAR(result.implied_vol, true_vol, 1e-6);
}

TEST_F(EdgeCasesTest, LongMaturity) {
    double S = 100.0, K = 100.0, T = 5.0, r = 0.05;
    double true_vol = 0.25;
    
    OptionSpec spec(S, K, T, r, OptionType::CALL);
    double market_price = engine.price(spec, true_vol);
    
    ImpliedVolResult result = solver.solve_with_fallback(spec, market_price);
    
    EXPECT_TRUE(result.is_success());
    EXPECT_NEAR(result.implied_vol, true_vol, 1e-6);
}

TEST_F(EdgeCasesTest, PoorInitialGuess) {
    double S = 100.0, K = 100.0, T = 1.0, r = 0.05;
    double true_vol = 0.25;
    
    OptionSpec spec(S, K, T, r, OptionType::CALL);
    double market_price = engine.price(spec, true_vol);
    
    ImpliedVolResult result = solver.solve_newton_raphson(spec, market_price, 0.8);
    
    EXPECT_TRUE(result.is_success());
    EXPECT_NEAR(result.implied_vol, true_vol, 1e-6);
}
