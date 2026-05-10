#include "asian_pricer.h"
#include <cmath>
using namespace std;

AsianPricer::AsianPricer(const MCParams &params, Option option, int numSteps)
    : OptionPricer(params, option), numSteps(numSteps) {}

double AsianPricer::price() {
  double sum = 0.0;
  double dt = params.T / numSteps;
  for (int i = 0; i < params.N; i++) {
    double s_current = params.S;
    double path_sum = 0.0;
    for (int step = 0; step < numSteps; step++) {
      auto [z0, z1] = generateGaussianNoise(0.0, 1.0);
      s_current =
          s_current * exp((params.r - 0.5 * params.vol * params.vol) * dt +
                          params.vol * sqrt(dt) * z0);
      path_sum += s_current;
    }
    double average = path_sum / numSteps;
    if (option == CALL)
      sum += fmax(average - params.K, 0.0);
    else
      sum += fmax(params.K - average, 0.0);
  }
  return exp(-params.r * params.T) * sum / params.N;
}
