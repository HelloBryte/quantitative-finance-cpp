#pragma once

#include <vector>

namespace yield_curve {

class CubicSpline {
public:
    CubicSpline() = default;
    
    void fit(const std::vector<double>& x, const std::vector<double>& y);
    
    double evaluate(double x) const;
    
    double derivative(double x) const;
    
    bool is_fitted() const { return fitted_; }
    
private:
    std::vector<double> x_;
    std::vector<double> y_;
    std::vector<double> a_;
    std::vector<double> b_;
    std::vector<double> c_;
    std::vector<double> d_;
    bool fitted_ = false;
    
    void solve_tridiagonal(
        const std::vector<double>& a,
        const std::vector<double>& b,
        const std::vector<double>& c,
        const std::vector<double>& d,
        std::vector<double>& x
    );
    
    size_t find_interval(double x) const;
};

}
