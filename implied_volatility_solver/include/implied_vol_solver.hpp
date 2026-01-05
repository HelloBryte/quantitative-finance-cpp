#pragma once

#include "option_types.hpp"
#include "black_scholes.hpp"
#include <vector>

namespace implied_vol {

class ImpliedVolSolver {
public:
    ImpliedVolSolver();
    
    ImpliedVolResult solve_newton_raphson(
        const OptionSpec& spec,
        double market_price,
        double initial_guess = 0.2,
        double tolerance = 1e-6,
        int max_iterations = 100
    );
    
    ImpliedVolResult solve_brent(
        const OptionSpec& spec,
        double market_price,
        double vol_low = 0.01,
        double vol_high = 5.0,
        double tolerance = 1e-6,
        int max_iterations = 100
    );
    
    ImpliedVolResult solve_with_fallback(
        const OptionSpec& spec,
        double market_price
    );
    
    VolSmile compute_vol_smile(
        double spot,
        const std::vector<double>& strikes,
        const std::vector<double>& market_prices,
        double time_to_expiry,
        double risk_free_rate,
        OptionType type = OptionType::CALL
    );
    
private:
    BlackScholesEngine bs_engine_;
    
    static constexpr double VEGA_MIN_THRESHOLD = 1e-10;
    static constexpr double VOL_MIN = 0.001;
    static constexpr double VOL_MAX = 10.0;
    
    bool validate_inputs(const OptionSpec& spec, double market_price) const;
    
    double get_initial_guess(const OptionSpec& spec, double market_price) const;
    
    double clamp_volatility(double vol) const;
};

}
