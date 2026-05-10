#include "lookback_pricer.h"
#include <algorithm>
#include <cmath>
#include <limits>
using namespace std;

LookbackPricer::LookbackPricer(const MCParams &params, Option option,
                               int numSteps)
    : OptionPricer(params, option), numSteps(numSteps) {}

double LookbackPricer::price() {
  double sum = 0.0;
  double dt = params.T / numSteps;

  for (int i = 0; i < params.N; i++) {
    double s_current = params.S;
    double path_min = numeric_limits<double>::max();
    double path_max = numeric_limits<double>::lowest();

    for (int step = 0; step < numSteps; step++) {
      auto [z0, z1] = generateGaussianNoise(0.0, 1.0);
      s_current =
          s_current * exp((params.r - 0.5 * params.vol * params.vol) * dt +
                          params.vol * sqrt(dt) * z0);
      path_min = min(path_min, s_current);
      path_max = max(path_max, s_current);
    }

    if (option == CALL)
      sum += max(s_current - path_min, 0.0);
    else
      sum += max(path_max - s_current, 0.0);
  }

  return exp(-params.r * params.T) * sum / params.N;
}
