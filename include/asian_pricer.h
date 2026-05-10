#pragma once
#include "option_pricer.h"

class AsianPricer : public OptionPricer {
public:
  AsianPricer(const MCParams &params, Option option, int numSteps);
  double price() override;

private:
  int numSteps;
};
