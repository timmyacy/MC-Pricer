#pragma once

enum OptionType { BINARY, ASIAN, BARRIER, EUROPEAN, LOOKBACK };
enum Option { CALL, PUT };

struct Greeks {
  double delta;
  double gamma;
  double vega;
  double theta;
  double rho;
};

struct MCParams {
  double S;
  double K;
  double r;
  double vol;
  double T;
  double N;
  Option option;
  Greeks greeks;
};

struct PrintData {
  MCParams params;
  Option option;
  double price;
  Greeks greeks;
  int numSteps = 0;
  double barrier = 0.0;
  bool hasSteps = false;
  bool hasBarrier = false;
};

double pdf(double x);
double cdf(double x);
double calculate_d1(const MCParams &params);
double calculate_d2(double d1, const MCParams &params);
Greeks computeGreeks(const MCParams &params, Option option);
void printResults(const PrintData &data);
