//
//  validate.cpp
//  Pseudo ASM
//
//  Created by Indi Kernick on 25/2/18.
//  Copyright © 2018 Indi Kernick. All rights reserved.
//

#include "validate.hpp"

#include <iostream>
#include <string_view>
#include <unordered_set>

bool validate(const std::vector<IR> &irs) {
  if (irs.empty()) {
    return false;
  }

  std::unordered_set<std::string_view> labels;
  for (auto i = irs.cbegin(); i != irs.cend(); ++i) {
    if (i->type == IRType::LABEL) {
      labels.emplace(i->label.data, i->label.size);
    }
  }
  
  for (auto i = irs.cbegin(); i != irs.cend(); ++i) {
    switch (i->type) {
      case IRType::INSTRUCTION:
        break;
      case IRType::OP_LABEL:
        if (labels.count({i->label.data, i->label.size}) == 0) {
          std::cerr << i->line << ':' << i->col << ' ' << "Unknown label name \""
            << std::string_view(i->label.data, i->label.size) << "\"\n";
          return false;
        }
    }
  }
  
  return true;
}
