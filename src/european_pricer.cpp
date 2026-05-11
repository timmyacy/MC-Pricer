#include "european_pricer.h"
#include <algorithm>
#include <cmath>
using namespace std;

EuropeanPricer::EuropeanPricer(const MCParams &params, Option option)
    : OptionPricer(params, option) {}

double EuropeanPricer::price() {
  double sum = 0.0;

  for (int i = 0; i < params.N; i++) {
    double z0 = generateGaussianNoise(0.0, 1.0);

    double ST =
        params.S * exp((params.r - 0.5 * params.vol * params.vol) * params.T +
                       params.vol * sqrt(params.T) * z0);

    double ST_anti =
        params.S * exp((params.r - 0.5 * params.vol * params.vol) * params.T +
                       params.vol * sqrt(params.T) * (-z0));

    double payoff, payoff_anti;

    if (option == CALL) {
      payoff = max(ST - params.K, 0.0);
      payoff_anti = max(ST_anti - params.K, 0.0);
    } else {
      payoff = max(params.K - ST, 0.0);
      payoff_anti = max(params.K - ST_anti, 0.0);
    }

    sum += useVarianceReduction ? 0.5 * (payoff + payoff_anti) : payoff;
  }

  return exp(-params.r * params.T) * sum / params.N;
}
