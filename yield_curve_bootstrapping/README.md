# Zero-Coupon Yield Curve Bootstrapping: From Bond Prices to Interest Rate Curves

## Table of Contents
1. [Financial Context: Fixed Income Trading](#financial-context)
2. [The Bootstrapping Problem](#the-problem)
3. [Mathematical Foundations](#mathematical-foundations)
4. [Numerical Methods](#numerical-methods)
5. [C++ Architecture Design](#cpp-architecture)
6. [Curve Interpolation & Smoothing](#interpolation)
7. [Advanced Features](#advanced-features)

---

## 1. Financial Context: Fixed Income Trading

### What You Encounter in Practice

In fixed income markets, you work with:

- **Bond Prices**: Market quotes for government/corporate bonds with different maturities
- **Yield Curves**: The relationship between interest rates and time to maturity
- **Discount Factors**: Present value of $1 received at future date
- **Forward Rates**: Expected future interest rates implied by current curve
- **Curve Construction**: Building a consistent zero-coupon curve from coupon-bearing bonds

### Why Bootstrapping Matters

**The Core Problem**:
- Market quotes coupon-bearing **bond prices** (e.g., 2-year Treasury at $98.50)
- Traders need **zero-coupon rates** to price derivatives, value portfolios, and manage risk
- You must extract the zero-coupon curve from observed bond prices

**Real-World Applications**:
1. **Derivatives Pricing**: Discount cash flows for swaps, options, futures
2. **Risk Management**: Calculate duration, convexity, DV01
3. **Relative Value Trading**: Identify mispriced bonds
4. **Portfolio Valuation**: Mark-to-market fixed income positions
5. **Regulatory Reporting**: Basel III, Solvency II requirements

---

## 2. The Bootstrapping Problem

### Problem Statement

**Given**:
- Set of bonds with market prices: `{P₁, P₂, ..., Pₙ}`
- Bond specifications: coupon rates, payment frequencies, maturities
- Each bond pays coupons at times `{t₁, t₂, ..., tₘ}`

**Find**:
- Zero-coupon discount factors: `D(t)` for all payment times
- Zero-coupon rates: `r(t)` such that `D(t) = e^(-r(t)·t)` (continuous compounding)

**Constraint**:
```
P_bond = Σ(C_i · D(t_i)) + FV · D(T)
```
Where:
- `C_i` = coupon payment at time `t_i`
- `FV` = face value (typically 100)
- `D(t_i)` = discount factor at time `t_i`

### Why It's Non-Trivial

1. **Recursive Nature**: Short-term rates needed to solve for long-term rates
2. **Mixed Equation System**: Linear in discount factors, nonlinear in rates
3. **Interpolation Required**: Market doesn't quote every maturity
4. **Numerical Stability**: Small price errors can cause large rate errors
5. **Curve Smoothness**: Economic rates should be continuous and smooth

---

## 3. Mathematical Foundations

### Discount Factors and Zero Rates

**Discount Factor**:
```
D(t) = Present value of $1 at time t
```

**Zero-Coupon Rate** (different conventions):

1. **Continuous Compounding**:
   ```
   D(t) = e^(-r(t)·t)
   r(t) = -ln(D(t)) / t
   ```

2. **Annual Compounding**:
   ```
   D(t) = 1 / (1 + r(t))^t
   r(t) = D(t)^(-1/t) - 1
   ```

3. **Semi-Annual Compounding** (US Treasuries):
   ```
   D(t) = 1 / (1 + r(t)/2)^(2t)
   r(t) = 2 · (D(t)^(-1/(2t)) - 1)
   ```

### Bond Pricing Equation

For a coupon-bearing bond:
```
P = Σ(C · D(t_i)) + 100 · D(T)

Where:
  C = coupon payment (e.g., 5% annual → $5 per year)
  t_i = coupon payment times
  T = maturity
  D(t) = discount factor
```

### Forward Rates

**Instantaneous Forward Rate**:
```
f(t) = -∂ln(D(t))/∂t = r(t) + t · ∂r(t)/∂t
```

**Discrete Forward Rate** (from t₁ to t₂):
```
f(t₁, t₂) = [ln(D(t₁)) - ln(D(t₂))] / (t₂ - t₁)
```

**Economic Interpretation**: The forward rate is the interest rate implied today for borrowing/lending between two future dates.

---

## 4. Numerical Methods

### Bootstrapping Algorithm

**Step-by-Step Process**:

1. **Sort bonds by maturity** (shortest to longest)

2. **Bootstrap shortest maturity** (e.g., 3-month T-bill):
   ```
   P = 100 · D(T)
   D(T) = P / 100
   r(T) = -ln(D(T)) / T
   ```

3. **Bootstrap next maturity** (e.g., 6-month bond with coupon):
   ```
   P = C · D(t₁) + (C + 100) · D(t₂)
   
   Known: D(t₁) from previous step
   Solve for: D(t₂)
   
   D(t₂) = [P - C · D(t₁)] / (C + 100)
   ```

4. **Repeat recursively** for all maturities

**Key Insight**: Each new bond adds one equation with one unknown (the next discount factor), making the system solvable sequentially.

### Handling Coupon Bonds

For a bond with multiple coupons:
```
P = C·D(0.5) + C·D(1.0) + C·D(1.5) + (C+100)·D(2.0)

If D(0.5), D(1.0), D(1.5) are known:
  D(2.0) = [P - C·(D(0.5) + D(1.0) + D(1.5))] / (C + 100)
```

### Interpolation Methods

When discount factors are needed at times not directly observed:

#### 1. Piecewise Constant (Flat Forward)
```
For t ∈ [t_i, t_{i+1}]:
  D(t) = D(t_i) · exp(-f_i · (t - t_i))
  
Where f_i is constant forward rate between t_i and t_{i+1}
```

#### 2. Linear Interpolation (on rates)
```
For t ∈ [t_i, t_{i+1}]:
  r(t) = r(t_i) + (r(t_{i+1}) - r(t_i)) · (t - t_i) / (t_{i+1} - t_i)
```

#### 3. Linear Interpolation (on log discount factors)
```
For t ∈ [t_i, t_{i+1}]:
  ln(D(t)) = ln(D(t_i)) + [ln(D(t_{i+1})) - ln(D(t_i))] · (t - t_i) / (t_{i+1} - t_i)
```

**Advantages**:
- Piecewise constant: Simple, no arbitrage
- Linear on rates: Intuitive, smooth rates
- Linear on log(D): Smooth discount factors, positive rates guaranteed

### Cubic Spline Smoothing

For smoother curves, fit cubic spline to zero rates:
```
r(t) = a_i + b_i·(t - t_i) + c_i·(t - t_i)² + d_i·(t - t_i)³

Constraints:
  - Passes through all bootstrapped points
  - First derivative continuous (smooth)
  - Second derivative continuous (no kinks)
  - Natural boundary conditions: r''(t_0) = r''(t_n) = 0
```

**Tridiagonal System**:
```
[2  1  0  ...  0 ] [c_0]   [b_1 - b_0]
[1  4  1  ...  0 ] [c_1] = [b_2 - b_1]
[0  1  4  ...  0 ] [c_2]   [b_3 - b_2]
[...            ] [...]   [...]
[0  ...  1  4  1] [c_n]   [b_n - b_{n-1}]
```

Solved efficiently with Thomas algorithm (O(n)).

---

## 5. C++ Architecture Design

### Design Principles

1. **Separation of Concerns**: Bond data, discount factors, interpolation separate
2. **Flexible Compounding**: Support multiple rate conventions
3. **Pluggable Interpolation**: Easy to swap interpolation methods
4. **Numerical Stability**: Careful handling of near-zero rates
5. **Performance**: Efficient spline solving, minimal allocations

### Module Structure

```
yield_curve_bootstrapping/
├── include/
│   ├── bond_types.hpp           # Bond specifications, market data
│   ├── discount_factor.hpp      # Discount factor calculations
│   ├── interpolation.hpp        # Interpolation methods
│   ├── bootstrapper.hpp         # Main bootstrapping engine
│   ├── cubic_spline.hpp         # Cubic spline implementation
│   └── forward_curve.hpp        # Forward rate calculations
├── src/
│   ├── bond_types.cpp
│   ├── discount_factor.cpp
│   ├── interpolation.cpp
│   ├── bootstrapper.cpp
│   ├── cubic_spline.cpp
│   ├── forward_curve.cpp
│   └── main.cpp                 # Demo & benchmarks
├── tests/
│   ├── test_discount_factor.cpp
│   ├── test_interpolation.cpp
│   ├── test_bootstrapper.cpp
│   └── test_cubic_spline.cpp
└── CMakeLists.txt
```

### Core Classes/Structs

#### BondData
```cpp
struct BondData {
    double maturity;           // Years to maturity
    double coupon_rate;        // Annual coupon rate (e.g., 0.05 for 5%)
    int payment_frequency;     // Payments per year (1, 2, 4)
    double market_price;       // Clean price (per 100 face value)
    double face_value;         // Typically 100
};
```

#### DiscountFactor
```cpp
class DiscountFactor {
public:
    enum class CompoundingType {
        CONTINUOUS,
        ANNUAL,
        SEMI_ANNUAL,
        QUARTERLY
    };
    
    double discount_factor(double time, double zero_rate, CompoundingType type) const;
    double zero_rate(double time, double discount_factor, CompoundingType type) const;
};
```

#### YieldCurve
```cpp
class YieldCurve {
public:
    void add_point(double time, double discount_factor);
    double get_discount_factor(double time) const;
    double get_zero_rate(double time, CompoundingType type) const;
    double get_forward_rate(double t1, double t2) const;
    
private:
    std::vector<double> times_;
    std::vector<double> discount_factors_;
    std::unique_ptr<Interpolator> interpolator_;
};
```

#### Bootstrapper
```cpp
class Bootstrapper {
public:
    Bootstrapper(CompoundingType type, InterpolationType interp_type);
    
    YieldCurve bootstrap(const std::vector<BondData>& bonds);
    
    YieldCurve bootstrap_with_spline_smoothing(
        const std::vector<BondData>& bonds,
        double smoothing_parameter = 0.0
    );
    
private:
    double solve_for_discount_factor(
        const BondData& bond,
        const YieldCurve& partial_curve
    );
    
    bool validate_inputs(const std::vector<BondData>& bonds) const;
};
```

### Interpolation Interface

```cpp
class Interpolator {
public:
    virtual ~Interpolator() = default;
    virtual double interpolate(double t, const std::vector<double>& times,
                              const std::vector<double>& values) const = 0;
};

class LinearInterpolator : public Interpolator { /* ... */ };
class LogLinearInterpolator : public Interpolator { /* ... */ };
class CubicSplineInterpolator : public Interpolator { /* ... */ };
```

---

## 6. Curve Interpolation & Smoothing

### Interpolation Comparison

| Method | Smoothness | Arbitrage-Free | Complexity | Use Case |
|--------|-----------|----------------|------------|----------|
| Piecewise Constant | Step function | Yes | O(log n) | Simple, conservative |
| Linear (rates) | C⁰ | No | O(log n) | Quick analysis |
| Log-Linear (DF) | C⁰ | Yes | O(log n) | Standard practice |
| Cubic Spline | C² | No* | O(n) | Smooth curves, research |

*Can be made arbitrage-free with constraints

### Cubic Spline Implementation

**Natural Cubic Spline Algorithm**:

1. **Setup**: Given points `(t_i, r_i)` for i = 0, 1, ..., n

2. **Compute second derivatives** `M_i` by solving:
   ```
   h_{i-1}·M_{i-1} + 2(h_{i-1} + h_i)·M_i + h_i·M_{i+1} = 
       6[(r_{i+1} - r_i)/h_i - (r_i - r_{i-1})/h_{i-1}]
   
   Where h_i = t_{i+1} - t_i
   ```

3. **Boundary conditions**: `M_0 = M_n = 0` (natural spline)

4. **Interpolate** for t ∈ [t_i, t_{i+1}]:
   ```
   r(t) = M_i·(t_{i+1} - t)³/(6h_i) + M_{i+1}·(t - t_i)³/(6h_i)
        + [r_i - M_i·h_i²/6]·(t_{i+1} - t)/h_i
        + [r_{i+1} - M_{i+1}·h_i²/6]·(t - t_i)/h_i
   ```

**Thomas Algorithm** (tridiagonal solver):
```cpp
// Forward elimination
for (int i = 1; i < n; ++i) {
    double m = a[i] / b[i-1];
    b[i] = b[i] - m * c[i-1];
    d[i] = d[i] - m * d[i-1];
}

// Back substitution
x[n-1] = d[n-1] / b[n-1];
for (int i = n-2; i >= 0; --i) {
    x[i] = (d[i] - c[i] * x[i+1]) / b[i];
}
```

---

## 7. Advanced Features

### Forward Curve Construction

**Instantaneous Forward Rate**:
```cpp
double instantaneous_forward(double t, double dt = 1e-6) {
    double D_t = get_discount_factor(t);
    double D_t_dt = get_discount_factor(t + dt);
    return -log(D_t_dt / D_t) / dt;
}
```

**Discrete Forward Rate**:
```cpp
double forward_rate(double t1, double t2) {
    double D1 = get_discount_factor(t1);
    double D2 = get_discount_factor(t2);
    return -log(D2 / D1) / (t2 - t1);
}
```

### Par Yield Calculation

**Par Yield**: Coupon rate that makes bond price = 100

```cpp
double par_yield(double maturity, int frequency) {
    double sum_df = 0.0;
    for (int i = 1; i <= maturity * frequency; ++i) {
        double t = i / static_cast<double>(frequency);
        sum_df += get_discount_factor(t);
    }
    double D_T = get_discount_factor(maturity);
    return frequency * (1.0 - D_T) / sum_df;
}
```

### Curve Sensitivity (DV01)

**DV01**: Change in curve value for 1bp parallel shift

```cpp
double calculate_dv01(const YieldCurve& curve) {
    double pv_base = calculate_portfolio_pv(curve);
    
    YieldCurve shifted_curve = curve;
    shifted_curve.parallel_shift(0.0001);  // 1bp = 0.01%
    
    double pv_shifted = calculate_portfolio_pv(shifted_curve);
    return pv_shifted - pv_base;
}
```

### Arbitrage Detection

Check for negative forward rates:
```cpp
bool has_arbitrage(const YieldCurve& curve) {
    for (size_t i = 0; i < curve.size() - 1; ++i) {
        double fwd = curve.get_forward_rate(
            curve.times()[i], 
            curve.times()[i+1]
        );
        if (fwd < 0) return true;
    }
    return false;
}
```

---

## Implementation Philosophy

### "Built from Scratch, Not Assembled"

**What This Means**:
- Implement cubic spline solver from first principles
- Write bootstrapping recursion explicitly
- Understand every interpolation formula
- No black-box optimization libraries

**Why It Matters**:
- **Interviews**: Shows deep understanding of numerical methods
- **Debugging**: Can trace through every calculation
- **Customization**: Easy to add exotic features (callable bonds, CDS)
- **Performance**: Eliminate abstraction overhead

### Engineering Rigor

1. **Unit Tests**: Every interpolation method, edge cases
2. **Benchmarks**: Compare against QuantLib for accuracy
3. **Documentation**: Doxygen with mathematical formulas
4. **Error Handling**: Validate inputs, detect arbitrage
5. **Code Quality**: Clean, idiomatic C++17

---

## Numerical Stability Considerations

### Common Pitfalls

1. **Division by Zero**: Near-zero time intervals
2. **Log of Negative**: Discount factors > 1 (arbitrage)
3. **Ill-Conditioned Systems**: Bonds with very similar maturities
4. **Extrapolation**: Rates beyond longest maturity

### Solutions

```cpp
// Safe discount factor calculation
double safe_discount_factor(double price, double coupons_pv) {
    double df = (price - coupons_pv) / 100.0;
    if (df <= 0 || df > 1.0) {
        throw ArbitrageException("Invalid discount factor");
    }
    return df;
}

// Safe rate calculation
double safe_zero_rate(double df, double time) {
    if (time < 1e-10) {
        throw InvalidInputException("Time too small");
    }
    if (df <= 0) {
        throw ArbitrageException("Non-positive discount factor");
    }
    return -log(df) / time;
}
```

---

## Performance Optimizations

1. **Binary Search**: O(log n) interpolation lookup
2. **Caching**: Store frequently accessed discount factors
3. **SIMD**: Vectorize spline evaluation
4. **Sparse Storage**: Only store curve pillars, interpolate on demand
5. **Parallel Bootstrap**: Independent bonds can be processed in parallel

---

## Real-World Extensions

### 1. Multi-Currency Curves
- Build separate curves for USD, EUR, GBP
- Handle cross-currency basis spreads

### 2. OIS Discounting
- Separate projection and discounting curves
- Post-2008 market practice

### 3. Credit Spreads
- Add credit spread to risk-free curve
- Bootstrap CDS curves

### 4. Callable Bonds
- Iterative bootstrapping with embedded options
- Requires option pricing model

### 5. Inflation Curves
- Real vs nominal rates
- Breakeven inflation calculation

