#include "european_pricer.h"

#include <iostream>
#include <memory>
using namespace std;

int main() {
  MCParams params = enterValues();
  int choice = getMenuChoice(); // 0 = CALL, 1 = PUT
  Option option = static_cast<Option>(choice);

  auto pricer = make_unique<EuropeanPricer>(params, option);
  cout << "\nPrice: " << pricer->price() << "\n";

  return 0;
}
