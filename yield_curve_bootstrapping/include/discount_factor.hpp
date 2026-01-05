#pragma once

#include "bond_types.hpp"

namespace yield_curve {

class DiscountFactor {
public:
    static double from_zero_rate(double time, double zero_rate, CompoundingType type);
    
    static double to_zero_rate(double time, double discount_factor, CompoundingType type);
    
    static bool is_valid(double discount_factor);
    
private:
    static constexpr double MIN_DF = 1e-10;
    static constexpr double MAX_DF = 1.0;
};

}
