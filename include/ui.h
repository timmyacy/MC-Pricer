#include <string>
#include <vector>
using namespace std;
struct MCParams {
  double S;
  double K;
  double r;
  double vol;
  double T;
  double N;
};

MCParams enterValues(int optionChoice);
int selectFromMenu(const string &title, const vector<string> &options);
