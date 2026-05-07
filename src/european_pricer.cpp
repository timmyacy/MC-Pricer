#include "european_pricer.h"
#include <algorithm>
#include <cmath>
using namespace std;

EuropeanPricer::EuropeanPricer(const MCParams &params, Option option)
    : OptionPricer(params, option) {}

double EuropeanPricer::price() {
  double sum = 0.0;
  for (int i = 0; i < params.N; i++) {
    auto [z0, z1] = generateGaussianNoise(0.0, 1.0);
    double ST =
        params.S * exp((params.r - 0.5 * params.vol * params.vol) * params.T +
                       params.vol * sqrt(params.T) * z0);
    if (option == CALL)
      sum += max(ST - params.K, 0.0);
    else
      sum += max(params.K - ST, 0.0);
  }
  return exp(-params.r * params.T) * sum / params.N;
}
