
#include "binary_pricer.h"
#include <cmath>
using namespace std;

BinaryPricer::BinaryPricer(const MCParams &params, Option option)
    : OptionPricer(params, option) {}

double BinaryPricer::price() {
  double sum = 0.0;

  for (int i = 0; i < params.N; i++) {
    double z0 = generateGaussianNoise(0.0, 1.0);

    double ST =
        params.S * exp((params.r - 0.5 * params.vol * params.vol) * params.T +
                       params.vol * sqrt(params.T) * z0);

    double ST_anti =
        params.S * exp((params.r - 0.5 * params.vol * params.vol) * params.T +
                       params.vol * sqrt(params.T) * (-z0));

    double payoff = 0.0;
    double payoff_anti = 0.0;

    if (option == CALL) {
      if (ST > params.K)
        payoff = 1.0;
      if (ST_anti > params.K)
        payoff_anti = 1.0;
    } else {
      if (ST < params.K)
        payoff = 1.0;
      if (ST_anti < params.K)
        payoff_anti = 1.0;
    }

    sum += useVarianceReduction ? 0.5 * (payoff + payoff_anti) : payoff;
  }

  return exp(-params.r * params.T) * sum / params.N;
}
