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
    double s_antithetic = params.S;

    double path_min = numeric_limits<double>::max();
    double path_max = numeric_limits<double>::lowest();
    double path_min_anti = numeric_limits<double>::max();
    double path_max_anti = numeric_limits<double>::lowest();

    for (int step = 0; step < numSteps; step++) {
      double z0 = generateGaussianNoise(0.0, 1.0);

      s_current =
          s_current * exp((params.r - 0.5 * params.vol * params.vol) * dt +
                          params.vol * sqrt(dt) * z0);

      s_antithetic =
          s_antithetic * exp((params.r - 0.5 * params.vol * params.vol) * dt +
                             params.vol * sqrt(dt) * (-z0));

      path_min = min(path_min, s_current);
      path_max = max(path_max, s_current);
      path_min_anti = min(path_min_anti, s_antithetic);
      path_max_anti = max(path_max_anti, s_antithetic);
    }

    double payoff, payoff_anti;
    if (option == CALL) {
      payoff = max(s_current - path_min, 0.0);
      payoff_anti = max(s_antithetic - path_min_anti, 0.0);
    } else {
      payoff = max(path_max - s_current, 0.0);
      payoff_anti = max(path_max_anti - s_antithetic, 0.0);
    }

    sum += useVarianceReduction ? 0.5 * (payoff + payoff_anti) : payoff;
  }

  return exp(-params.r * params.T) * sum / params.N;
}
