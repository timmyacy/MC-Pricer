#include "ui.h"
#include "model.h"
#include <iostream>
using namespace std;

#include <termios.h>
#include <unistd.h>

void setRawMode(bool enable) {
  static struct termios oldt;
  if (enable) {
    struct termios newt;
    tcgetattr(STDIN_FILENO, &oldt); // save old settings
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // raw, no echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  } else {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // restore
  }
}
MCParams enterValues(int optionChoice) {
  MCParams params;
  cout << "Please enter your current price (S) \n";
  cin >> params.S;
  cout << "Please enter your strike price (K) \n";
  cin >> params.K;
  cout << "Please enter your interest rate  (r) \n";
  cin >> params.r;
  cout << "Please enter the volatility i.e 0.05 ,0.2 etc (vol) \n";
  cin >> params.vol;
  cout << "Please enter the duration in years (6 months i.e 0.5) (T) \n";
  cin >> params.T;
  cout << "Please enter a path count (N) \n";
  cin >> params.N;
  return params;
}

int selectFromMenu(const string &title, const vector<string> &options) {
  int selected = 0;
  int total = options.size();

  setRawMode(true);
  while (true) {
    cout << "\033[2J\033[H";
    cout << title << "\n\n";
    for (int i = 0; i < total; i++) {
      cout << (i == selected ? " > " : "   ") << options[i] << "\n";
    }

    char c = getchar();
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
  setRawMode(false);
  return selected;
}
