#include "yield_curve.hpp"
#include "discount_factor.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace yield_curve {

YieldCurve::YieldCurve(CompoundingType type, InterpolationType interp_type)
    : compounding_type_(type), interpolator_(create_interpolator(interp_type)) {}

void YieldCurve::add_point(double time, double discount_factor) {
    if (time < 0) {
        throw std::invalid_argument("Time must be non-negative");
    }
    
    if (!DiscountFactor::is_valid(discount_factor)) {
        throw std::invalid_argument("Invalid discount factor");
    }
    
    times_.push_back(time);
    discount_factors_.push_back(discount_factor);
    
    use_spline_ = false;
}

double YieldCurve::get_discount_factor(double time) const {
    if (times_.empty()) {
        throw std::runtime_error("Curve has no points");
    }
    
    if (time < 0) {
        throw std::invalid_argument("Time must be non-negative");
    }
    
    if (time < 1e-10) {
        return 1.0;
    }
    
    if (use_spline_ && spline_ && spline_->is_fitted()) {
        std::vector<double> zero_rates;
        for (size_t i = 0; i < times_.size(); ++i) {
            zero_rates.push_back(
                DiscountFactor::to_zero_rate(times_[i], discount_factors_[i], compounding_type_)
            );
        }
        
        double rate = spline_->evaluate(time);
        return DiscountFactor::from_zero_rate(time, rate, compounding_type_);
    }
    
    return interpolator_->interpolate(time, times_, discount_factors_);
}

double YieldCurve::get_zero_rate(double time) const {
    double df = get_discount_factor(time);
    return DiscountFactor::to_zero_rate(time, df, compounding_type_);
}

double YieldCurve::get_forward_rate(double t1, double t2) const {
    if (t1 >= t2) {
        throw std::invalid_argument("t1 must be less than t2");
    }
    
    double df1 = get_discount_factor(t1);
    double df2 = get_discount_factor(t2);
    
    return -std::log(df2 / df1) / (t2 - t1);
}

double YieldCurve::get_instantaneous_forward(double t, double dt) const {
    if (dt <= 0) {
        throw std::invalid_argument("dt must be positive");
    }
    
    return get_forward_rate(t, t + dt);
}

void YieldCurve::apply_cubic_spline_smoothing() {
    if (times_.size() < 2) {
        throw std::runtime_error("Need at least 2 points for spline smoothing");
    }
    
    std::vector<double> zero_rates;
    for (size_t i = 0; i < times_.size(); ++i) {
        zero_rates.push_back(
            DiscountFactor::to_zero_rate(times_[i], discount_factors_[i], compounding_type_)
        );
    }
    
    spline_ = std::make_unique<CubicSpline>();
    spline_->fit(times_, zero_rates);
    use_spline_ = true;
}

bool YieldCurve::has_arbitrage() const {
    for (size_t i = 0; i < times_.size() - 1; ++i) {
        double fwd = get_forward_rate(times_[i], times_[i + 1]);
        if (fwd < -1e-6) {
            return true;
        }
    }
    return false;
}

}
