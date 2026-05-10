#pragma once
#include "model.h"
#include <string>
#include <vector>

MCParams enterValues(int optionChoice);
int selectFromMenu(const std::string &title,
                   const std::vector<std::string> &options);
