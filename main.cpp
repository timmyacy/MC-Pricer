#include "asian_pricer.h"
#include "barrier_pricer.h"
#include "binary_pricer.h"
#include "european_pricer.h"
#include "lookback_pricer.h"
#include "ui.h"
#include <iostream>
#include <memory>
using namespace std;

int main(int argc, char *argv[]) {
  bool useVarianceReduction = false;
  for (int i = 1; i < argc; i++) {
    if (string(argv[i]) == "--variance-reduction") {
      useVarianceReduction = true;
    }
  }
  int optionChoice = selectFromMenu(
      "Select option type:", {"Asian Option", "Barrier Option", "Binary Option",
                              "European Option", "Lookback Option"});
  int choice = selectFromMenu("Select call or put:", {"Call", "Put"});
  MCParams params = enterValues(optionChoice);
  Option option = static_cast<Option>(choice);

  unique_ptr<OptionPricer> pricer;
  PrintData data;
  data.params = params;
  data.option = option;

  if (optionChoice == 0) {
    int numSteps;
    cout << "Please enter number of steps (e.g. 12 = monthly, 252 = daily): \n";
    cin >> numSteps;
    pricer = make_unique<AsianPricer>(params, option, numSteps);
    data.hasSteps = true;
    data.numSteps = numSteps;
  }
  if (optionChoice == 1) {
    int numSteps;
    double barrier;
    cout << "Please enter number of steps (e.g. 12 = monthly, 252 = daily): \n";
    cin >> numSteps;
    cout << "Please enter barrier level: \n";
    cin >> barrier;
    pricer = make_unique<BarrierPricer>(params, option, numSteps, barrier);
    data.hasSteps = true;
    data.hasBarrier = true;
    data.numSteps = numSteps;
    data.barrier = barrier;
  }
  if (optionChoice == 2) {
    pricer = make_unique<BinaryPricer>(params, option);
  }
  if (optionChoice == 3) {
    pricer = make_unique<EuropeanPricer>(params, option);
  }
  if (optionChoice == 4) {
    int numSteps;
    cout << "Please enter number of steps (e.g. 12 = monthly, 252 = daily): \n";
    cin >> numSteps;
    pricer = make_unique<LookbackPricer>(params, option, numSteps);
    data.hasSteps = true;
    data.numSteps = numSteps;
  }

  if (!pricer) {
    cerr << "Invalid option type selected.\n";
    return 1;
  }

  pricer->setVarianceReduction(useVarianceReduction);

  if (useVarianceReduction)
    cout << "Variance reduction: ON (antithetic variates)\n\n";

  data.price = pricer->price();
  data.greeks = computeGreeks(params, option);
  printResults(data);

  return 0;
}
