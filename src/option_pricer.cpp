#include "option_pricer.h"
#include <cmath>
#include <random>
#include <utility>
using namespace std;

OptionPricer::OptionPricer(const MCParams &params, Option option)
    : params(params), option(option) {}

double OptionPricer::generateGaussianNoise(double mu, double sigma) {
  constexpr double two_pi = 2.0 * M_PI;
  static mt19937 rng(random_device{}());
  static uniform_real_distribution<> runif(0.0, 1.0);
  double u1, u2;
  do {
    u1 = runif(rng);
  } while (u1 == 0);
  u2 = runif(rng);
  auto mag = sigma * sqrt(-2.0 * log(u1));
  auto z0 = mag * cos(two_pi * u2) + mu;
  return z0;
}
