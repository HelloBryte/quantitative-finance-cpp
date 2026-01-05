#pragma once

#include <string>
#include <vector>

namespace implied_vol {

enum class OptionType {
    CALL,
    PUT
};

struct OptionSpec {
    double spot;
    double strike;
    double time_to_expiry;
    double risk_free_rate;
    OptionType type;
    
    OptionSpec(double S, double K, double T, double r, OptionType opt_type)
        : spot(S), strike(K), time_to_expiry(T), risk_free_rate(r), type(opt_type) {}
};

enum class ConvergenceStatus {
    SUCCESS,
    MAX_ITERATIONS_REACHED,
    VEGA_TOO_SMALL,
    PRICE_OUT_OF_BOUNDS,
    NEGATIVE_VOLATILITY,
    ARBITRAGE_VIOLATION,
    INVALID_INPUT
};

struct ImpliedVolResult {
    double implied_vol;
    int iterations;
    double final_error;
    ConvergenceStatus status;
    
    ImpliedVolResult()
        : implied_vol(0.0), iterations(0), final_error(0.0), 
          status(ConvergenceStatus::INVALID_INPUT) {}
    
    ImpliedVolResult(double vol, int iter, double err, ConvergenceStatus stat)
        : implied_vol(vol), iterations(iter), final_error(err), status(stat) {}
    
    bool is_success() const {
        return status == ConvergenceStatus::SUCCESS;
    }
    
    std::string status_string() const;
};

struct VolSmile {
    std::vector<double> strikes;
    std::vector<double> implied_vols;
    std::vector<ConvergenceStatus> statuses;
    
    void add_point(double strike, double vol, ConvergenceStatus status) {
        strikes.push_back(strike);
        implied_vols.push_back(vol);
        statuses.push_back(status);
    }
};

}
