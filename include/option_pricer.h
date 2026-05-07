#pragma once
#include "model.h"
#include <utility>
class OptionPricer {
public:
  OptionPricer(const MCParams &params, Option option);
  virtual double price() = 0;
  virtual ~OptionPricer() = default;

protected:
  MCParams params;
  Option option;
  std::pair<double, double> generateGaussianNoise(double mu, double sigma);
};
