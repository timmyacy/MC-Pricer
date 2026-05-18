#pragma once
#include "model.h"
class OptionPricer {
public:
  OptionPricer(const MCParams &params, Option option);
  virtual double price() = 0;
  virtual ~OptionPricer() = default;

  void setVarianceReduction(bool enabled) { useVarianceReduction = enabled; }

protected:
  MCParams params;
  Option option;
  bool useVarianceReduction = false;
  double generateGaussianNoise(double mu, double sigma);
};
