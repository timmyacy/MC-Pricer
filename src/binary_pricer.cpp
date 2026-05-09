
#include "binary_pricer.h"
#include <cmath>
using namespace std;

BinaryPricer::BinaryPricer(const MCParams &params, Option option)
    : OptionPricer(params, option) {}

double BinaryPricer::price() {
  double count = 0.0;
  for (int i = 0; i < params.N; i++) {
    auto [z0, z1] = generateGaussianNoise(0.0, 1.0);
    double ST =
        params.S * exp((params.r - 0.5 * params.vol * params.vol) * params.T +
                       params.vol * sqrt(params.T) * z0);
    if (option == CALL && ST > params.K)
      count += 1;
    if (option == PUT && ST < params.K)
      count += 1;
  }
  return exp(-params.r * params.T) * count / params.N;
}
