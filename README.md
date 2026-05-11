# MC-Pricer
 
A Monte Carlo options pricing engine written in C++. It Prices five types of options via an interactive CLI, computes Black-Scholes Greeks, and supports antithetic variance reduction as a command-line flag.
 
---
 
## Table of Contents
 
- [What is an Option?](#what-is-an-option)
- [What is Monte Carlo Pricing?](#what-is-monte-carlo-pricing)
- [Geometric Brownian Motion](#geometric-brownian-motion)
- [The Box-Muller Transform](#the-box-muller-transform)
- [Supported Option Types](#supported-option-types)
- [Greeks](#greeks)
- [Variance Reduction](#variance-reduction)
- [Building](#building)
- [Usage](#usage)
- [Project Structure](#project-structure)
---
 
## What is an Option? 
 
A financial option is a contract that gives the buyer the right, but not the obligation, to buy or sell an underlying asset at a specified price (the strike, K) on or before a given date (the expiry, T).
 
A Call option profits when the asset price rises above the strike. A Put option profits when it falls below.
 
The core inputs shared by all option types:
 
| Parameter | Symbol | Description |
|---|---|---|
| Spot Price | S | Current price of the underlying asset |
| Strike Price | K | Agreed price at which the option can be exercised |
| Risk-Free Rate | r | Annualised continuously compounded interest rate |
| Volatility | σ (vol) | Annualised standard deviation of log returns |
| Time to Expiry | T | Duration of the option in years |
| Simulations | N | Number of Monte Carlo paths |
 
---
 
## What is Monte Carlo Pricing?
 
Monte Carlo pricing simulates thousands of possible future price paths for the underlying asset, computes the option payoff at the end of each path, and averages those payoffs. The result is discounted back to the present using the risk-free rate:
 
```
Price = e^(-rT) * (1/N) * Σ payoff(i)
```
 
The law of large numbers guarantees convergence i.e. the more paths you simulate, the closer the estimate gets to the true theoretical price. At N = 1,000,000 the error is typically within ±0.02 of the Black-Scholes closed form for European options.
 
---
 
## Geometric Brownian Motion
 
Each simulated price path follows Geometric Brownian Motion (GBM), the standard model for stock price evolution. For a single time step of size dt:
 
```
S(t + dt) = S(t) * exp((r - 0.5 * σ²) * dt + σ * √dt * Z)
```
 
Where Z is a standard normal random variable N(0,1). For path-dependent options (Asian, Barrier, Lookback) this is applied repeatedly with `dt = T / numSteps`, building up a full price path step by step. For European and Binary options a single step with `dt = T` is sufficient since only the terminal price matters.
 
---
 
## The Box-Muller Transform
 
Generating standard normal random variables efficiently is central to the simulation. This engine uses the Box-Muller transform, which converts two independent uniform random variables into a standard normal:
 
Given U1, U2 ~ Uniform(0, 1):
 
```
Z = sqrt(-2 * ln(U1)) * cos(2π * U2)
```
 
This works by interpreting (U1, U2) as polar coordinates and mapping them to a point on a 2D Gaussian distribution. The result is an exact standard normal sample — not an approximation — making it both efficient and numerically clean. The transform naturally produces two independent normals per call; this engine uses one (Z) for the simulation and negates it (-Z) for antithetic variance reduction.
 
---
 
## Supported Option Types
 
### European Option
 
The standard option. Payoff depends only on the terminal price S_T:
 
```
Call payoff = max(S_T - K, 0)
Put  payoff = max(K - S_T, 0)
```
 
Has a Black-Scholes closed form used for validation.
 
---
 
### Binary (Digital) Option
 
Pays a fixed amount of 1 if the option finishes in the money, otherwise 0:
 
```
Call payoff = 1  if S_T > K,  else 0
Put  payoff = 1  if S_T < K,  else 0
```
 
The closed-form price is the discounted risk-neutral probability of finishing in the money:
 
```
Binary Call = e^(-rT) * N(d2)
Binary Put  = e^(-rT) * N(-d2)
 
where d2 = (ln(S/K) + (r - 0.5σ²)T) / (σ√T)
```
 
The price per unit of payout is a pure probability , it tells you what the market believes the chance of the option expiring in the money is, discounted to today.
 
---
 
### Asian Option
 
Payoff depends on the arithmetic average of the asset price sampled at each time step over the option's life, rather than the terminal price. This smooths out the effect of any single day's price:
 
```
avg = (1/numSteps) * Σ S(t_i)
 
Call payoff = max(avg - K, 0)
Put  payoff = max(K - avg, 0)
```
 
No closed form exists for the arithmetic Asian. Th Monte Carlo is the standard pricing method. Asian options are typically cheaper than Europeans because averaging reduces the effective volatility of the payoff. Requires `numSteps` as an additional input (12 = monthly, 52 = weekly, 252 = daily).
 
---
 
### Barrier Option
 
A European-style option that is knocked out if the asset price crosses a barrier level B at any point during the path:
 
```
Knock-Out Call: pays max(S_T - K, 0) unless S ever crosses B upward  → pays 0
Knock-Out Put:  pays max(K - S_T, 0) unless S ever crosses B downward → pays 0
```
 
The barrier is checked at every time step. Barrier options are always cheaper than equivalent Europeans. The knock-out condition removes scenarios where the option would have paid out. Requires `numSteps` and a `barrier` level as additional inputs.
 
---
 
### Lookback Option
 
Payoff depends on the maximum or minimum price observed over the entire path, giving the holder perfect hindsight on entry or exit:
 
```
Lookback Call: pays S_T - min(S over path)    (best possible entry price)
Lookback Put:  pays max(S over path) - S_T    (best possible exit price)
```
 
Lookbacks are the most expensive option type , the holder always receives the best possible outcome in hindsight. Requires `numSteps`.
 
---
 
## Greeks
 
Greeks measure the sensitivity of the option price to changes in its inputs. Computed using Black-Scholes closed-form formulae from d1 and d2:
 
```
d1 = (ln(S/K) + (r + 0.5σ²)T) / (σ√T)
d2 = d1 - σ√T
```
 
| Greek | Measures | Call | Put |
|---|---|---|---|
| Delta (Δ) | Sensitivity to spot price | N(d1) | N(d1) - 1 |
| Gamma (Γ) | Rate of change of delta | pdf(d1) / (S·σ·√T) | same as call |
| Vega (ν) | Sensitivity to volatility | S·pdf(d1)·√T / 100 | same as call |
| Theta (Θ) | Time decay per day | -(S·pdf(d1)·σ/2√T + r·K·e^(-rT)·N(d2)) / 365 | sign flips on second term |
| Rho (ρ) | Sensitivity to interest rate | K·T·e^(-rT)·N(d2) / 100 | negative, uses N(-d2) |
 
Gamma and Vega are identical for calls and puts with the same parameters.
 
---
 
## Variance Reduction
 
Monte Carlo pricing converges at a rate of 1/√N , to halve the error you need four times the simulations. Antithetic variates is a variance reduction technique that cuts the standard error roughly in half with almost no extra computation.
 
For every random draw Z used to simulate a path, the technique also simulates a second path using -Z. Since Z and -Z are perfectly negatively correlated, their payoffs partially cancel each other's noise:
 
```
Standard:    payoff = f(Z)
Antithetic:  payoff = 0.5 * (f(Z) + f(-Z))
```
 
When Z produces an unusually high price path, -Z produces a symmetrically low one. The average of the two is closer to the true expected value than either alone, reducing variance without introducing any bias.
 
Enable via the `--variance-reduction` flag:
 
```bash
./MC_Pricer --variance-reduction
```
 
Implemented for all five option types. The antithetic path uses the same Box-Muller sample negated — no extra RNG calls are required.
 
---
 
## Building
 
**Requirements:** C++17, CMake 3.15+, GCC or Clang.
 
```bash
git clone https://github.com/timmyacy/MC-Pricer.git
cd MC-Pricer
mkdir build && cd build
cmake ..
make
```
 
 
## Usage
 
Standard run:
 
```bash
./MC_Pricer
```
 
With antithetic variance reduction:
 
```bash
./MC_Pricer --variance-reduction
```
 
The CLI uses arrow keys to navigate menus:
 
```
Select option type:
 
 > Asian Option
   Barrier Option
   Binary Option
   European Option
   Lookback Option
```
 
After selecting option type and call/put, enter the parameters when prompted. The engine prints a formatted results table:
 
```
-----------------------------------
Metric                       Value
-----------------------------------
 
Inputs:
Spot Price                     100
Strike                         100
Time (years)                     1
Volatility                     0.2
Risk Free Rate                0.05
Simulations                100000
 
-----------------------------------
 
Results:
Call Price                10.4506
 
-----------------------------------
 
Greeks:
Delta                      0.6368
Gamma                      0.0188
Vega                       0.3752
Theta                     -0.0277
Rho                        0.5323
-----------------------------------
```
 
 
 
## Project Structure
 
```
MC-Pricer/
├── main.cpp                  Entry point, CLI flag parsing, pricer dispatch
├── CMakeLists.txt
├── include/
│   ├── model.h               MCParams, Greeks, PrintData, enums
│   ├── option_pricer.h       Abstract base class with virtual price()
│   ├── european_pricer.h
│   ├── binary_pricer.h
│   ├── asian_pricer.h
│   ├── barrier_pricer.h
│   ├── lookback_pricer.h
│   └── ui.h                  CLI input/output declarations
└── src/
    ├── option_pricer.cpp     Base class constructor + Box-Muller RNG
    ├── model.cpp             d1/d2, Greeks computation, printResults
    ├── european_pricer.cpp
    ├── binary_pricer.cpp
    ├── asian_pricer.cpp
    ├── barrier_pricer.cpp
    ├── lookback_pricer.cpp
    └── ui.cpp                enterValues, selectFromMenu
```
 
All pricers inherit from `OptionPricer` and override the pure virtual `price()` method. Variance reduction is toggled via `setVarianceReduction()` on the base class which is a single call in `main` applies to whichever subclass is constructed at runtime, with no changes needed in any subclass.
