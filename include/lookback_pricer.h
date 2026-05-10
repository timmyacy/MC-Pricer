#pragma once
#include "option_pricer.h"

class LookbackPricer : public OptionPricer {
public:
  LookbackPricer(const MCParams &params, Option option, int numSteps);
  double price() override;

private:
  int numSteps;
};
