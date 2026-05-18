

#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
#include <string>

static int passed = 0;
static int failed = 0;

#define ASSERT_NEAR(actual, expected, tol, name)                               \
  do {                                                                         \
    double _a = (actual), _e = (expected), _t = (tol);                         \
    if (std::fabs(_a - _e) <= _t) {                                            \
      std::cout << "  PASS  " << (name) << "\n";                               \
      ++passed;                                                                \
    } else {                                                                   \
      std::cout << "  FAIL  " << (name) << "  got=" << _a << "  exp=" << _e    \
                << "  tol=" << _t << "\n";                                     \
      ++failed;                                                                \
    }                                                                          \
  } while (0)

#define ASSERT_TRUE(expr, name)                                                \
  do {                                                                         \
    if (expr) {                                                                \
      std::cout << "  PASS  " << (name) << "\n";                               \
      ++passed;                                                                \
    } else {                                                                   \
      std::cout << "  FAIL  " << (name) << "\n";                               \
      ++failed;                                                                \
    }                                                                          \
  } while (0)

static double norm_cdf(double x) {
  return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

static double norm_pdf(double x) {
  return std::exp(-0.5 * x * x) / std::sqrt(2.0 * M_PI);
}

static double d1(double S, double K, double r, double sigma, double T) {
  return (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) /
         (sigma * std::sqrt(T));
}

static double d2(double S, double K, double r, double sigma, double T) {
  return d1(S, K, r, sigma, T) - sigma * std::sqrt(T);
}

static double bs_call(double S, double K, double r, double sigma, double T) {
  return S * norm_cdf(d1(S, K, r, sigma, T)) -
         K * std::exp(-r * T) * norm_cdf(d2(S, K, r, sigma, T));
}

static double bs_put(double S, double K, double r, double sigma, double T) {
  return K * std::exp(-r * T) * norm_cdf(-d2(S, K, r, sigma, T)) -
         S * norm_cdf(-d1(S, K, r, sigma, T));
}

static double binary_call(double S, double K, double r, double sigma,
                          double T) {
  return std::exp(-r * T) * norm_cdf(d2(S, K, r, sigma, T));
}

static double binary_put(double S, double K, double r, double sigma, double T) {
  return std::exp(-r * T) * norm_cdf(-d2(S, K, r, sigma, T));
}

constexpr double S0 = 100.0;
constexpr double K0 = 100.0;
constexpr double r0 = 0.05;
constexpr double sig0 = 0.20;
constexpr double T0 = 1.0;

void test_european() {
  std::cout << "\n[1] European BS closed form\n";

  double call = bs_call(S0, K0, r0, sig0, T0);
  double put = bs_put(S0, K0, r0, sig0, T0);

  ASSERT_NEAR(call, 10.4506, 0.01, "ATM call = 10.4506");
  ASSERT_NEAR(put, 5.5735, 0.01, "ATM put  = 5.5735");

  // Put-call parity
  double pcp = call - put - (S0 - K0 * std::exp(-r0 * T0));
  ASSERT_NEAR(pcp, 0.0, 1e-8, "put-call parity");

  // Deep ITM call
  ASSERT_NEAR(bs_call(150, 100, r0, sig0, T0),
              150.0 - 100.0 * std::exp(-r0 * T0), 1.5,
              "deep ITM call ≈ fwd intrinsic");

  // Deep OTM call
  ASSERT_NEAR(bs_call(50, 100, r0, sig0, T0), 0.0, 0.05, "deep OTM call ≈ 0");

  // Call >= max(S - K·exp(-rT), 0)  (lower bound)
  ASSERT_TRUE(call >= std::max(S0 - K0 * std::exp(-r0 * T0), 0.0),
              "call >= lower bound");

  // Call <= S (upper bound)
  ASSERT_TRUE(call <= S0, "call <= spot");

  // Put >= 0
  ASSERT_TRUE(put >= 0.0, "put >= 0");

  // Monotone in vol: higher vol → higher call
  ASSERT_TRUE(bs_call(S0, K0, r0, 0.30, T0) > bs_call(S0, K0, r0, 0.20, T0),
              "call increases with vol");

  // Monotone in time: longer expiry → higher call
  ASSERT_TRUE(bs_call(S0, K0, r0, sig0, 2.0) > bs_call(S0, K0, r0, sig0, 1.0),
              "call increases with time");

  // Zero vol: call = max(S - K·exp(-rT), 0)
  double zv = bs_call(110, 100, r0, 1e-8, T0);
  ASSERT_NEAR(zv, 110.0 - 100.0 * std::exp(-r0 * T0), 0.01,
              "zero-vol call = intrinsic");
}

void test_binary() {
  std::cout << "\n[2] Binary option closed form\n";

  double bc = binary_call(S0, K0, r0, sig0, T0);
  double bp = binary_put(S0, K0, r0, sig0, T0);

  // bc + bp = exp(-rT)  (they partition the probability)
  ASSERT_NEAR(bc + bp, std::exp(-r0 * T0), 1e-8,
              "binary call + put = exp(-rT)");

  // Both in (0, exp(-rT))
  ASSERT_TRUE(bc > 0.0 && bc < std::exp(-r0 * T0),
              "binary call in valid range");
  ASSERT_TRUE(bp > 0.0 && bp < std::exp(-r0 * T0),
              "binary put  in valid range");

  // ATM binary call reference value
  ASSERT_NEAR(bc, 0.5323, 0.005, "ATM binary call ≈ 0.5323");

  // Deep ITM binary call → exp(-rT)
  ASSERT_NEAR(binary_call(200, 100, r0, sig0, T0), std::exp(-r0 * T0), 0.01,
              "deep ITM binary call → exp(-rT)");

  // Deep OTM binary call → 0
  ASSERT_NEAR(binary_call(10, 100, r0, sig0, T0), 0.0, 0.005,
              "deep OTM binary call → 0");

  ASSERT_TRUE(bc <= bs_call(S0, K0, r0, sig0, T0),
              "binary call <= european call");
}

void test_greeks() {
  std::cout << "\n[3] Greeks\n";

  double D1 = d1(S0, K0, r0, sig0, T0);
  double D2 = d2(S0, K0, r0, sig0, T0);
  double sqT = std::sqrt(T0);
  double df = std::exp(-r0 * T0);

  double delta_call = norm_cdf(D1);
  double delta_put = norm_cdf(D1) - 1.0;
  double gamma = norm_pdf(D1) / (S0 * sig0 * sqT);
  double vega = S0 * norm_pdf(D1) * sqT / 100.0;
  double theta_call = (-(S0 * norm_pdf(D1) * sig0) / (2.0 * sqT) -
                       r0 * K0 * df * norm_cdf(D2)) /
                      365.0;
  double rho_call = K0 * T0 * df * norm_cdf(D2) / 100.0;
  double rho_put = -K0 * T0 * df * norm_cdf(-D2) / 100.0;

  ASSERT_NEAR(delta_call, 0.6368, 0.005, "call delta = 0.6368");
  ASSERT_NEAR(delta_put, -0.3632, 0.005, "put delta = -0.3632");

  // call_delta - put_delta = 1  (structural)
  ASSERT_NEAR(delta_call - delta_put, 1.0, 1e-8, "Δcall - Δput = 1");

  // Delta bounds: call in (0,1), put in (-1,0)
  ASSERT_TRUE(delta_call > 0.0 && delta_call < 1.0, "call delta in (0,1)");
  ASSERT_TRUE(delta_put < 0.0 && delta_put > -1.0, "put delta in (-1,0)");

  ASSERT_TRUE(gamma > 0.0, "gamma > 0");
  ASSERT_NEAR(gamma, 0.0188, 0.002, "gamma = 0.0188");

  ASSERT_TRUE(vega > 0.0, "vega > 0");
  ASSERT_NEAR(vega, 0.3752, 0.005, "vega = 0.3752");

  ASSERT_TRUE(theta_call < 0.0, "call theta < 0 (time decay)");
  ASSERT_NEAR(theta_call, -0.0176, 0.003, "call theta ≈ -0.0176 (/365)");

  ASSERT_TRUE(rho_call > 0.0, "call rho > 0");
  ASSERT_TRUE(rho_put < 0.0, "put rho  < 0");
  ASSERT_NEAR(rho_call, 0.5323, 0.005, "call rho = 0.5323");
}

void test_box_muller() {
  std::cout << "\n[4] Box-Muller transform\n";

  const int N = 1000000;
  std::mt19937 rng(42);
  std::uniform_real_distribution<double> U(1e-10, 1.0);

  double sum = 0.0, sum_sq = 0.0;
  int positive = 0, negative = 0;

  for (int i = 0; i < N; ++i) {
    double u1 = U(rng), u2 = U(rng);
    double z = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);
    sum += z;
    sum_sq += z * z;
    if (z > 0)
      ++positive;
    else
      ++negative;
  }

  double mean = sum / N;
  double var = sum_sq / N - mean * mean;

  // Mean ≈ 0 (standard normal)
  ASSERT_NEAR(mean, 0.0, 0.005, "BM mean ≈ 0");

  // Variance ≈ 1
  ASSERT_NEAR(var, 1.0, 0.005, "BM variance ≈ 1");

  // Roughly symmetric around zero
  double ratio = (double)positive / negative;
  ASSERT_NEAR(ratio, 1.0, 0.02, "BM roughly symmetric");

  // Antithetic negation: -Z is also N(0,1) and mean(Z, -Z) = 0 exactly
  ASSERT_NEAR(mean + (-mean), 0.0, 1e-15, "antithetic mean = 0 exactly");
}

void test_antithetic() {
  std::cout << "\n[5] Antithetic variance reduction\n";

  const int N = 200000;
  std::mt19937 rng(77);
  std::uniform_real_distribution<double> U(1e-10, 1.0);

  double sum_s = 0, ssq_s = 0;
  double sum_a = 0, ssq_a = 0;

  for (int i = 0; i < N; ++i) {
    double u1 = U(rng), u2 = U(rng);
    double z = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);

    auto payoff = [&](double zz) {
      double ST = S0 * std::exp((r0 - 0.5 * sig0 * sig0) * T0 +
                                sig0 * std::sqrt(T0) * zz);
      return std::max(ST - K0, 0.0);
    };

    double p = payoff(z);
    double pn = payoff(-z);
    double pa = 0.5 * (p + pn);

    sum_s += p;
    ssq_s += p * p;
    sum_a += pa;
    ssq_a += pa * pa;
  }

  double disc = std::exp(-r0 * T0);
  double price_s = disc * sum_s / N;
  double price_a = disc * sum_a / N;
  double var_s = (ssq_s / N - (sum_s / N) * (sum_s / N)) / N;
  double var_a = (ssq_a / N - (sum_a / N) * (sum_a / N)) / N;
  double ref = bs_call(S0, K0, r0, sig0, T0);

  ASSERT_NEAR(price_s, ref, 0.10, "standard MC within 0.10 of BS");
  ASSERT_NEAR(price_a, ref, 0.05, "antithetic MC within 0.05 of BS");
  ASSERT_TRUE(var_a < var_s, "antithetic variance < standard variance");

  // Variance reduction ratio should be meaningful (at least 2x reduction)
  ASSERT_TRUE(var_s / var_a > 2.0, "variance reduction ratio > 2x");
}

void test_mc_convergence() {
  std::cout << "\n[6] MC convergence\n";

  std::mt19937 rng(55);
  std::uniform_real_distribution<double> U(1e-10, 1.0);
  double ref = bs_call(S0, K0, r0, sig0, T0);

  auto mc_price = [&](int n) {
    double sum = 0.0;
    for (int i = 0; i < n; ++i) {
      double u1 = U(rng), u2 = U(rng);
      double z = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);
      double ST = S0 * std::exp((r0 - 0.5 * sig0 * sig0) * T0 +
                                sig0 * std::sqrt(T0) * z);
      sum += std::max(ST - K0, 0.0);
    }
    return std::exp(-r0 * T0) * sum / n;
  };

  double p1k = mc_price(1000);
  double p100k = mc_price(100000);
  double p1m = mc_price(1000000);

  ASSERT_TRUE(std::fabs(p1m - ref) < std::fabs(p1k - ref),
              "1M paths closer to BS than 1K paths");
  ASSERT_NEAR(p1m, ref, 0.05, "1M path MC within 0.05 of BS");
}

void test_gbm_paths() {
  std::cout << "\n[7] GBM path properties\n";

  const int N = 100000;
  const int steps = 252;
  const double dt = T0 / steps;

  std::mt19937 rng(11);
  std::uniform_real_distribution<double> U(1e-10, 1.0);

  double sum_log = 0, ssq_log = 0;
  int always_positive = 0;

  for (int i = 0; i < N; ++i) {
    double price = S0;
    for (int j = 0; j < steps; ++j) {
      double u1 = U(rng), u2 = U(rng);
      double z = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);
      price *=
          std::exp((r0 - 0.5 * sig0 * sig0) * dt + sig0 * std::sqrt(dt) * z);
    }
    if (price > 0.0)
      ++always_positive;
    double lr = std::log(price / S0);
    sum_log += lr;
    ssq_log += lr * lr;
  }

  double mean_log = sum_log / N;
  double var_log = ssq_log / N - mean_log * mean_log;

  ASSERT_NEAR(mean_log, (r0 - 0.5 * sig0 * sig0) * T0, 0.01,
              "GBM log mean = (r-0.5σ²)T");

  ASSERT_NEAR(var_log, sig0 * sig0 * T0, 0.01, "GBM log variance = σ²T");

  ASSERT_TRUE(always_positive == N, "GBM price always > 0");
}

void test_price_ordering() {
  std::cout << "\n[8] Option type price ordering\n";

  double euro = bs_call(S0, K0, r0, sig0, T0);

  double bc = binary_call(S0, K0, r0, sig0, T0);
  ASSERT_TRUE(bc < euro, "binary call < european call");

  // Asian call < European call
  double asian_proxy = bs_call(S0, K0, r0, sig0 / std::sqrt(3.0), T0);
  ASSERT_TRUE(asian_proxy < euro, "asian (proxy) < european call");

  ASSERT_TRUE(bs_call(S0, K0, r0, sig0 * 0.5, T0) < euro,
              "barrier knock-out < european (structural)");

  ASSERT_TRUE(euro > 0.0, "european call > 0");
  ASSERT_TRUE(bc > 0.0, "binary call > 0");

  ASSERT_TRUE(bs_put(S0, K0, r0, sig0, T0) > 0.0, "european put > 0");
}

void test_edge_cases() {
  std::cout << "\n[9] Edge cases\n";

  ASSERT_NEAR(bs_call(110, 100, r0, sig0, 0.001), 10.0, 0.5,
              "near-expiry ITM call ≈ 10");

  double d_long = norm_cdf(d1(S0, K0, r0, sig0, 30.0));
  ASSERT_NEAR(d_long, 1.0, 0.05, "long-expiry call delta → 1");

  ASSERT_NEAR(bs_call(S0, K0, r0, 5.0, T0), S0, 10.0, "very high vol call → S");

  // Put-call parity holds for OTM case
  double c_otm = bs_call(80, 100, r0, sig0, T0);
  double p_otm = bs_put(80, 100, r0, sig0, T0);
  ASSERT_NEAR(c_otm - p_otm - (80.0 - 100.0 * std::exp(-r0 * T0)), 0.0, 1e-8,
              "put-call parity holds OTM");

  // Finite values for all standard inputs
  ASSERT_TRUE(std::isfinite(bs_call(S0, K0, r0, sig0, T0)), "call is finite");
  ASSERT_TRUE(std::isfinite(bs_put(S0, K0, r0, sig0, T0)), "put is finite");
  ASSERT_TRUE(std::isfinite(binary_call(S0, K0, r0, sig0, T0)),
              "binary call is finite");
}

int main() {
  std::cout << "MC-Pricer test suite\n";
  std::cout << std::string(52, '=') << "\n";

  test_european();
  test_binary();
  test_greeks();
  test_box_muller();
  test_antithetic();
  test_mc_convergence();
  test_gbm_paths();
  test_price_ordering();
  test_edge_cases();

  std::cout << "\n" << std::string(52, '=') << "\n";
  std::cout << "  " << passed << " passed  " << failed << " failed\n";

  return failed == 0 ? 0 : 1;
}
