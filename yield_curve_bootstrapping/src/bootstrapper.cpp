#include "bootstrapper.hpp"
#include "discount_factor.hpp"
#include <algorithm>
#include <stdexcept>
#include <cmath>

namespace yield_curve {

Bootstrapper::Bootstrapper(CompoundingType type, InterpolationType interp_type)
    : compounding_type_(type), interpolation_type_(interp_type) {}

YieldCurve Bootstrapper::bootstrap(const std::vector<BondData>& bonds) {
    if (!validate_bonds(bonds)) {
        throw std::invalid_argument("Invalid bond data");
    }
    
    auto sorted_bonds = sort_bonds_by_maturity(bonds);
    
    YieldCurve curve(compounding_type_, interpolation_type_);
    
    for (const auto& bond : sorted_bonds) {
        double df = solve_for_discount_factor(bond, curve);
        curve.add_point(bond.maturity, df);
    }
    
    return curve;
}

YieldCurve Bootstrapper::bootstrap_with_spline(const std::vector<BondData>& bonds) {
    YieldCurve curve = bootstrap(bonds);
    curve.apply_cubic_spline_smoothing();
    return curve;
}

double Bootstrapper::solve_for_discount_factor(
    const BondData& bond,
    const YieldCurve& partial_curve
) {
    std::vector<double> payment_times = bond.get_payment_times();
    std::vector<double> cash_flows = bond.get_cash_flows();
    
    double pv_known = 0.0;
    
    for (size_t i = 0; i < payment_times.size() - 1; ++i) {
        double df = partial_curve.get_discount_factor(payment_times[i]);
        pv_known += cash_flows[i] * df;
    }
    
    double final_cash_flow = cash_flows.back();
    double df_final = (bond.market_price - pv_known) / final_cash_flow;
    
    if (df_final <= 0 || df_final > 1.0) {
        throw std::runtime_error("Calculated discount factor out of valid range - possible arbitrage");
    }
    
    return df_final;
}

bool Bootstrapper::validate_bonds(const std::vector<BondData>& bonds) const {
    if (bonds.empty()) {
        return false;
    }
    
    for (const auto& bond : bonds) {
        if (bond.maturity <= 0) {
            return false;
        }
        if (bond.coupon_rate < 0) {
            return false;
        }
        if (bond.payment_frequency <= 0) {
            return false;
        }
        if (bond.market_price <= 0) {
            return false;
        }
        if (bond.face_value <= 0) {
            return false;
        }
        
        double intrinsic = bond.face_value;
        if (bond.market_price > intrinsic * 2.0) {
            return false;
        }
    }
    
    return true;
}

std::vector<BondData> Bootstrapper::sort_bonds_by_maturity(std::vector<BondData> bonds) const {
    std::sort(bonds.begin(), bonds.end(),
        [](const BondData& a, const BondData& b) {
            return a.maturity < b.maturity;
        });
    return bonds;
}

}
