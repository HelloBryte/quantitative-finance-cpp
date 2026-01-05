#include "discount_factor.hpp"
#include <cmath>
#include <stdexcept>

namespace yield_curve {

double DiscountFactor::from_zero_rate(double time, double zero_rate, CompoundingType type) {
    if (time < 0) {
        throw std::invalid_argument("Time must be non-negative");
    }
    
    if (time < 1e-10) {
        return 1.0;
    }
    
    switch (type) {
        case CompoundingType::CONTINUOUS:
            return std::exp(-zero_rate * time);
            
        case CompoundingType::ANNUAL:
            return 1.0 / std::pow(1.0 + zero_rate, time);
            
        case CompoundingType::SEMI_ANNUAL:
            return 1.0 / std::pow(1.0 + zero_rate / 2.0, 2.0 * time);
            
        case CompoundingType::QUARTERLY:
            return 1.0 / std::pow(1.0 + zero_rate / 4.0, 4.0 * time);
            
        default:
            throw std::invalid_argument("Unknown compounding type");
    }
}

double DiscountFactor::to_zero_rate(double time, double discount_factor, CompoundingType type) {
    if (time < 1e-10) {
        throw std::invalid_argument("Time too small for rate calculation");
    }
    
    if (discount_factor <= 0 || discount_factor > 1.0) {
        throw std::invalid_argument("Invalid discount factor");
    }
    
    switch (type) {
        case CompoundingType::CONTINUOUS:
            return -std::log(discount_factor) / time;
            
        case CompoundingType::ANNUAL:
            return std::pow(discount_factor, -1.0 / time) - 1.0;
            
        case CompoundingType::SEMI_ANNUAL:
            return 2.0 * (std::pow(discount_factor, -1.0 / (2.0 * time)) - 1.0);
            
        case CompoundingType::QUARTERLY:
            return 4.0 * (std::pow(discount_factor, -1.0 / (4.0 * time)) - 1.0);
            
        default:
            throw std::invalid_argument("Unknown compounding type");
    }
}

bool DiscountFactor::is_valid(double discount_factor) {
    return discount_factor > MIN_DF && discount_factor <= MAX_DF;
}

}
