# Implied Volatility Solver: From Securities Internship to Production-Grade Implementation

## Table of Contents
1. [Financial Context: Securities Internship Perspective](#financial-context)
2. [The Implied Volatility Problem](#the-problem)
3. [Mathematical Foundations](#mathematical-foundations)
4. [Numerical Methods](#numerical-methods)
5. [C++ Architecture Design](#cpp-architecture)
6. [Robustness & Edge Cases](#robustness)
7. [Advanced Features](#advanced-features)

---

## 1. Financial Context: Securities Internship Perspective

### What You Encounter in Practice

During a securities internship, particularly in derivatives trading or risk management, you frequently encounter:

- **Market Data**: Real-time option prices from exchanges (e.g., CBOE, CME)
- **Greeks Calculation**: Delta, Gamma, Vega, Theta, Rho for risk management
- **Volatility Surface**: Market makers quote options using implied volatility, not prices
- **Arbitrage Detection**: Identifying mispriced options requires accurate vol calculations
- **Risk Reports**: Daily P&L attribution often decomposed by volatility changes

### Why Implied Volatility Matters

**The Core Problem**: 
- Market quotes option **prices** (e.g., a call option trades at $5.20)
- Traders think in terms of **volatility** (e.g., "this option is trading at 25% vol")
- You need to convert: Price → Implied Volatility (σ_implied)

**Real-World Applications**:
1. **Volatility Smile/Skew**: Plot implied vol vs strike to see market sentiment
2. **Relative Value Trading**: Compare vols across strikes/maturities to find mispricing
3. **Risk Management**: VaR calculations require vol surfaces
4. **Model Calibration**: Fit local vol or stochastic vol models to market data

---

## 2. The Implied Volatility Problem

### Problem Statement

**Given**:
- Market price of an option: `C_market` (or `P_market` for puts)
- Contract specifications: `S` (spot), `K` (strike), `T` (time to maturity), `r` (risk-free rate)

**Find**:
- Implied volatility `σ_implied` such that:
  ```
  BS_Price(S, K, T, r, σ_implied) = C_market
  ```

### Why It's Non-Trivial

1. **No Closed-Form Inverse**: The Black-Scholes formula cannot be algebraically inverted
2. **Numerical Instability**: Near-zero or deep ITM/OTM options cause convergence issues
3. **Boundary Conditions**: σ must be positive; prices have arbitrage bounds
4. **Performance Requirements**: Need to solve thousands of options per second in production

---

## 3. Mathematical Foundations

### Black-Scholes Formula

For a **European Call Option**:

```
C(S, K, T, r, σ) = S·Φ(d₁) - K·e^(-rT)·Φ(d₂)

where:
    d₁ = [ln(S/K) + (r + σ²/2)T] / (σ√T)
    d₂ = d₁ - σ√T
    Φ(x) = Standard normal CDF
```

For a **European Put Option** (via put-call parity):
```
P(S, K, T, r, σ) = K·e^(-rT)·Φ(-d₂) - S·Φ(-d₁)
```

### Vega: The Key Derivative

**Vega** measures sensitivity of option price to volatility:

```
ν = ∂C/∂σ = S·φ(d₁)·√T

where:
    φ(x) = (1/√(2π))·e^(-x²/2)  [Standard normal PDF]
```

**Critical Properties**:
- Vega > 0 always (option value increases with volatility)
- Vega is maximized for ATM options
- Vega → 0 for deep ITM/OTM options (numerical challenges!)

### Why Vega Matters for Root-Finding

Newton-Raphson requires the derivative:
```
σ_(n+1) = σ_n - [BS_Price(σ_n) - C_market] / Vega(σ_n)
```

Vega acts as the "slope" that guides us toward the solution.

---

## 4. Numerical Methods

### Newton-Raphson Method

**Algorithm**:
```
Input: C_market, initial guess σ₀
Repeat until convergence:
    1. Calculate price: C_n = BS_Price(σ_n)
    2. Calculate Vega: ν_n = Vega(σ_n)
    3. Update: σ_(n+1) = σ_n - (C_n - C_market) / ν_n
    4. Check: |σ_(n+1) - σ_n| < tolerance
```

**Advantages**:
- Quadratic convergence (doubles correct digits per iteration)
- Typically converges in 3-5 iterations

**Failure Modes**:
- **Zero Vega**: Deep OTM options → division by zero
- **Overshooting**: Poor initial guess → negative σ
- **Flat Regions**: Near boundaries → slow convergence

### Fallback Methods

#### Brent's Method
- **Hybrid**: Combines bisection, secant, and inverse quadratic interpolation
- **Robust**: Guaranteed convergence if solution exists in bracket
- **Slower**: Linear convergence, but never fails

#### Bisection Method
- **Simplest**: Repeatedly halve the interval [σ_low, σ_high]
- **Bulletproof**: Always converges, but slow (1 bit per iteration)
- **Use Case**: Last resort when all else fails

### Initial Guess Strategies

**Naive**: σ₀ = 0.2 (20% vol - typical equity volatility)

**Intelligent** (Brenner-Subrahmanyam approximation for ATM options):
```
σ₀ ≈ √(2π/T) · (C_market/S)
```

**Production**: Use previous day's implied vol or interpolate from vol surface

---

## 5. C++ Architecture Design

### Design Principles

1. **Separation of Concerns**: Pricing, root-finding, and utilities in separate modules
2. **Template Metaprogramming**: Generic solvers work with any pricing model
3. **Exception Safety**: Handle edge cases without crashes
4. **Performance**: Inline hot paths, minimize allocations
5. **Testability**: Pure functions, dependency injection

### Module Structure

```
implied_volatility_solver/
├── include/
│   ├── black_scholes.hpp      # BS pricing & Greeks
│   ├── normal_distribution.hpp # Φ(x) and φ(x) implementations
│   ├── implied_vol_solver.hpp  # Root-finding algorithms
│   └── option_types.hpp        # Data structures (OptionSpec, Result)
├── src/
│   ├── black_scholes.cpp
│   ├── normal_distribution.cpp
│   ├── implied_vol_solver.cpp
│   └── main.cpp                # Demo & benchmarks
├── tests/
│   ├── test_black_scholes.cpp
│   ├── test_solvers.cpp
│   └── test_edge_cases.cpp
└── CMakeLists.txt
```

### Core Classes/Structs

#### OptionSpecification
```cpp
struct OptionSpec {
    double spot;           // S: Current asset price
    double strike;         // K: Strike price
    double time_to_expiry; // T: Years to expiration
    double risk_free_rate; // r: Annualized rate
    OptionType type;       // CALL or PUT
};
```

#### SolverResult
```cpp
struct ImpliedVolResult {
    double implied_vol;
    int iterations;
    double final_error;
    ConvergenceStatus status; // SUCCESS, MAX_ITER, VEGA_ZERO, etc.
};
```

### Key Interfaces

#### Pricing Engine
```cpp
class BlackScholesEngine {
public:
    double price(const OptionSpec& spec, double volatility) const;
    double vega(const OptionSpec& spec, double volatility) const;
    double delta(const OptionSpec& spec, double volatility) const;
    // ... other Greeks
};
```

#### Implied Vol Solver
```cpp
class ImpliedVolSolver {
public:
    ImpliedVolResult solve_newton_raphson(
        const OptionSpec& spec,
        double market_price,
        double initial_guess = 0.2,
        double tolerance = 1e-6,
        int max_iterations = 100
    );
    
    ImpliedVolResult solve_brent(
        const OptionSpec& spec,
        double market_price,
        double vol_low = 0.01,
        double vol_high = 5.0,
        double tolerance = 1e-6
    );
    
    ImpliedVolResult solve_with_fallback(
        const OptionSpec& spec,
        double market_price
    );
};
```

### Numerical Stability Techniques

1. **Intrinsic Value Check**:
   ```cpp
   double intrinsic = max(S - K, 0.0); // for calls
   if (market_price < intrinsic) {
       throw ArbitrageException("Price below intrinsic value");
   }
   ```

2. **Vega Threshold**:
   ```cpp
   if (vega < VEGA_MIN_THRESHOLD) {
       // Switch to Brent method
   }
   ```

3. **Volatility Bounds**:
   ```cpp
   sigma = clamp(sigma_new, VOL_MIN, VOL_MAX);
   ```

4. **Relative Error**:
   ```cpp
   double rel_error = abs(price - market_price) / market_price;
   ```

---

## 6. Robustness & Edge Cases

### Boundary Conditions

| Scenario | Challenge | Solution |
|----------|-----------|----------|
| Deep ITM | Vega ≈ 0, price ≈ intrinsic | Use wide Brent bracket |
| Deep OTM | Price ≈ 0, numerical noise | Return error or use min vol |
| Near Expiry | T → 0, d₁/d₂ → ±∞ | Special handling for T < 1 day |
| Zero Interest | r = 0 (crypto options) | Simplify formulas |
| High Vol | σ > 200% (distressed stocks) | Expand search range |

### Convergence Failure Handling

```cpp
enum class ConvergenceStatus {
    SUCCESS,
    MAX_ITERATIONS_REACHED,
    VEGA_TOO_SMALL,
    PRICE_OUT_OF_BOUNDS,
    NEGATIVE_VOLATILITY,
    ARBITRAGE_VIOLATION
};
```

### Input Validation

```cpp
void validate_option_spec(const OptionSpec& spec) {
    if (spec.spot <= 0) throw InvalidInput("Spot must be positive");
    if (spec.strike <= 0) throw InvalidInput("Strike must be positive");
    if (spec.time_to_expiry <= 0) throw InvalidInput("Time must be positive");
    // ... more checks
}
```

---

## 7. Advanced Features

### Volatility Smile Calculation

**Goal**: Compute implied vol for multiple strikes at fixed maturity

```cpp
struct VolSmile {
    std::vector<double> strikes;
    std::vector<double> implied_vols;
    std::vector<ConvergenceStatus> statuses;
};

VolSmile compute_vol_smile(
    double spot,
    const std::vector<double>& strikes,
    const std::vector<double>& market_prices,
    double time_to_expiry,
    double risk_free_rate
);
```

**Optimization**: Use previous strike's vol as initial guess for next strike

### Put-Call Parity Check

**Theory**:
```
C - P = S - K·e^(-rT)
```

**Implementation**:
```cpp
bool verify_put_call_parity(
    double call_price,
    double put_price,
    const OptionSpec& spec,
    double tolerance = 1e-4
) {
    double lhs = call_price - put_price;
    double rhs = spec.spot - spec.strike * exp(-spec.risk_free_rate * spec.time_to_expiry);
    return abs(lhs - rhs) < tolerance;
}
```

**Use Case**: Detect arbitrage or data errors in market quotes

### Volatility Surface Interpolation

**2D Grid**: (Strike, Maturity) → Implied Vol

**Methods**:
- Bilinear interpolation (simple)
- Cubic spline (smooth)
- SVI parameterization (no-arbitrage constraints)

### Performance Optimizations

1. **Vectorization**: Use SIMD for batch calculations
2. **Lookup Tables**: Pre-compute Φ(x) for common values
3. **Parallel Processing**: OpenMP for smile calculations
4. **Caching**: Store Vega alongside price to avoid recomputation

---

## Implementation Philosophy

### "Built from Scratch, Not Assembled"

**What This Means**:
- Implement normal CDF/PDF from first principles (not `<random>`)
- Write Newton-Raphson loop explicitly (not generic optimizer)
- Understand every numerical constant (e.g., why tolerance = 1e-6?)

**Why It Matters**:
- **Interviews**: Demonstrates deep understanding vs. library usage
- **Debugging**: Can trace through every calculation step
- **Customization**: Easy to add exotic features (American options, dividends)
- **Performance**: Eliminate abstraction overhead

### Engineering Rigor

1. **Unit Tests**: Every function has edge case tests
2. **Benchmarks**: Compare against QuantLib for accuracy
3. **Documentation**: Doxygen comments with math formulas
4. **Error Handling**: No silent failures, informative exceptions
5. **Code Review Ready**: Clean, idiomatic C++17/20

