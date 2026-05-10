#pragma once
#include "model.h"
#include <cmath>
#include <iomanip>
#include <iostream>
using namespace std;

Greeks computeGreeks(const MCParams &params, Option option) {
  Greeks g;
  double d1 = calculate_d1(params);
  double d2 = calculate_d2(d1, params);
  double df = exp(-params.r * params.T);

  g.gamma = pdf(d1) / (params.S * params.vol * sqrt(params.T));
  g.vega = params.S * pdf(d1) * sqrt(params.T) / 100.0;

  if (option == CALL) {
    g.delta = cdf(d1);
    g.theta = (-(params.S * pdf(d1) * params.vol) / (2.0 * sqrt(params.T)) -
               params.r * params.K * df * cdf(d2)) /
              365.0;
    g.rho = params.K * params.T * df * cdf(d2) / 100.0;
  } else {
    g.delta = cdf(d1) - 1.0;
    g.theta = (-(params.S * pdf(d1) * params.vol) / (2.0 * sqrt(params.T)) +
               params.r * params.K * df * cdf(-d2)) /
              365.0;
    g.rho = -(params.K * params.T * df * cdf(-d2)) / 100.0;
  }

  return g;
}

double calculate_d1(const MCParams &params) {
  return (log(params.S / params.K) +
          (params.r + 0.5 * params.vol * params.vol) * params.T) /
         (params.vol * sqrt(params.T));
}

double calculate_d2(double d1, const MCParams &params) {
  return d1 - params.vol * sqrt(params.T);
}
double pdf(double x) { return exp(-0.5 * x * x) / sqrt(2 * M_PI); }

double cdf(double x) { return erfc(-x / sqrt(2)) / 2; }

void printResults(const PrintData &data) {
  cout << string(35, '-') << "\n";
  cout << left << setw(20) << "Metric" << right << setw(12) << "Value" << "\n";
  cout << string(35, '-') << "\n";

  cout << "\nInputs:\n";
  cout << left << setw(20) << "Spot Price" << right << setw(12) << data.params.S
       << "\n";
  cout << left << setw(20) << "Strike" << right << setw(12) << data.params.K
       << "\n";
  cout << left << setw(20) << "Time (years)" << right << setw(12)
       << data.params.T << "\n";
  cout << left << setw(20) << "Volatility" << right << setw(12)
       << data.params.vol << "\n";
  cout << left << setw(20) << "Risk Free Rate" << right << setw(12)
       << data.params.r << "\n";
  cout << left << setw(20) << "Simulations" << right << setw(12)
       << data.params.N << "\n";

  if (data.hasSteps)
    cout << left << setw(20) << "Steps" << right << setw(12) << data.numSteps
         << "\n";

  if (data.hasBarrier)
    cout << left << setw(20) << "Barrier" << right << setw(12) << data.barrier
         << "\n";

  cout << string(35, '-') << "\n";
  cout << "\nResults:\n";
  cout << fixed << setprecision(6);
  cout << left << setw(20) << (data.option == CALL ? "Call Price" : "Put Price")
       << right << setw(12) << data.price << "\n";
  cout << string(35, '-') << "\n";

  cout << "\nGreeks:\n";
  cout << left << setw(20) << "Delta" << right << setw(12) << data.greeks.delta
       << "\n";
  cout << left << setw(20) << "Gamma" << right << setw(12) << data.greeks.gamma
       << "\n";
  cout << left << setw(20) << "Vega" << right << setw(12) << data.greeks.vega
       << "\n";
  cout << left << setw(20) << "Rho" << right << setw(12) << data.greeks.rho
       << "\n";
  cout << left << setw(20) << "Theta" << right << setw(12) << data.greeks.theta
       << "\n";
  cout << string(35, '-') << "\n";
}
