#pragma once

#include <vector>
#include <memory>

namespace yield_curve {

enum class InterpolationType {
    LINEAR,
    LOG_LINEAR,
    FLAT_FORWARD
};

class Interpolator {
public:
    virtual ~Interpolator() = default;
    
    virtual double interpolate(
        double t,
        const std::vector<double>& times,
        const std::vector<double>& discount_factors
    ) const = 0;
    
    virtual std::string name() const = 0;
    
protected:
    size_t find_interval(double t, const std::vector<double>& times) const;
};

class LinearInterpolator : public Interpolator {
public:
    double interpolate(
        double t,
        const std::vector<double>& times,
        const std::vector<double>& discount_factors
    ) const override;
    
    std::string name() const override { return "Linear"; }
};

class LogLinearInterpolator : public Interpolator {
public:
    double interpolate(
        double t,
        const std::vector<double>& times,
        const std::vector<double>& discount_factors
    ) const override;
    
    std::string name() const override { return "Log-Linear"; }
};

class FlatForwardInterpolator : public Interpolator {
public:
    double interpolate(
        double t,
        const std::vector<double>& times,
        const std::vector<double>& discount_factors
    ) const override;
    
    std::string name() const override { return "Flat-Forward"; }
};

std::unique_ptr<Interpolator> create_interpolator(InterpolationType type);

}
