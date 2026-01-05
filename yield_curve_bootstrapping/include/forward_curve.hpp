#pragma once

#include "yield_curve.hpp"
#include <vector>

namespace yield_curve {

class ForwardCurve {
public:
    explicit ForwardCurve(const YieldCurve& yield_curve);
    
    double get_forward_rate(double t1, double t2) const;
    
    double get_instantaneous_forward(double t) const;
    
    std::vector<double> get_forward_curve(
        const std::vector<double>& tenors
    ) const;
    
private:
    const YieldCurve& yield_curve_;
};

}
