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

using namespace std::string_view_literals;
using IRIter = std::vector<IR>::const_iterator;

bool inRange(const Instr ie, const Instr lowe, const Instr highe) {
  using Type = std::underlying_type_t<Instr>;
  const Type i = static_cast<Type>(ie);
  const Type low = static_cast<Type>(lowe);
  const Type high = static_cast<Type>(highe);
  return low <= i && i <= high;
}

void validateInstr(const IRIter begin, const IRIter end) {
  const Instr instr = begin->instr;
  const size_t numOps = (end - begin) - 1;
  
  if (inRange(instr, Instr::MOV, Instr::XOR)) {
    if (numOps != 2) {
      throw "Instruction must have 2 operands"sv;
    }
    if (begin[1].type != IRType::OP_REGISTER) {
      throw "First operand must be a register"sv;
    }
    if (begin[2].type != IRType::OP_REGISTER && begin[2].type != IRType::OP_NUM) {
      throw "Second operand must be a register or number"sv;
    }
  }
  
  if (instr == Instr::NOT) {
    if (numOps != 1) {
      throw "Instruction must have 1 operand"sv;
    }
    if (begin[1].type != IRType::OP_REGISTER) {
      throw "Operand must be a register"sv;
    }
  }
  
  if (instr == Instr::CMP) {
    if (numOps != 2) {
      throw "Instruction must have 2 operands"sv;
    }
    if (begin[1].type != IRType::OP_REGISTER && begin[1].type != IRType::OP_NUM) {
      throw "First operand must be a register or number"sv;
    }
    if (begin[2].type != IRType::OP_REGISTER && begin[2].type != IRType::OP_NUM) {
      throw "Second operand must be a register or number"sv;
    }
  }
  
  if (inRange(instr, Instr::JE, Instr::CALL)) {
    if (numOps != 1) {
      throw "Instruction must have 1 operand"sv;
    }
    if (begin[1].type != IRType::OP_LABEL) {
      throw "Operand must be a label"sv;
    }
  }
  
  if (instr == Instr::RET) {
    if (numOps != 0) {
      throw "Instruct must have no operands"sv;
    }
  }
  
  //lets just skip syscall for the moment
  
  if (instr == Instr::PUSH || instr == Instr::POP) {
    if (numOps != 1) {
      throw "Instruction must have 1 operand"sv;
    }
    if (begin[1].type != IRType::OP_REGISTER) {
      throw "Operand must be a register"sv;
    }
  }
}

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
  
  bool ok = true;
  IRIter lastInstr;
  for (auto i = irs.cbegin(); i != irs.cend(); ++i) {
    switch (i->type) {
      case IRType::INSTRUCTION:
        lastInstr = i;
        break;
      case IRType::LABEL:
        break;
      case IRType::OP_REGISTER:
        break;
      case IRType::OP_LABEL:
        if (labels.count({i->label.data, i->label.size}) == 0) {
          std::cerr << i->line << ':' << i->col << ' ' << "Unknown label name \""
            << std::string_view(i->label.data, i->label.size) << "\"\n";
          ok = false;
        }
        break;
      case IRType::OP_NUM:
        break;
      case IRType::END_OP:
        try {
          validateInstr(lastInstr, i);
        } catch (std::string_view &e) {
          std::cerr << lastInstr->line << ':' << lastInstr->col << ' ' << e << '\n';
          ok = false;
        }
        break;
    }
  }
  
  return ok;
}
