#include "implied_vol_solver.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>

using namespace implied_vol;

void print_separator() {
    std::cout << std::string(80, '=') << "\n";
}

void demo_basic_usage() {
    print_separator();
    std::cout << "DEMO 1: Basic Implied Volatility Calculation\n";
    print_separator();
    
    BlackScholesEngine engine;
    ImpliedVolSolver solver;
    
    double S = 100.0, K = 100.0, T = 1.0, r = 0.05;
    double true_vol = 0.25;
    
    OptionSpec spec(S, K, T, r, OptionType::CALL);
    double market_price = engine.price(spec, true_vol);
    
    std::cout << "Market Conditions:\n";
    std::cout << "  Spot Price (S):      $" << S << "\n";
    std::cout << "  Strike Price (K):    $" << K << "\n";
    std::cout << "  Time to Expiry (T):  " << T << " years\n";
    std::cout << "  Risk-free Rate (r):  " << (r * 100) << "%\n";
    std::cout << "  Market Price:        $" << std::fixed << std::setprecision(4) << market_price << "\n\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    ImpliedVolResult result = solver.solve_newton_raphson(spec, market_price);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Newton-Raphson Results:\n";
    std::cout << "  Status:              " << result.status_string() << "\n";
    std::cout << "  Implied Volatility:  " << (result.implied_vol * 100) << "%\n";
    std::cout << "  Iterations:          " << result.iterations << "\n";
    std::cout << "  Final Error:         $" << std::scientific << result.final_error << "\n";
    std::cout << "  Computation Time:    " << duration.count() << " μs\n";
    std::cout << "  True Volatility:     " << (true_vol * 100) << "%\n\n";
}

void demo_method_comparison() {
    print_separator();
    std::cout << "DEMO 2: Method Comparison (Newton-Raphson vs Brent)\n";
    print_separator();
    
    BlackScholesEngine engine;
    ImpliedVolSolver solver;
    
    double S = 100.0, K = 100.0, T = 1.0, r = 0.05;
    double true_vol = 0.30;
    
    OptionSpec spec(S, K, T, r, OptionType::CALL);
    double market_price = engine.price(spec, true_vol);
    
    auto start_nr = std::chrono::high_resolution_clock::now();
    ImpliedVolResult result_nr = solver.solve_newton_raphson(spec, market_price);
    auto end_nr = std::chrono::high_resolution_clock::now();
    auto duration_nr = std::chrono::duration_cast<std::chrono::microseconds>(end_nr - start_nr);
    
    auto start_brent = std::chrono::high_resolution_clock::now();
    ImpliedVolResult result_brent = solver.solve_brent(spec, market_price);
    auto end_brent = std::chrono::high_resolution_clock::now();
    auto duration_brent = std::chrono::duration_cast<std::chrono::microseconds>(end_brent - start_brent);
    
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Newton-Raphson:\n";
    std::cout << "  Implied Vol:  " << (result_nr.implied_vol * 100) << "%\n";
    std::cout << "  Iterations:   " << result_nr.iterations << "\n";
    std::cout << "  Time:         " << duration_nr.count() << " μs\n\n";
    
    std::cout << "Brent's Method:\n";
    std::cout << "  Implied Vol:  " << (result_brent.implied_vol * 100) << "%\n";
    std::cout << "  Iterations:   " << result_brent.iterations << "\n";
    std::cout << "  Time:         " << duration_brent.count() << " μs\n\n";
    
    std::cout << "Speed Ratio: " << std::setprecision(2) 
              << (static_cast<double>(duration_brent.count()) / duration_nr.count()) << "x faster (Newton-Raphson)\n\n";
}

void demo_volatility_smile() {
    print_separator();
    std::cout << "DEMO 3: Volatility Smile Calculation\n";
    print_separator();
    
    BlackScholesEngine engine;
    ImpliedVolSolver solver;
    
    double S = 100.0, T = 1.0, r = 0.05;
    
    std::vector<double> strikes = {80.0, 85.0, 90.0, 95.0, 100.0, 105.0, 110.0, 115.0, 120.0};
    std::vector<double> true_vols = {0.28, 0.25, 0.23, 0.21, 0.20, 0.21, 0.23, 0.25, 0.28};
    
    std::vector<double> market_prices;
    for (size_t i = 0; i < strikes.size(); ++i) {
        OptionSpec spec(S, strikes[i], T, r, OptionType::CALL);
        market_prices.push_back(engine.price(spec, true_vols[i]));
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    VolSmile smile = solver.compute_vol_smile(S, strikes, market_prices, T, r);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << std::setw(10) << "Strike" 
              << std::setw(15) << "Market Price"
              << std::setw(15) << "Implied Vol"
              << std::setw(15) << "True Vol"
              << std::setw(12) << "Moneyness\n";
    std::cout << std::string(67, '-') << "\n";
    
    for (size_t i = 0; i < smile.strikes.size(); ++i) {
        double moneyness = S / strikes[i];
        std::cout << std::fixed << std::setprecision(2)
                  << std::setw(10) << strikes[i]
                  << std::setw(15) << market_prices[i]
                  << std::setw(14) << (smile.implied_vols[i] * 100) << "%"
                  << std::setw(14) << (true_vols[i] * 100) << "%"
                  << std::setw(12) << moneyness << "\n";
    }
    
    std::cout << "\nTotal computation time: " << duration.count() << " μs\n";
    std::cout << "Average per strike: " << (duration.count() / strikes.size()) << " μs\n\n";
}

void demo_put_call_parity() {
    print_separator();
    std::cout << "DEMO 4: Put-Call Parity Verification\n";
    print_separator();
    
    BlackScholesEngine engine;
    ImpliedVolSolver solver;
    
    double S = 100.0, K = 100.0, T = 1.0, r = 0.05;
    double vol = 0.25;
    
    OptionSpec call_spec(S, K, T, r, OptionType::CALL);
    OptionSpec put_spec(S, K, T, r, OptionType::PUT);
    
    double call_price = engine.price(call_spec, vol);
    double put_price = engine.price(put_spec, vol);
    
    ImpliedVolResult call_result = solver.solve_newton_raphson(call_spec, call_price);
    ImpliedVolResult put_result = solver.solve_newton_raphson(put_spec, put_price);
    
    bool parity_holds = engine.verify_put_call_parity(call_price, put_price, call_spec);
    
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Call Option:\n";
    std::cout << "  Price:        $" << call_price << "\n";
    std::cout << "  Implied Vol:  " << (call_result.implied_vol * 100) << "%\n\n";
    
    std::cout << "Put Option:\n";
    std::cout << "  Price:        $" << put_price << "\n";
    std::cout << "  Implied Vol:  " << (put_result.implied_vol * 100) << "%\n\n";
    
    std::cout << "Put-Call Parity Check:\n";
    std::cout << "  C - P = " << (call_price - put_price) << "\n";
    std::cout << "  S - K*e^(-rT) = " << (S - K * std::exp(-r * T)) << "\n";
    std::cout << "  Parity Holds: " << (parity_holds ? "YES ✓" : "NO ✗") << "\n\n";
}

void demo_greeks() {
    print_separator();
    std::cout << "DEMO 5: Greeks Calculation\n";
    print_separator();
    
    BlackScholesEngine engine;
    
    double S = 100.0, K = 100.0, T = 1.0, r = 0.05;
    double vol = 0.25;
    
    OptionSpec call_spec(S, K, T, r, OptionType::CALL);
    
    double price = engine.price(call_spec, vol);
    double delta = engine.delta(call_spec, vol);
    double gamma = engine.gamma(call_spec, vol);
    double vega = engine.vega(call_spec, vol);
    double theta = engine.theta(call_spec, vol);
    double rho = engine.rho(call_spec, vol);
    
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Option Price: $" << price << "\n\n";
    std::cout << "Greeks:\n";
    std::cout << "  Delta (∂C/∂S):  " << delta << "\n";
    std::cout << "  Gamma (∂²C/∂S²): " << gamma << "\n";
    std::cout << "  Vega (∂C/∂σ):   " << vega << "\n";
    std::cout << "  Theta (∂C/∂t):  " << theta << " (per year)\n";
    std::cout << "  Rho (∂C/∂r):    " << rho << "\n\n";
}

void demo_edge_cases() {
    print_separator();
    std::cout << "DEMO 6: Edge Cases & Robustness\n";
    print_separator();
    
    BlackScholesEngine engine;
    ImpliedVolSolver solver;
    
    struct TestCase {
        std::string name;
        double S, K, T, r, vol;
    };
    
    std::vector<TestCase> cases = {
        {"ATM Standard", 100.0, 100.0, 1.0, 0.05, 0.25},
        {"Deep ITM", 150.0, 100.0, 1.0, 0.05, 0.25},
        {"Deep OTM", 50.0, 100.0, 1.0, 0.05, 0.25},
        {"Near Expiry", 100.0, 100.0, 0.01, 0.05, 0.25},
        {"High Vol", 100.0, 100.0, 1.0, 0.05, 1.0},
        {"Low Vol", 100.0, 100.0, 1.0, 0.05, 0.05},
        {"Zero Rate", 100.0, 100.0, 1.0, 0.0, 0.25}
    };
    
    std::cout << std::setw(20) << "Case" 
              << std::setw(12) << "Status"
              << std::setw(10) << "Iters"
              << std::setw(15) << "Recovered Vol"
              << std::setw(12) << "Error\n";
    std::cout << std::string(69, '-') << "\n";
    
    for (const auto& tc : cases) {
        OptionSpec spec(tc.S, tc.K, tc.T, tc.r, OptionType::CALL);
        double market_price = engine.price(spec, tc.vol);
        
        ImpliedVolResult result = solver.solve_with_fallback(spec, market_price);
        
        double error = std::abs(result.implied_vol - tc.vol);
        
        std::cout << std::setw(20) << tc.name
                  << std::setw(12) << (result.is_success() ? "SUCCESS" : "FAILED")
                  << std::setw(10) << result.iterations
                  << std::setw(14) << std::fixed << std::setprecision(4) << (result.implied_vol * 100) << "%"
                  << std::setw(12) << std::scientific << std::setprecision(2) << error << "\n";
    }
    std::cout << "\n";
}

int main() {
    std::cout << "\n";
    print_separator();
    std::cout << "        IMPLIED VOLATILITY SOLVER - DEMONSTRATION\n";
    std::cout << "    Black-Scholes Option Pricing & Vol Calibration\n";
    print_separator();
    std::cout << "\n";
    
    demo_basic_usage();
    demo_method_comparison();
    demo_volatility_smile();
    demo_put_call_parity();
    demo_greeks();
    demo_edge_cases();
    
    print_separator();
    std::cout << "All demonstrations completed successfully!\n";
    print_separator();
    std::cout << "\n";
    
    return 0;
}
