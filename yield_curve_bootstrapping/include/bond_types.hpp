#pragma once

#include <string>
#include <vector>

namespace yield_curve {

enum class CompoundingType {
    CONTINUOUS,
    ANNUAL,
    SEMI_ANNUAL,
    QUARTERLY
};

std::string compounding_type_string(CompoundingType type);

struct BondData {
    double maturity;
    double coupon_rate;
    int payment_frequency;
    double market_price;
    double face_value;
    
    BondData(double mat, double coupon, int freq, double price, double fv = 100.0)
        : maturity(mat), coupon_rate(coupon), payment_frequency(freq),
          market_price(price), face_value(fv) {}
    
    std::vector<double> get_payment_times() const;
    std::vector<double> get_cash_flows() const;
};

struct CurvePoint {
    double time;
    double discount_factor;
    
    CurvePoint(double t, double df) : time(t), discount_factor(df) {}
};

}
