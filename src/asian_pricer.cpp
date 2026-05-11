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
    double s_antithetic = params.S;
    double path_sum = 0.0;
    double path_sum_anti = 0.0;

    for (int step = 0; step < numSteps; step++) {
      double z0 = generateGaussianNoise(0.0, 1.0);

      s_current =
          s_current * exp((params.r - 0.5 * params.vol * params.vol) * dt +
                          params.vol * sqrt(dt) * z0);

      s_antithetic =
          s_antithetic * exp((params.r - 0.5 * params.vol * params.vol) * dt +
                             params.vol * sqrt(dt) * (-z0));

      path_sum += s_current;
      path_sum_anti += s_antithetic;
    }

    double avg = path_sum / numSteps;
    double avg_anti = path_sum_anti / numSteps;

    double payoff, payoff_anti;
    if (option == CALL) {
      payoff = fmax(avg - params.K, 0.0);
      payoff_anti = fmax(avg_anti - params.K, 0.0);
    } else {
      payoff = fmax(params.K - avg, 0.0);
      payoff_anti = fmax(params.K - avg_anti, 0.0);
    }

    sum += useVarianceReduction ? 0.5 * (payoff + payoff_anti) : payoff;
  }

  return exp(-params.r * params.T) * sum / params.N;
}
