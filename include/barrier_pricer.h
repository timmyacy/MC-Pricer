#pragma once
#include "option_pricer.h"

class BarrierPricer : public OptionPricer {
public:
  BarrierPricer(const MCParams &params, Option option, int numSteps,
                double barrier);
  double price() override;

private:
  double barrier;
  int numSteps;
};
