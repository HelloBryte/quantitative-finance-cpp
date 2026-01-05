#include "interpolation.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace yield_curve {

size_t Interpolator::find_interval(double t, const std::vector<double>& times) const {
    if (times.empty()) {
        throw std::runtime_error("Empty times vector");
    }
    
    if (t <= times.front()) {
        return 0;
    }
    
    if (t >= times.back()) {
        return times.size() - 2;
    }
    
    auto it = std::lower_bound(times.begin(), times.end(), t);
    size_t idx = std::distance(times.begin(), it);
    
    if (idx > 0) {
        --idx;
    }
    
    return idx;
}

double LinearInterpolator::interpolate(
    double t,
    const std::vector<double>& times,
    const std::vector<double>& discount_factors
) const {
    if (times.size() != discount_factors.size()) {
        throw std::invalid_argument("Times and discount factors size mismatch");
    }
    
    if (times.size() == 1) {
        return discount_factors[0];
    }
    
    if (t <= times.front()) {
        return discount_factors.front();
    }
    
    if (t >= times.back()) {
        return discount_factors.back();
    }
    
    size_t i = find_interval(t, times);
    
    double t1 = times[i];
    double t2 = times[i + 1];
    double df1 = discount_factors[i];
    double df2 = discount_factors[i + 1];
    
    double weight = (t - t1) / (t2 - t1);
    return df1 + weight * (df2 - df1);
}

double LogLinearInterpolator::interpolate(
    double t,
    const std::vector<double>& times,
    const std::vector<double>& discount_factors
) const {
    if (times.size() != discount_factors.size()) {
        throw std::invalid_argument("Times and discount factors size mismatch");
    }
    
    if (times.size() == 1) {
        return discount_factors[0];
    }
    
    if (t <= times.front()) {
        return discount_factors.front();
    }
    
    if (t >= times.back()) {
        return discount_factors.back();
    }
    
    size_t i = find_interval(t, times);
    
    double t1 = times[i];
    double t2 = times[i + 1];
    double df1 = discount_factors[i];
    double df2 = discount_factors[i + 1];
    
    double log_df1 = std::log(df1);
    double log_df2 = std::log(df2);
    
    double weight = (t - t1) / (t2 - t1);
    double log_df = log_df1 + weight * (log_df2 - log_df1);
    
    return std::exp(log_df);
}

double FlatForwardInterpolator::interpolate(
    double t,
    const std::vector<double>& times,
    const std::vector<double>& discount_factors
) const {
    if (times.size() != discount_factors.size()) {
        throw std::invalid_argument("Times and discount factors size mismatch");
    }
    
    if (times.size() == 1) {
        return discount_factors[0];
    }
    
    if (t <= times.front()) {
        return discount_factors.front();
    }
    
    if (t >= times.back()) {
        size_t n = times.size();
        double t1 = times[n - 2];
        double t2 = times[n - 1];
        double df1 = discount_factors[n - 2];
        double df2 = discount_factors[n - 1];
        
        double forward_rate = -std::log(df2 / df1) / (t2 - t1);
        return df2 * std::exp(-forward_rate * (t - t2));
    }
    
    size_t i = find_interval(t, times);
    
    double t1 = times[i];
    double t2 = times[i + 1];
    double df1 = discount_factors[i];
    double df2 = discount_factors[i + 1];
    
    double forward_rate = -std::log(df2 / df1) / (t2 - t1);
    
    return df1 * std::exp(-forward_rate * (t - t1));
}

std::unique_ptr<Interpolator> create_interpolator(InterpolationType type) {
    switch (type) {
        case InterpolationType::LINEAR:
            return std::make_unique<LinearInterpolator>();
        case InterpolationType::LOG_LINEAR:
            return std::make_unique<LogLinearInterpolator>();
        case InterpolationType::FLAT_FORWARD:
            return std::make_unique<FlatForwardInterpolator>();
        default:
            throw std::invalid_argument("Unknown interpolation type");
    }
}

}
