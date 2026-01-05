#include "black_scholes.hpp"
#include "normal_distribution.hpp"
#include <cmath>
#include <algorithm>

namespace implied_vol {

BlackScholesEngine::D1D2 BlackScholesEngine::calculate_d1_d2(
    const OptionSpec& spec, 
    double volatility
) const {
    double sqrt_T = std::sqrt(spec.time_to_expiry);
    double vol_sqrt_T = volatility * sqrt_T;
    
    double d1 = (std::log(spec.spot / spec.strike) + 
                 (spec.risk_free_rate + 0.5 * volatility * volatility) * spec.time_to_expiry) 
                / vol_sqrt_T;
    double d2 = d1 - vol_sqrt_T;
    
    return {d1, d2};
}

double BlackScholesEngine::price(const OptionSpec& spec, double volatility) const {
    auto [d1, d2] = calculate_d1_d2(spec, volatility);
    
    double discount_factor = std::exp(-spec.risk_free_rate * spec.time_to_expiry);
    
    if (spec.type == OptionType::CALL) {
        return spec.spot * NormalDistribution::cdf(d1) - 
               spec.strike * discount_factor * NormalDistribution::cdf(d2);
    } else {
        return spec.strike * discount_factor * NormalDistribution::cdf(-d2) - 
               spec.spot * NormalDistribution::cdf(-d1);
    }
}

double BlackScholesEngine::vega(const OptionSpec& spec, double volatility) const {
    auto [d1, d2] = calculate_d1_d2(spec, volatility);
    return spec.spot * NormalDistribution::pdf(d1) * std::sqrt(spec.time_to_expiry);
}

double BlackScholesEngine::delta(const OptionSpec& spec, double volatility) const {
    auto [d1, d2] = calculate_d1_d2(spec, volatility);
    
    if (spec.type == OptionType::CALL) {
        return NormalDistribution::cdf(d1);
    } else {
        return NormalDistribution::cdf(d1) - 1.0;
    }
}

double BlackScholesEngine::gamma(const OptionSpec& spec, double volatility) const {
    auto [d1, d2] = calculate_d1_d2(spec, volatility);
    return NormalDistribution::pdf(d1) / (spec.spot * volatility * std::sqrt(spec.time_to_expiry));
}

double BlackScholesEngine::theta(const OptionSpec& spec, double volatility) const {
    auto [d1, d2] = calculate_d1_d2(spec, volatility);
    
    double sqrt_T = std::sqrt(spec.time_to_expiry);
    double discount_factor = std::exp(-spec.risk_free_rate * spec.time_to_expiry);
    
    double term1 = -(spec.spot * NormalDistribution::pdf(d1) * volatility) / (2.0 * sqrt_T);
    
    if (spec.type == OptionType::CALL) {
        double term2 = spec.risk_free_rate * spec.strike * discount_factor * NormalDistribution::cdf(d2);
        return term1 - term2;
    } else {
        double term2 = spec.risk_free_rate * spec.strike * discount_factor * NormalDistribution::cdf(-d2);
        return term1 + term2;
    }
}

double BlackScholesEngine::rho(const OptionSpec& spec, double volatility) const {
    auto [d1, d2] = calculate_d1_d2(spec, volatility);
    
    double discount_factor = std::exp(-spec.risk_free_rate * spec.time_to_expiry);
    
    if (spec.type == OptionType::CALL) {
        return spec.strike * spec.time_to_expiry * discount_factor * NormalDistribution::cdf(d2);
    } else {
        return -spec.strike * spec.time_to_expiry * discount_factor * NormalDistribution::cdf(-d2);
    }
}

double BlackScholesEngine::intrinsic_value(const OptionSpec& spec) const {
    if (spec.type == OptionType::CALL) {
        return std::max(spec.spot - spec.strike, 0.0);
    } else {
        return std::max(spec.strike - spec.spot, 0.0);
    }
}

bool BlackScholesEngine::verify_put_call_parity(
    double call_price,
    double put_price,
    const OptionSpec& spec,
    double tolerance
) const {
    double lhs = call_price - put_price;
    double rhs = spec.spot - spec.strike * std::exp(-spec.risk_free_rate * spec.time_to_expiry);
    return std::abs(lhs - rhs) < tolerance;
}

}
