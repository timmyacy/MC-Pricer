#include "binary_pricer.h"
#include "european_pricer.h"

#include <iostream>
using namespace std;

int main() {
  int optionChoice = selectFromMenu("Select option type:",
                                    {"Binary Option", "European Option"});
  int choice = selectFromMenu("Select call or put:", {"CALL", "PUT"});
  MCParams params = enterValues();
  Option option = static_cast<Option>(choice);

  if (optionChoice == 0) {
    BinaryPricer pricer(params, option);
    cout << "The price of the Binary " << (choice == 0 ? "call" : "put")
         << " option: " << pricer.price() << "\n";
  }
  if (optionChoice == 1) {
    EuropeanPricer pricer(params, option);
    cout << "The price of the European " << (choice == 0 ? "call" : "put")
         << " option: " << pricer.price() << "\n";
  }

  return 0;
}
