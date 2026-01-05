#include "option_types.hpp"

namespace implied_vol {

std::string ImpliedVolResult::status_string() const {
    switch (status) {
        case ConvergenceStatus::SUCCESS:
            return "SUCCESS";
        case ConvergenceStatus::MAX_ITERATIONS_REACHED:
            return "MAX_ITERATIONS_REACHED";
        case ConvergenceStatus::VEGA_TOO_SMALL:
            return "VEGA_TOO_SMALL";
        case ConvergenceStatus::PRICE_OUT_OF_BOUNDS:
            return "PRICE_OUT_OF_BOUNDS";
        case ConvergenceStatus::NEGATIVE_VOLATILITY:
            return "NEGATIVE_VOLATILITY";
        case ConvergenceStatus::ARBITRAGE_VIOLATION:
            return "ARBITRAGE_VIOLATION";
        case ConvergenceStatus::INVALID_INPUT:
            return "INVALID_INPUT";
        default:
            return "UNKNOWN";
    }
}

}
