//
//  compile.cpp
//  Pseudo ASM
//
//  Created by Indi Kernick on 5/3/18.
//  Copyright © 2018 Indi Kernick. All rights reserved.
//

#include "compile.hpp"

#include <unordered_map>

using namespace std::string_view_literals;

namespace {
  int getBase(const char back) {
    if (back == 'h') {
      return 16;
    } else if (back == 'b') {
      return 2;
    } else {
      return 0;
    }
  }
  
  template <typename Int>
  void pushNum(std::vector<Byte> &bytes, const Int num) {
    union {
      Int number = num;
      Byte numBytes[sizeof(Int)];
    };
    bytes.insert(bytes.end(), std::cbegin(numBytes), std::cend(numBytes));
  }
  
  template <typename Int>
  void parseNumLiteral(Byte *bytes, const std::string_view str) {
    using Signed = std::make_signed_t<Int>;
    using Unsigned = std::make_unsigned_t<Int>;
    using SLimits = std::numeric_limits<Signed>;
    using ULimits = std::numeric_limits<Unsigned>;
    
    char *end;
    const long n = strtol(str.data(), &end, getBase(str.back()));
    if (n == 0 && end[-1] != 0) {
      throw "Invalid number literal"sv;
    }
    if (SLimits::min() <= n && n <= SLimits::max()) {
      pushNum(bytes, static_cast<Signed>(n));
    } else if (0 <= n && n <= ULimits::max()) {
      pushNum(bytes, static_cast<Unsigned>(n));
    } else {
      throw "Number literal out of range"sv;
    }
  }
  
  template <typename Int>
  void pushNumLiteral(std::vector<Byte> &bytes, const std::string_view str) {
    Byte b[sizeof(Int)];
    parseNumLiteral<Int>(b, str);
    bytes.insert(bytes.end(), std::cbegin(b), std::cend(b));
  }
  
  using TokenIter = std::vector<Token>::const_iterator;
  
  void pushBytes(std::vector<Byte> &bytes, TokenIter &t) {
    ++t;
    while (t->type != TokenType::END_OP) {
      const char front = t->str.front();
      const char back = t->str.back();
      if ((front == '"' || front == '\'') && front == back) {
        bytes.insert(bytes.end(), t->str.begin() + 1, t->str.end() - 1);
      } else if (isdigit(front)) {
        pushNumLiteral<int8_t>(bytes, t->str);
      } else {
        throw "Invalid byte literal"sv;
      }
      ++t;
    }
  }
  
  void pushWords(std::vector<Byte> &bytes, TokenIter &t) {
    ++t;
    while (t->type != TokenType::END_OP) {
      pushNumLiteral<int16_t>(bytes, t->str);
      ++t;
    }
  }
  
  void checkInstr(Instruction &instr) {
    
  }
  
  const std::string_view REGS[] = {
    "ip", "sf", "sp", "bp", "si", "di",
    "a", "b", "c", "d"
  };
  
  const std::string_view INSTRS[] = {
    "mov", "load", "store",
    "add", "sub", "mul", "div", "mod", "neg", "inc", "dec",
    "and", "or", "xor", "shr", "shl", "not",
    "cmp",
    "je", "jne", "jl", "jle", "jg", "jge",
    "jmp", "call", "ret", "int", "hlt",
    "push", "pop"
  };
  
  bool parseReg(RegByte &byte, OpSize &size, std::string_view regStr) {
    OpSize opSize = OpSize::WORD;
    BytePos pos = BytePos::LOW;
    
    if (regStr.back() == 'l' || regStr.back() == 'h') {
      regStr.remove_suffix(1);
      opSize = OpSize::BYTE;
    }
    if (regStr.back() == 'h') {
      pos = BytePos::HIGH;
    }
    
    /*
    if (regStr.back() == 'l') goto Low;
    if (regStr.back() != 'h') goto Word;
    pos = BytePos::HIGH;
    Low:
    regStr.remove_suffix(1);
    opSize = OpSize::BYTE;
    Word:
    */
    
    const std::string_view *regName = std::find(
      std::cbegin(REGS), std::cend(REGS), regStr
    );
    const RegCode regCode = static_cast<RegCode>(regName - REGS);
    if (regName != std::cend(REGS)) {
      byte = makeRegByte(regCode, pos);
      size = opSize;
      return true;
    } else {
      return false;
    }
  }
  
  using LabelMap = std::unordered_map<std::string_view, Word>;
  
  void pushInstr(std::vector<Byte> &bytes, TokenIter &t, const LabelMap &labels) {
    const std::string_view *instrName = std::find(
      std::cbegin(INSTRS), std::cend(INSTRS), t->str
    );
    if (instrName == std::cend(INSTRS)) {
      throw "Invalid instruction name"sv;
    }
    
    OpSize opSize = OpSize::WORD;
    DstOp dstOp = DstOp::REG;
    SrcOp srcOp = SrcOp::REG;
    const OpCode opCode = static_cast<OpCode>(instrName - INSTRS);
    Instruction instr = {};
    
    for (int o = 0; o != 2; ++o) {
      ++t;
      SplitWord &operand = (&instr.dst)[o];
      if (t->type == TokenType::END_OP) {
        instr.op = makeOpByte(opSize, dstOp, srcOp, opCode);
        checkInstr(instr);
        pushNum(bytes, instr);
        return;
      } else if (isdigit(t->str.front())) {
        dstOp = DstOp::CON;
        parseNumLiteral<Word>(&operand.l, t->str);
      } else {
        if (!parseReg(operand.l, opSize, t->str)) {
          const auto label = labels.find(t->str);
          if (label == labels.cend()) {
            operand.w = label->second;
          } else {
            throw "Name is not a register or label"sv;
          }
        }
      }
    }
    throw "Too many operands"sv;
  }
}

std::vector<Byte> compile(const std::vector<Token> &tokens) {
  LabelMap labels;
  
  Word currentInstr = 0;
  for (const Token &token : tokens) {
    if (token.type == TokenType::LABEL) {
      labels.emplace(token.str, currentInstr);
    } else if (token.type == TokenType::INSTRUCTION) {
      currentInstr += sizeof(Instruction);
    }
  }
  
  std::vector<Byte> bytes;
  bytes.reserve(tokens.size() * 2);
  
  for (auto t = tokens.cbegin(); t != tokens.cend(); ++t) {
    if (t->type == TokenType::INSTRUCTION) {
      try {
        if (t->str == "db") {
          pushBytes(bytes, t);
        } else if (t->str == "dw") {
          pushWords(bytes, t);
        } else {
          pushInstr(bytes, t, labels);
        }
      } catch (std::string_view &e) {
        throw std::runtime_error(e.data());
      }
    }
  }
  
  return bytes;
}
