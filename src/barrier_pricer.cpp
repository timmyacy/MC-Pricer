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
    bool knocked_out = false;
    for (int step = 0; step < numSteps; step++) {
      auto [z0, z1] = generateGaussianNoise(0.0, 1.0);
      s_current =
          s_current * exp((params.r - 0.5 * params.vol * params.vol) * dt +
                          params.vol * sqrt(dt) * z0);
      if (option == CALL && s_current >= barrier) {
        knocked_out = true;
        break;
      }
      if (option == PUT && s_current <= barrier) {
        knocked_out = true;
        break;
      }
    }
    if (!knocked_out) {
      if (option == CALL)
        sum += max(s_current - params.K, 0.0);
      else
        sum += max(params.K - s_current, 0.0);
    }
  }

  return exp(-params.r * params.T) * sum / params.N;
}
