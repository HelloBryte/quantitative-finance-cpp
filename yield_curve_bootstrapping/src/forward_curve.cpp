#include "forward_curve.hpp"

namespace yield_curve {

ForwardCurve::ForwardCurve(const YieldCurve& yield_curve)
    : yield_curve_(yield_curve) {}

double ForwardCurve::get_forward_rate(double t1, double t2) const {
    return yield_curve_.get_forward_rate(t1, t2);
}

double ForwardCurve::get_instantaneous_forward(double t) const {
    return yield_curve_.get_instantaneous_forward(t);
}

std::vector<double> ForwardCurve::get_forward_curve(
    const std::vector<double>& tenors
) const {
    std::vector<double> forward_rates;
    
    for (size_t i = 0; i < tenors.size() - 1; ++i) {
        double fwd = get_forward_rate(tenors[i], tenors[i + 1]);
        forward_rates.push_back(fwd);
    }
    
    return forward_rates;
}

}
