//
//  ir.hpp
//  Pseudo ASM
//
//  Created by Indi Kernick on 25/2/18.
//  Copyright © 2018 Indi Kernick. All rights reserved.
//

#ifndef ir_hpp
#define ir_hpp

#include <cstdint>
#include <string_view>

enum class IRType : uint8_t {
  INSTRUCTION,
  LABEL,
  OP_REGISTER,
  OP_LABEL,
  OP_NUM
};

enum class Instr : uint8_t {
  // arithmetic
  MOV,
  ADD,
  SUB,
  MUL,
  DIV,
  
  // bits
  AND,
  OR,
  XOR,
  NOT,
  
  // conditional jumps
  CMP,
  JE,
  JNE,
  JL,
  JLE,
  JG,
  JGE,
  
  // unconditional jumps
  JMP,
  CALL,
  RET,
  
  // stack
  PUSH,
  POP,
};

struct Register {
  uint8_t index;
  enum : uint8_t {
    BYTE = 1,
    WORD = 2,
    LONG = 4,
    QUAD = 8
  } size;
};

struct NumberLiteral {
  union {
    uint64_t u;
    int64_t s;
  };
  bool sign;
};

struct Label {
  const char *data;
  size_t size;
};

struct IR {
  IRType type;
  union {
    Instr instr;
    Label label;
    Register reg;
    NumberLiteral num;
  };
};

#endif
