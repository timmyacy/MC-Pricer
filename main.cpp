#include "asian_pricer.h"
#include "barrier_pricer.h"
#include "binary_pricer.h"
#include "european_pricer.h"
#include "lookback_pricer.h"
#include <iostream>
using namespace std;

int main() {
  int optionChoice = selectFromMenu(
      "Select option type:", {"Asian Option", "Barrier Option", "Binary Option",
                              "European Option", "Lookback Option"});
  int choice = selectFromMenu("Select call or put:", {"Call", "Put"});
  MCParams params = enterValues(optionChoice);
  Option option = static_cast<Option>(choice);

  string callOrPut = (choice == 0 ? "call" : "put");

  if (optionChoice == 0) {
    int numSteps;
    cout << "Please enter number of steps (e.g. 12 = monthly, 252 = daily): \n";
    cin >> numSteps;
    AsianPricer pricer(params, option, numSteps);
    cout << "The price of the Asian " << callOrPut
         << " option: " << pricer.price() << "\n";
  }
  if (optionChoice == 1) {
    int numSteps;
    double barrier;
    cout << "Please enter number of steps (e.g. 12 = monthly, 252 = daily): \n";
    cin >> numSteps;
    cout << "Please enter barrier level: \n";
    cin >> barrier;
    BarrierPricer pricer(params, option, numSteps, barrier);
    cout << "The price of the Barrier " << callOrPut
         << " option: " << pricer.price() << "\n";
  }
  if (optionChoice == 2) {
    BinaryPricer pricer(params, option);
    cout << "The price of the Binary " << callOrPut
         << " option: " << pricer.price() << "\n";
  }
  if (optionChoice == 3) {
    EuropeanPricer pricer(params, option);
    cout << "The price of the European " << callOrPut
         << " option: " << pricer.price() << "\n";
  }
  if (optionChoice == 4) {
    int numSteps;
    cout << "Please enter number of steps (e.g. 12 = monthly, 252 = daily): \n";
    cin >> numSteps;
    LookbackPricer pricer(params, option, numSteps);
    cout << "The price of the lookback " << callOrPut
         << " option: " << pricer.price() << "\n";
  }

  return 0;
}
