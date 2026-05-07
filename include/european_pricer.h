#pragma once
#include "option_pricer.h"

class EuropeanPricer : public OptionPricer {
public:
  EuropeanPricer(const MCParams &params, Option option);
  double price() override;
};
