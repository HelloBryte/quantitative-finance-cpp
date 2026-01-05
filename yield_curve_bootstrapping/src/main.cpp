#include "bootstrapper.hpp"
#include "forward_curve.hpp"
#include <iostream>
#include <iomanip>
#include <vector>

using namespace yield_curve;

void print_separator() {
    std::cout << std::string(80, '=') << "\n";
}

void demo_basic_bootstrapping() {
    print_separator();
    std::cout << "DEMO 1: Basic Yield Curve Bootstrapping\n";
    print_separator();
    
    std::vector<BondData> bonds = {
        BondData(0.5, 0.00, 2, 98.50),
        BondData(1.0, 0.02, 2, 99.00),
        BondData(1.5, 0.03, 2, 99.50),
        BondData(2.0, 0.04, 2, 100.00),
        BondData(3.0, 0.045, 2, 101.50),
        BondData(5.0, 0.05, 2, 103.00)
    };
    
    Bootstrapper bootstrapper(CompoundingType::CONTINUOUS, InterpolationType::LOG_LINEAR);
    YieldCurve curve = bootstrapper.bootstrap(bonds);
    
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "\nBootstrapped Zero-Coupon Curve:\n";
    std::cout << std::setw(12) << "Maturity" 
              << std::setw(18) << "Discount Factor"
              << std::setw(15) << "Zero Rate\n";
    std::cout << std::string(45, '-') << "\n";
    
    for (size_t i = 0; i < curve.size(); ++i) {
        double t = curve.times()[i];
        double df = curve.discount_factors()[i];
        double rate = curve.get_zero_rate(t);
        
        std::cout << std::setw(12) << t
                  << std::setw(18) << df
                  << std::setw(14) << (rate * 100) << "%\n";
    }
    
    std::cout << "\nInterpolated Values:\n";
    std::vector<double> test_times = {0.25, 0.75, 2.5, 4.0};
    for (double t : test_times) {
        double df = curve.get_discount_factor(t);
        double rate = curve.get_zero_rate(t);
        std::cout << "  t=" << t << "y: DF=" << df 
                  << ", Rate=" << (rate * 100) << "%\n";
    }
    std::cout << "\n";
}

void demo_interpolation_comparison() {
    print_separator();
    std::cout << "DEMO 2: Interpolation Method Comparison\n";
    print_separator();
    
    std::vector<BondData> bonds = {
        BondData(1.0, 0.02, 1, 99.00),
        BondData(2.0, 0.03, 1, 99.50),
        BondData(3.0, 0.04, 1, 100.00)
    };
    
    std::vector<InterpolationType> methods = {
        InterpolationType::LINEAR,
        InterpolationType::LOG_LINEAR,
        InterpolationType::FLAT_FORWARD
    };
    
    std::vector<std::string> method_names = {
        "Linear", "Log-Linear", "Flat-Forward"
    };
    
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "\nDiscount Factor at t=1.5:\n";
    
    for (size_t i = 0; i < methods.size(); ++i) {
        Bootstrapper bootstrapper(CompoundingType::CONTINUOUS, methods[i]);
        YieldCurve curve = bootstrapper.bootstrap(bonds);
        double df = curve.get_discount_factor(1.5);
        double rate = curve.get_zero_rate(1.5);
        
        std::cout << "  " << std::setw(15) << method_names[i] 
                  << ": DF=" << df << ", Rate=" << (rate * 100) << "%\n";
    }
    std::cout << "\n";
}

void demo_forward_curve() {
    print_separator();
    std::cout << "DEMO 3: Forward Rate Curve\n";
    print_separator();
    
    std::vector<BondData> bonds = {
        BondData(1.0, 0.02, 1, 99.00),
        BondData(2.0, 0.025, 1, 99.20),
        BondData(3.0, 0.03, 1, 99.50),
        BondData(4.0, 0.035, 1, 99.80),
        BondData(5.0, 0.04, 1, 100.00)
    };
    
    Bootstrapper bootstrapper(CompoundingType::CONTINUOUS, InterpolationType::LOG_LINEAR);
    YieldCurve curve = bootstrapper.bootstrap(bonds);
    ForwardCurve forward_curve(curve);
    
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "\nSpot vs Forward Rates:\n";
    std::cout << std::setw(12) << "Maturity"
              << std::setw(15) << "Spot Rate"
              << std::setw(18) << "Forward Rate\n";
    std::cout << std::string(45, '-') << "\n";
    
    for (size_t i = 0; i < curve.size(); ++i) {
        double t = curve.times()[i];
        double spot_rate = curve.get_zero_rate(t);
        
        std::cout << std::setw(12) << t
                  << std::setw(14) << (spot_rate * 100) << "%";
        
        if (i < curve.size() - 1) {
            double t_next = curve.times()[i + 1];
            double fwd = forward_curve.get_forward_rate(t, t_next);
            std::cout << std::setw(17) << (fwd * 100) << "%";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void demo_cubic_spline_smoothing() {
    print_separator();
    std::cout << "DEMO 4: Cubic Spline Smoothing\n";
    print_separator();
    
    std::vector<BondData> bonds = {
        BondData(0.5, 0.01, 2, 99.50),
        BondData(1.0, 0.02, 2, 99.00),
        BondData(2.0, 0.03, 2, 99.00),
        BondData(3.0, 0.035, 2, 99.50),
        BondData(5.0, 0.04, 2, 100.00)
    };
    
    Bootstrapper bootstrapper(CompoundingType::CONTINUOUS, InterpolationType::LOG_LINEAR);
    
    YieldCurve curve_original = bootstrapper.bootstrap(bonds);
    YieldCurve curve_smoothed = bootstrapper.bootstrap_with_spline(bonds);
    
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "\nOriginal vs Smoothed Rates:\n";
    std::cout << std::setw(12) << "Time"
              << std::setw(18) << "Original Rate"
              << std::setw(18) << "Smoothed Rate\n";
    std::cout << std::string(48, '-') << "\n";
    
    std::vector<double> test_times = {0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0};
    for (double t : test_times) {
        double rate_orig = curve_original.get_zero_rate(t);
        double rate_smooth = curve_smoothed.get_zero_rate(t);
        
        std::cout << std::setw(12) << t
                  << std::setw(17) << (rate_orig * 100) << "%"
                  << std::setw(17) << (rate_smooth * 100) << "%\n";
    }
    std::cout << "\n";
}

void demo_compounding_conventions() {
    print_separator();
    std::cout << "DEMO 5: Compounding Convention Comparison\n";
    print_separator();
    
    std::vector<BondData> bonds = {
        BondData(1.0, 0.03, 2, 99.00),
        BondData(2.0, 0.04, 2, 99.50),
        BondData(3.0, 0.045, 2, 100.00)
    };
    
    std::vector<CompoundingType> conventions = {
        CompoundingType::CONTINUOUS,
        CompoundingType::ANNUAL,
        CompoundingType::SEMI_ANNUAL
    };
    
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "\nZero Rate at t=2.0 under different conventions:\n\n";
    
    for (auto conv : conventions) {
        Bootstrapper bootstrapper(conv, InterpolationType::LOG_LINEAR);
        YieldCurve curve = bootstrapper.bootstrap(bonds);
        double rate = curve.get_zero_rate(2.0);
        
        std::cout << "  " << std::setw(15) << compounding_type_string(conv)
                  << ": " << (rate * 100) << "%\n";
    }
    std::cout << "\n";
}

void demo_arbitrage_detection() {
    print_separator();
    std::cout << "DEMO 6: Arbitrage Detection\n";
    print_separator();
    
    std::vector<BondData> good_bonds = {
        BondData(1.0, 0.02, 1, 99.00),
        BondData(2.0, 0.03, 1, 99.50),
        BondData(3.0, 0.04, 1, 100.00)
    };
    
    Bootstrapper bootstrapper(CompoundingType::CONTINUOUS, InterpolationType::LOG_LINEAR);
    YieldCurve curve = bootstrapper.bootstrap(good_bonds);
    
    std::cout << "Testing curve for arbitrage opportunities...\n";
    std::cout << "  Has arbitrage: " << (curve.has_arbitrage() ? "YES" : "NO") << "\n\n";
    
    std::cout << "Forward rates (should all be positive):\n";
    for (size_t i = 0; i < curve.size() - 1; ++i) {
        double t1 = curve.times()[i];
        double t2 = curve.times()[i + 1];
        double fwd = curve.get_forward_rate(t1, t2);
        std::cout << "  f(" << t1 << ", " << t2 << ") = " 
                  << std::fixed << std::setprecision(4) << (fwd * 100) << "%\n";
    }
    std::cout << "\n";
}

int main() {
    std::cout << "\n";
    print_separator();
    std::cout << "    YIELD CURVE BOOTSTRAPPING - DEMONSTRATION\n";
    std::cout << "  Zero-Coupon Curve Construction from Bond Prices\n";
    print_separator();
    std::cout << "\n";
    
    demo_basic_bootstrapping();
    demo_interpolation_comparison();
    demo_forward_curve();
    demo_cubic_spline_smoothing();
    demo_compounding_conventions();
    demo_arbitrage_detection();
    
    print_separator();
    std::cout << "All demonstrations completed successfully!\n";
    print_separator();
    std::cout << "\n";
    
    return 0;
}
