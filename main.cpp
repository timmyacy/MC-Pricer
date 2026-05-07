#include "european_pricer.h"

#include <iostream>
#include <memory>
using namespace std;

int main() {
  int choice = getMenuChoice();
  MCParams params = enterValues();

  Option option = static_cast<Option>(choice);

  auto pricer = make_unique<EuropeanPricer>(params, option);
  cout << "\nPrice: " << pricer->price() << "\n";

  return 0;
}
