#include "bond_types.hpp"
#include <sstream>

namespace yield_curve {

std::string compounding_type_string(CompoundingType type) {
    switch (type) {
        case CompoundingType::CONTINUOUS:
            return "Continuous";
        case CompoundingType::ANNUAL:
            return "Annual";
        case CompoundingType::SEMI_ANNUAL:
            return "Semi-Annual";
        case CompoundingType::QUARTERLY:
            return "Quarterly";
        default:
            return "Unknown";
    }
}

std::vector<double> BondData::get_payment_times() const {
    std::vector<double> times;
    double dt = 1.0 / payment_frequency;
    
    for (double t = dt; t <= maturity + 1e-10; t += dt) {
        times.push_back(t);
    }
    
    return times;
}

std::vector<double> BondData::get_cash_flows() const {
    std::vector<double> cash_flows;
    std::vector<double> times = get_payment_times();
    
    double coupon_payment = coupon_rate * face_value / payment_frequency;
    
    for (size_t i = 0; i < times.size(); ++i) {
        if (i == times.size() - 1) {
            cash_flows.push_back(coupon_payment + face_value);
        } else {
            cash_flows.push_back(coupon_payment);
        }
    }
    
    return cash_flows;
}

}
