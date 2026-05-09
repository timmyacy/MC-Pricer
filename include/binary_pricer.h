#pragma once
#include "option_pricer.h"

class BinaryPricer : public OptionPricer {
public:
  BinaryPricer(const MCParams &params, Option option);
  double price() override;
};
