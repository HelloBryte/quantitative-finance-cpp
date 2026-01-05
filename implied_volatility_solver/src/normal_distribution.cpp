#include "normal_distribution.hpp"
#include <cmath>

namespace implied_vol {

double NormalDistribution::erf_approx(double x) {
    constexpr double a1 =  0.254829592;
    constexpr double a2 = -0.284496736;
    constexpr double a3 =  1.421413741;
    constexpr double a4 = -1.453152027;
    constexpr double a5 =  1.061405429;
    constexpr double p  =  0.3275911;
    
    int sign = (x >= 0) ? 1 : -1;
    x = std::abs(x);
    
    double t = 1.0 / (1.0 + p * x);
    double y = 1.0 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * std::exp(-x * x);
    
    return sign * y;
}

double NormalDistribution::cdf(double x) {
    return 0.5 * (1.0 + erf_approx(x / SQRT_2));
}

double NormalDistribution::pdf(double x) {
    return INV_SQRT_2PI * std::exp(-0.5 * x * x);
}

}
