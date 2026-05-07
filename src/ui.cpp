#include "ui.h"
#include <iostream>

using namespace std;
MCParams enterValues() {
  MCParams params;
  cout << "Please enter your current price (S) \n";
  cin >> params.S;
  cout << "Please enter your strike price (K) \n";
  cin >> params.K;
  cout << "Please enter your interest rate in percentage (r) \n";
  cin >> params.r;
  cout << "Please enter the volatility i.e 5%,10% (vol) \n";
  cin >> params.vol;
  cout << "Please enter the duration in years (6 months i.e 1.5) (T) \n";
  cin >> params.T;

  cout << "Please enter a path count (N) \n";
  cin >> params.N;
  cin.ignore();
  return params;
}

int getMenuChoice() {
  string options[] = {"CALL", "PUT"};
  int selected = 0;
  int total = 3;

  while (true) {
    cout << "\033[2J\033[H";
    cout << "Select option type (use arrow keys, enter to confirm):\n\n";

    for (int i = 0; i < total; i++) {
      if (i == selected)
        cout << " > " << options[i] << "\n";
      else
        cout << "   " << options[i] << "\n";
    }

    system("stty raw -echo");
    char c = getchar();
    system("stty cooked echo");

    if (c == '\033') {
      getchar();
      char arrow = getchar();
      if (arrow == 'A')
        selected = max(0, selected - 1);
      if (arrow == 'B')
        selected = min(total - 1, selected + 1);
    }
    if (c == '\n' || c == '\r')
      break;
  }

  return selected;
}
