#pragma once
#include "model.h"
class OptionPricer {
public:
  OptionPricer(const MCParams &params, Option option);
  virtual double price() = 0;
  virtual ~OptionPricer() = default;

  void setVarianceReduction(bool enabled) { // ← setter lives here
    useVarianceReduction = enabled;
  }

protected:
  MCParams params;
  Option option;
  bool useVarianceReduction = false; // ← member lives here
  double generateGaussianNoise(double mu, double sigma);
};
