#include "barrier_pricer.h"
#include <algorithm>
#include <cmath>
using namespace std;

BarrierPricer::BarrierPricer(const MCParams &params, Option option,
                             int numSteps, double barrier)
    : OptionPricer(params, option), numSteps(numSteps), barrier(barrier) {}

double BarrierPricer::price() {
  double sum = 0.0;
  double dt = params.T / numSteps;

  for (int i = 0; i < params.N; i++) {
    double s_current = params.S;
    double s_antithetic = params.S;
    bool knocked_out = false;
    bool knocked_out_anti = false;

    for (int step = 0; step < numSteps; step++) {
      double z = generateGaussianNoise(0.0, 1.0);

      s_current =
          s_current * exp((params.r - 0.5 * params.vol * params.vol) * dt +
                          params.vol * sqrt(dt) * z);

      s_antithetic =
          s_antithetic * exp((params.r - 0.5 * params.vol * params.vol) * dt +
                             params.vol * sqrt(dt) * (-z));

      if (option == CALL) {
        if (s_current >= barrier)
          knocked_out = true;
        if (s_antithetic >= barrier)
          knocked_out_anti = true;
      } else {
        if (s_current <= barrier)
          knocked_out = true;
        if (s_antithetic <= barrier)
          knocked_out_anti = true;
      }

      // only break if both knocked out otherwise one path still live
      if (knocked_out && knocked_out_anti)
        break;
    }

    double payoff = 0.0, payoff_anti = 0.0;

    if (!knocked_out) {
      payoff = option == CALL ? max(s_current - params.K, 0.0)
                              : max(params.K - s_current, 0.0);
    }
    if (!knocked_out_anti) {
      payoff_anti = option == CALL ? max(s_antithetic - params.K, 0.0)
                                   : max(params.K - s_antithetic, 0.0);
    }

    sum += useVarianceReduction ? 0.5 * (payoff + payoff_anti) : payoff;
  }

  return exp(-params.r * params.T) * sum / params.N;
}
