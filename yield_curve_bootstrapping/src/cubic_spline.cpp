#include "cubic_spline.hpp"
#include <stdexcept>
#include <algorithm>

namespace yield_curve {

void CubicSpline::fit(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size()) {
        throw std::invalid_argument("x and y must have same size");
    }
    
    if (x.size() < 2) {
        throw std::invalid_argument("Need at least 2 points for spline");
    }
    
    size_t n = x.size() - 1;
    
    x_ = x;
    y_ = y;
    
    std::vector<double> h(n);
    for (size_t i = 0; i < n; ++i) {
        h[i] = x[i + 1] - x[i];
        if (h[i] <= 0) {
            throw std::invalid_argument("x values must be strictly increasing");
        }
    }
    
    std::vector<double> alpha(n);
    for (size_t i = 1; i < n; ++i) {
        alpha[i] = 3.0 * ((y[i + 1] - y[i]) / h[i] - (y[i] - y[i - 1]) / h[i - 1]);
    }
    
    std::vector<double> l(n + 1), mu(n + 1), z(n + 1);
    l[0] = 1.0;
    mu[0] = 0.0;
    z[0] = 0.0;
    
    for (size_t i = 1; i < n; ++i) {
        l[i] = 2.0 * (x[i + 1] - x[i - 1]) - h[i - 1] * mu[i - 1];
        mu[i] = h[i] / l[i];
        z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
    }
    
    l[n] = 1.0;
    z[n] = 0.0;
    c_.resize(n + 1);
    c_[n] = 0.0;
    
    b_.resize(n);
    d_.resize(n);
    a_ = y_;
    
    for (int j = n - 1; j >= 0; --j) {
        c_[j] = z[j] - mu[j] * c_[j + 1];
        b_[j] = (y[j + 1] - y[j]) / h[j] - h[j] * (c_[j + 1] + 2.0 * c_[j]) / 3.0;
        d_[j] = (c_[j + 1] - c_[j]) / (3.0 * h[j]);
    }
    
    fitted_ = true;
}

double CubicSpline::evaluate(double x) const {
    if (!fitted_) {
        throw std::runtime_error("Spline not fitted");
    }
    
    if (x <= x_.front()) {
        return y_.front();
    }
    
    if (x >= x_.back()) {
        return y_.back();
    }
    
    size_t i = find_interval(x);
    
    double dx = x - x_[i];
    return a_[i] + b_[i] * dx + c_[i] * dx * dx + d_[i] * dx * dx * dx;
}

double CubicSpline::derivative(double x) const {
    if (!fitted_) {
        throw std::runtime_error("Spline not fitted");
    }
    
    if (x <= x_.front() || x >= x_.back()) {
        return 0.0;
    }
    
    size_t i = find_interval(x);
    
    double dx = x - x_[i];
    return b_[i] + 2.0 * c_[i] * dx + 3.0 * d_[i] * dx * dx;
}

size_t CubicSpline::find_interval(double x) const {
    auto it = std::lower_bound(x_.begin(), x_.end(), x);
    size_t idx = std::distance(x_.begin(), it);
    
    if (idx > 0) {
        --idx;
    }
    
    if (idx >= x_.size() - 1) {
        idx = x_.size() - 2;
    }
    
    return idx;
}

void CubicSpline::solve_tridiagonal(
    const std::vector<double>& a,
    const std::vector<double>& b,
    const std::vector<double>& c,
    const std::vector<double>& d,
    std::vector<double>& x
) {
    size_t n = d.size();
    std::vector<double> c_prime(n);
    std::vector<double> d_prime(n);
    
    c_prime[0] = c[0] / b[0];
    d_prime[0] = d[0] / b[0];
    
    for (size_t i = 1; i < n; ++i) {
        double m = b[i] - a[i] * c_prime[i - 1];
        c_prime[i] = c[i] / m;
        d_prime[i] = (d[i] - a[i] * d_prime[i - 1]) / m;
    }
    
    x.resize(n);
    x[n - 1] = d_prime[n - 1];
    
    for (int i = n - 2; i >= 0; --i) {
        x[i] = d_prime[i] - c_prime[i] * x[i + 1];
    }
}

}
