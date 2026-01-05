#pragma once

#include "option_types.hpp"

namespace implied_vol {

class BlackScholesEngine {
public:
    double price(const OptionSpec& spec, double volatility) const;
    
    double vega(const OptionSpec& spec, double volatility) const;
    
    double delta(const OptionSpec& spec, double volatility) const;
    
    double gamma(const OptionSpec& spec, double volatility) const;
    
    double theta(const OptionSpec& spec, double volatility) const;
    
    double rho(const OptionSpec& spec, double volatility) const;
    
    double intrinsic_value(const OptionSpec& spec) const;
    
    bool verify_put_call_parity(
        double call_price,
        double put_price,
        const OptionSpec& spec,
        double tolerance = 1e-4
    ) const;
    
private:
    struct D1D2 {
        double d1;
        double d2;
    };
    
    D1D2 calculate_d1_d2(const OptionSpec& spec, double volatility) const;
};

}
