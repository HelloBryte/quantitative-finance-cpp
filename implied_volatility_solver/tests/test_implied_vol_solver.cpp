#include <gtest/gtest.h>
#include "implied_vol_solver.hpp"
#include <cmath>

using namespace implied_vol;

class ImpliedVolSolverTest : public ::testing::Test {
protected:
    ImpliedVolSolver solver;
    BlackScholesEngine engine;
};

TEST_F(ImpliedVolSolverTest, RecoverKnownVolatilityATM) {
    double S = 100.0, K = 100.0, T = 1.0, r = 0.05;
    double true_vol = 0.25;
    
    OptionSpec spec(S, K, T, r, OptionType::CALL);
    double market_price = engine.price(spec, true_vol);
    
    ImpliedVolResult result = solver.solve_newton_raphson(spec, market_price);
    
    EXPECT_TRUE(result.is_success());
    EXPECT_NEAR(result.implied_vol, true_vol, 1e-6);
    EXPECT_LT(result.iterations, 10);
}

TEST_F(ImpliedVolSolverTest, RecoverKnownVolatilityITM) {
    double S = 110.0, K = 100.0, T = 1.0, r = 0.05;
    double true_vol = 0.30;
    
    OptionSpec spec(S, K, T, r, OptionType::CALL);
    double market_price = engine.price(spec, true_vol);
    
    ImpliedVolResult result = solver.solve_newton_raphson(spec, market_price);
    
    EXPECT_TRUE(result.is_success());
    EXPECT_NEAR(result.implied_vol, true_vol, 1e-5);
}

TEST_F(ImpliedVolSolverTest, RecoverKnownVolatilityOTM) {
    double S = 90.0, K = 100.0, T = 1.0, r = 0.05;
    double true_vol = 0.20;
    
    OptionSpec spec(S, K, T, r, OptionType::CALL);
    double market_price = engine.price(spec, true_vol);
    
    ImpliedVolResult result = solver.solve_newton_raphson(spec, market_price);
    
    EXPECT_TRUE(result.is_success());
    EXPECT_NEAR(result.implied_vol, true_vol, 1e-5);
}

TEST_F(ImpliedVolSolverTest, PutOption) {
    double S = 100.0, K = 100.0, T = 1.0, r = 0.05;
    double true_vol = 0.25;
    
    OptionSpec spec(S, K, T, r, OptionType::PUT);
    double market_price = engine.price(spec, true_vol);
    
    ImpliedVolResult result = solver.solve_newton_raphson(spec, market_price);
    
    EXPECT_TRUE(result.is_success());
    EXPECT_NEAR(result.implied_vol, true_vol, 1e-6);
}

TEST_F(ImpliedVolSolverTest, BrentMethodConverges) {
    double S = 100.0, K = 100.0, T = 1.0, r = 0.05;
    double true_vol = 0.25;
    
    OptionSpec spec(S, K, T, r, OptionType::CALL);
    double market_price = engine.price(spec, true_vol);
    
    ImpliedVolResult result = solver.solve_brent(spec, market_price);
    
    EXPECT_TRUE(result.is_success());
    EXPECT_NEAR(result.implied_vol, true_vol, 1e-6);
}

TEST_F(ImpliedVolSolverTest, FallbackMethod) {
    double S = 100.0, K = 100.0, T = 1.0, r = 0.05;
    double true_vol = 0.25;
    
    OptionSpec spec(S, K, T, r, OptionType::CALL);
    double market_price = engine.price(spec, true_vol);
    
    ImpliedVolResult result = solver.solve_with_fallback(spec, market_price);
    
    EXPECT_TRUE(result.is_success());
    EXPECT_NEAR(result.implied_vol, true_vol, 1e-6);
}

TEST_F(ImpliedVolSolverTest, InvalidInputNegativePrice) {
    OptionSpec spec(100.0, 100.0, 1.0, 0.05, OptionType::CALL);
    ImpliedVolResult result = solver.solve_newton_raphson(spec, -5.0);
    
    EXPECT_EQ(result.status, ConvergenceStatus::INVALID_INPUT);
}

TEST_F(ImpliedVolSolverTest, InvalidInputBelowIntrinsic) {
    OptionSpec spec(110.0, 100.0, 1.0, 0.05, OptionType::CALL);
    ImpliedVolResult result = solver.solve_newton_raphson(spec, 5.0);
    
    EXPECT_EQ(result.status, ConvergenceStatus::INVALID_INPUT);
}

TEST_F(ImpliedVolSolverTest, VolSmileCalculation) {
    double S = 100.0, T = 1.0, r = 0.05;
    std::vector<double> strikes = {80.0, 90.0, 100.0, 110.0, 120.0};
    std::vector<double> vols = {0.25, 0.22, 0.20, 0.22, 0.25};
    
    std::vector<double> market_prices;
    for (size_t i = 0; i < strikes.size(); ++i) {
        OptionSpec spec(S, strikes[i], T, r, OptionType::CALL);
        market_prices.push_back(engine.price(spec, vols[i]));
    }
    
    VolSmile smile = solver.compute_vol_smile(S, strikes, market_prices, T, r);
    
    EXPECT_EQ(smile.strikes.size(), strikes.size());
    EXPECT_EQ(smile.implied_vols.size(), strikes.size());
    
    for (size_t i = 0; i < strikes.size(); ++i) {
        EXPECT_NEAR(smile.implied_vols[i], vols[i], 1e-5);
    }
}
