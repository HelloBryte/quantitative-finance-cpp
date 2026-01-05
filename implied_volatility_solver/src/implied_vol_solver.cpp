#include "implied_vol_solver.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace implied_vol {

ImpliedVolSolver::ImpliedVolSolver() : bs_engine_() {}

bool ImpliedVolSolver::validate_inputs(const OptionSpec& spec, double market_price) const {
    if (spec.spot <= 0 || spec.strike <= 0 || spec.time_to_expiry <= 0) {
        return false;
    }
    
    if (market_price < 0) {
        return false;
    }
    
    double intrinsic = bs_engine_.intrinsic_value(spec);
    if (market_price < intrinsic - 1e-6) {
        return false;
    }
    
    return true;
}

double ImpliedVolSolver::get_initial_guess(const OptionSpec& spec, double market_price) const {
    constexpr double PI = 3.14159265358979323846;
    double sqrt_2pi_over_T = std::sqrt(2.0 * PI / spec.time_to_expiry);
    double atm_approx = sqrt_2pi_over_T * (market_price / spec.spot);
    
    if (atm_approx > 0.01 && atm_approx < 2.0) {
        return atm_approx;
    }
    
    return 0.2;
}

double ImpliedVolSolver::clamp_volatility(double vol) const {
    return std::clamp(vol, VOL_MIN, VOL_MAX);
}

ImpliedVolResult ImpliedVolSolver::solve_newton_raphson(
    const OptionSpec& spec,
    double market_price,
    double initial_guess,
    double tolerance,
    int max_iterations
) {
    if (!validate_inputs(spec, market_price)) {
        return ImpliedVolResult(0.0, 0, 0.0, ConvergenceStatus::INVALID_INPUT);
    }
    
    double sigma = clamp_volatility(initial_guess);
    
    for (int iter = 0; iter < max_iterations; ++iter) {
        double price = bs_engine_.price(spec, sigma);
        double price_diff = price - market_price;
        
        if (std::abs(price_diff) < tolerance) {
            return ImpliedVolResult(sigma, iter + 1, price_diff, ConvergenceStatus::SUCCESS);
        }
        
        double vega_val = bs_engine_.vega(spec, sigma);
        
        if (vega_val < VEGA_MIN_THRESHOLD) {
            return ImpliedVolResult(sigma, iter + 1, price_diff, ConvergenceStatus::VEGA_TOO_SMALL);
        }
        
        double sigma_new = sigma - price_diff / vega_val;
        sigma_new = clamp_volatility(sigma_new);
        
        if (std::abs(sigma_new - sigma) < tolerance * 0.01) {
            return ImpliedVolResult(sigma_new, iter + 1, price_diff, ConvergenceStatus::SUCCESS);
        }
        
        sigma = sigma_new;
    }
    
    double final_error = bs_engine_.price(spec, sigma) - market_price;
    return ImpliedVolResult(sigma, max_iterations, final_error, ConvergenceStatus::MAX_ITERATIONS_REACHED);
}

ImpliedVolResult ImpliedVolSolver::solve_brent(
    const OptionSpec& spec,
    double market_price,
    double vol_low,
    double vol_high,
    double tolerance,
    int max_iterations
) {
    if (!validate_inputs(spec, market_price)) {
        return ImpliedVolResult(0.0, 0, 0.0, ConvergenceStatus::INVALID_INPUT);
    }
    
    double a = vol_low;
    double b = vol_high;
    double fa = bs_engine_.price(spec, a) - market_price;
    double fb = bs_engine_.price(spec, b) - market_price;
    
    if (fa * fb > 0) {
        return ImpliedVolResult(0.0, 0, 0.0, ConvergenceStatus::PRICE_OUT_OF_BOUNDS);
    }
    
    if (std::abs(fa) < std::abs(fb)) {
        std::swap(a, b);
        std::swap(fa, fb);
    }
    
    double c = a;
    double fc = fa;
    bool mflag = true;
    double s = 0.0;
    double d = 0.0;
    
    for (int iter = 0; iter < max_iterations; ++iter) {
        if (std::abs(fb) < tolerance) {
            return ImpliedVolResult(b, iter + 1, fb, ConvergenceStatus::SUCCESS);
        }
        
        if (std::abs(b - a) < tolerance) {
            return ImpliedVolResult(b, iter + 1, fb, ConvergenceStatus::SUCCESS);
        }
        
        if (fa != fc && fb != fc) {
            s = a * fb * fc / ((fa - fb) * (fa - fc)) +
                b * fa * fc / ((fb - fa) * (fb - fc)) +
                c * fa * fb / ((fc - fa) * (fc - fb));
        } else {
            s = b - fb * (b - a) / (fb - fa);
        }
        
        double tmp2 = (3.0 * a + b) / 4.0;
        bool condition1 = !((s > tmp2 && s < b) || (s < tmp2 && s > b));
        bool condition2 = mflag && (std::abs(s - b) >= std::abs(b - c) / 2.0);
        bool condition3 = !mflag && (std::abs(s - b) >= std::abs(c - d) / 2.0);
        bool condition4 = mflag && (std::abs(b - c) < tolerance);
        bool condition5 = !mflag && (std::abs(c - d) < tolerance);
        
        if (condition1 || condition2 || condition3 || condition4 || condition5) {
            s = (a + b) / 2.0;
            mflag = true;
        } else {
            mflag = false;
        }
        
        double fs = bs_engine_.price(spec, s) - market_price;
        d = c;
        c = b;
        fc = fb;
        
        if (fa * fs < 0) {
            b = s;
            fb = fs;
        } else {
            a = s;
            fa = fs;
        }
        
        if (std::abs(fa) < std::abs(fb)) {
            std::swap(a, b);
            std::swap(fa, fb);
        }
    }
    
    return ImpliedVolResult(b, max_iterations, fb, ConvergenceStatus::MAX_ITERATIONS_REACHED);
}

ImpliedVolResult ImpliedVolSolver::solve_with_fallback(
    const OptionSpec& spec,
    double market_price
) {
    double initial_guess = get_initial_guess(spec, market_price);
    
    ImpliedVolResult result = solve_newton_raphson(spec, market_price, initial_guess);
    
    if (result.is_success()) {
        return result;
    }
    
    if (result.status == ConvergenceStatus::VEGA_TOO_SMALL || 
        result.status == ConvergenceStatus::MAX_ITERATIONS_REACHED) {
        return solve_brent(spec, market_price);
    }
    
    return result;
}

VolSmile ImpliedVolSolver::compute_vol_smile(
    double spot,
    const std::vector<double>& strikes,
    const std::vector<double>& market_prices,
    double time_to_expiry,
    double risk_free_rate,
    OptionType type
) {
    VolSmile smile;
    
    if (strikes.size() != market_prices.size()) {
        return smile;
    }
    
    double prev_vol = 0.2;
    
    for (size_t i = 0; i < strikes.size(); ++i) {
        OptionSpec spec(spot, strikes[i], time_to_expiry, risk_free_rate, type);
        
        ImpliedVolResult result = solve_newton_raphson(spec, market_prices[i], prev_vol);
        
        if (!result.is_success()) {
            result = solve_brent(spec, market_prices[i]);
        }
        
        smile.add_point(strikes[i], result.implied_vol, result.status);
        
        if (result.is_success()) {
            prev_vol = result.implied_vol;
        }
    }
    
    return smile;
}

}
