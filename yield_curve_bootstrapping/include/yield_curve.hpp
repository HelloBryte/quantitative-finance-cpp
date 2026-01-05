#pragma once

#include "bond_types.hpp"
#include "interpolation.hpp"
#include "cubic_spline.hpp"
#include <vector>
#include <memory>

namespace yield_curve {

class YieldCurve {
public:
    YieldCurve(CompoundingType type, InterpolationType interp_type);
    
    void add_point(double time, double discount_factor);
    
    double get_discount_factor(double time) const;
    
    double get_zero_rate(double time) const;
    
    double get_forward_rate(double t1, double t2) const;
    
    double get_instantaneous_forward(double t, double dt = 1e-6) const;
    
    const std::vector<double>& times() const { return times_; }
    const std::vector<double>& discount_factors() const { return discount_factors_; }
    
    size_t size() const { return times_.size(); }
    
    void apply_cubic_spline_smoothing();
    
    bool has_arbitrage() const;
    
    CompoundingType compounding_type() const { return compounding_type_; }
    
private:
    std::vector<double> times_;
    std::vector<double> discount_factors_;
    CompoundingType compounding_type_;
    std::unique_ptr<Interpolator> interpolator_;
    std::unique_ptr<CubicSpline> spline_;
    bool use_spline_ = false;
};

}
