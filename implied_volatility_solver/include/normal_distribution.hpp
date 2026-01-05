#pragma once

namespace implied_vol {

class NormalDistribution {
public:
    static double cdf(double x);
    
    static double pdf(double x);
    
private:
    static constexpr double INV_SQRT_2PI = 0.3989422804014327;
    static constexpr double SQRT_2 = 1.4142135623730951;
    
    static double erf_approx(double x);
};

}
