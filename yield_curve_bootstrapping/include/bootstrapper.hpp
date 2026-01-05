#pragma once

#include "bond_types.hpp"
#include "yield_curve.hpp"
#include <vector>

namespace yield_curve {

class Bootstrapper {
public:
    Bootstrapper(CompoundingType type, InterpolationType interp_type);
    
    YieldCurve bootstrap(const std::vector<BondData>& bonds);
    
    YieldCurve bootstrap_with_spline(const std::vector<BondData>& bonds);
    
private:
    CompoundingType compounding_type_;
    InterpolationType interpolation_type_;
    
    double solve_for_discount_factor(
        const BondData& bond,
        const YieldCurve& partial_curve
    );
    
    bool validate_bonds(const std::vector<BondData>& bonds) const;
    
    std::vector<BondData> sort_bonds_by_maturity(std::vector<BondData> bonds) const;
};

}
