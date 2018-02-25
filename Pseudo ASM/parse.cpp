//
//  parse.cpp
//  Pseudo ASM
//
//  Created by Indi Kernick on 25/2/18.
//  Copyright © 2018 Indi Kernick. All rights reserved.
//

#include "parse.hpp"

#include <cassert>
#include <iostream>

using namespace std::string_view_literals;

const std::string_view registers[] = {
  "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
  "r0l", "r1l", "r2l", "r3l", "r4l", "r5l", "r6l", "r7l",
  "r0w", "r1w", "r2w", "r3w", "r4w", "r5w", "r6w", "r7w",
  "r0b", "r1b", "r2b", "r3b", "r4b", "r5b", "r6b", "r7b",
};

const std::string_view instrs[] = {
  "mov", "add", "sub", "mul", "div",
  "and", "or", "xor", "not",
  "cmp", "je", "jne", "jl", "jle", "jg", "jge",
  "jmp", "call", "ret",
  "push", "pop"
};

bool isRegister(const std::string_view str) {
  return std::find(std::cbegin(registers), std::cend(registers), str) != std::cend(registers);
}

IR parseInstr(const std::string_view str) {
  const std::string_view *instr = std::find(
    std::cbegin(instrs), std::cend(instrs), str
  );
  if (instr == std::cend(instrs)) {
    throw "Invalid instruction"sv;
  }
  IR ir;
  ir.type = IRType::INSTRUCTION;
  ir.instr = static_cast<Instr>(instr - instrs);
  return ir;
}

IR parseLabel(const std::string_view str) {
  if (isRegister(str)) {
    throw "Label cannot have same name as register"sv;
  }
  IR ir;
  ir.type = IRType::LABEL;
  ir.label = {str.data(), str.size()};
  return ir;
}

IR parseNumberOp(const std::string_view str) {
  IR ir;
  ir.type = IRType::OP_NUM;
  if (str.back() == 'u') {
    ir.num.sign = false;
    char *end;
    ir.num.u = std::strtoull(str.data(), &end, 0);
    if ((ir.num.u == 0 && end[-1] != '0') || end != &str.back()) {
      throw "Invalid number"sv;
    }
  } else {
    ir.num.sign = true;
    char *end;
    ir.num.s = std::strtoll(str.data(), &end, 0);
    if ((ir.num.s == 0 && end[-1] != '0') || end != str.data() + str.size()) {
      throw "Invalid number"sv;
    }
  }
  return ir;
}

IR parseRegisterOp(const std::string_view str) {
  IR ir;
  ir.type = IRType::OP_REGISTER;
  ir.reg.index = str[1] - '0';
  if (str.size() == 2) {
    ir.reg.size = Register::QUAD;
  } else if (str.back() == 'l') {
    ir.reg.size = Register::LONG;
  } else if (str.back() == 'w') {
    ir.reg.size = Register::WORD;
  } else if (str.back() == 'b') {
    ir.reg.size = Register::BYTE;
  } else {
    assert(false);
  }
  return ir;
}

IR parseLabelOp(const std::string_view str) {
  if (!std::isalpha(str.front())) {
    throw "Invalid label name"sv;
  }
  auto identifier = [] (const char c) -> bool {
    return isalnum(c) || c == '_';
  };
  if (!std::all_of(str.begin(), str.end(), identifier)) {
    throw "Invalid label name"sv;
  }
  IR ir;
  ir.type = IRType::OP_LABEL;
  ir.label = {str.data(), str.size()};
  return ir;
}

IR parseToken(const TokenType type, const std::string_view str) {
  switch (type) {
    case TokenType::INSTRUCTION:
      return parseInstr(str);
    case TokenType::LABEL:
      return parseLabel(str);
    case TokenType::OPERAND:
      if (str[0] == '-' || std::isdigit(str[0])) {
        return parseNumberOp(str);
      }
      if (isRegister(str)) {
        return parseRegisterOp(str);
      }
      return parseLabelOp(str);
    case TokenType::END_OP:
      IR ir;
      ir.type = IRType::END_OP;
      return ir;
  }
}

std::vector<IR> parse(const std::vector<Token> &tokens) {
  std::vector<IR> irs;
  
  for (auto t = tokens.cbegin(); t != tokens.cend(); ++t) {
    IR ir;
    try {
      ir = parseToken(t->type, t->str);
    } catch (std::string_view &e) {
      std::cerr << t->line << ':' << t->col << ' ' << e << " \"" << t->str << "\"\n";
      return {};
    }
    ir.line = t->line;
    ir.col = t->col;
    irs.push_back(ir);
  }
  
  return irs;
}
