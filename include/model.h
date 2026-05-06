#include "ui.h"

enum OptionType { BERMUDAN, ASIAN, EUROPEAN };

enum Option { CALL, PUT };

struct Model {
  Option option;
  OptionType type;
  MCParams params;
};
int getModelType() {}
