#include <gtest/gtest.h>
#include "black_scholes.hpp"
#include <cmath>

using namespace implied_vol;

class BlackScholesTest : public ::testing::Test {
protected:
    BlackScholesEngine engine;
};

TEST_F(BlackScholesTest, CallPriceATM) {
    OptionSpec spec(100.0, 100.0, 1.0, 0.05, OptionType::CALL);
    double price = engine.price(spec, 0.2);
    EXPECT_GT(price, 0.0);
    EXPECT_LT(price, 100.0);
}

TEST_F(BlackScholesTest, PutPriceATM) {
    OptionSpec spec(100.0, 100.0, 1.0, 0.05, OptionType::PUT);
    double price = engine.price(spec, 0.2);
    EXPECT_GT(price, 0.0);
    EXPECT_LT(price, 100.0);
}

TEST_F(BlackScholesTest, PutCallParity) {
    double S = 100.0, K = 100.0, T = 1.0, r = 0.05, sigma = 0.2;
    
    OptionSpec call_spec(S, K, T, r, OptionType::CALL);
    OptionSpec put_spec(S, K, T, r, OptionType::PUT);
    
    double call_price = engine.price(call_spec, sigma);
    double put_price = engine.price(put_spec, sigma);
    
    EXPECT_TRUE(engine.verify_put_call_parity(call_price, put_price, call_spec));
}

TEST_F(BlackScholesTest, VegaPositive) {
    OptionSpec spec(100.0, 100.0, 1.0, 0.05, OptionType::CALL);
    double vega = engine.vega(spec, 0.2);
    EXPECT_GT(vega, 0.0);
}

TEST_F(BlackScholesTest, VegaMaximizedATM) {
    double S = 100.0, T = 1.0, r = 0.05, sigma = 0.2;
    
    OptionSpec atm(S, 100.0, T, r, OptionType::CALL);
    OptionSpec itm(S, 80.0, T, r, OptionType::CALL);
    OptionSpec otm(S, 120.0, T, r, OptionType::CALL);
    
    double vega_atm = engine.vega(atm, sigma);
    double vega_itm = engine.vega(itm, sigma);
    double vega_otm = engine.vega(otm, sigma);
    
    EXPECT_GT(vega_atm, vega_itm);
    EXPECT_GT(vega_atm, vega_otm);
}

TEST_F(BlackScholesTest, DeltaCallBounds) {
    OptionSpec spec(100.0, 100.0, 1.0, 0.05, OptionType::CALL);
    double delta = engine.delta(spec, 0.2);
    EXPECT_GT(delta, 0.0);
    EXPECT_LT(delta, 1.0);
}

TEST_F(BlackScholesTest, DeltaPutBounds) {
    OptionSpec spec(100.0, 100.0, 1.0, 0.05, OptionType::PUT);
    double delta = engine.delta(spec, 0.2);
    EXPECT_GT(delta, -1.0);
    EXPECT_LT(delta, 0.0);
}

TEST_F(BlackScholesTest, GammaPositive) {
    OptionSpec spec(100.0, 100.0, 1.0, 0.05, OptionType::CALL);
    double gamma = engine.gamma(spec, 0.2);
    EXPECT_GT(gamma, 0.0);
}

TEST_F(BlackScholesTest, IntrinsicValueCall) {
    OptionSpec itm_call(110.0, 100.0, 1.0, 0.05, OptionType::CALL);
    EXPECT_NEAR(engine.intrinsic_value(itm_call), 10.0, 1e-10);
    
    OptionSpec otm_call(90.0, 100.0, 1.0, 0.05, OptionType::CALL);
    EXPECT_NEAR(engine.intrinsic_value(otm_call), 0.0, 1e-10);
}

TEST_F(BlackScholesTest, IntrinsicValuePut) {
    OptionSpec itm_put(90.0, 100.0, 1.0, 0.05, OptionType::PUT);
    EXPECT_NEAR(engine.intrinsic_value(itm_put), 10.0, 1e-10);
    
    OptionSpec otm_put(110.0, 100.0, 1.0, 0.05, OptionType::PUT);
    EXPECT_NEAR(engine.intrinsic_value(otm_put), 0.0, 1e-10);
}
