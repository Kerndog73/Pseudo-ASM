//
//  bytecode.hpp
//  Pseudo ASM
//
//  Created by Indi Kernick on 28/2/18.
//  Copyright © 2018 Indi Kernick. All rights reserved.
//

#ifndef bytecode_hpp
#define bytecode_hpp

#include <cstdint>

using Byte = uint8_t;
using Word = uint16_t;

// 4 bits
enum class RegCode : uint8_t {
  IP,
  SP,
  BP,
  SI,
  DI,
  A,
  B,
  C,
  D
};

// 2 bits
enum class RegSize : uint8_t {
  // bit 4 is whether this register is a byte
  WORD = 0b00,
  LOW  = 0b01,
  // 0b10
  HIGH = 0b11
};

using RegByte = Byte;

inline RegByte makeRegByte(const RegCode reg, const RegSize size) {
  return
    static_cast<RegByte>(size) << 4
    | static_cast<RegByte>(reg)
  ;
}

inline RegSize getRegSize(const RegByte byte) {
  return static_cast<RegSize>((byte >> 4) & 0b11);
}

inline RegCode getRegCode(const RegByte byte) {
  return static_cast<RegCode>(byte & 0b1111);
}

// 5 bits (26 operations)
enum class OpCode : uint8_t {
  //copies
  MOV, // copy a register or a constant into a register
  LOAD, // load the memory pointed to by SI into a register
  STORE, // store the register to the memory pointed to by DI
  
  //arithmetic
  ADD,
  SUB,
  MUL,
  DIV,
  MOD,
  NEG,
  
  //bits
  AND,
  OR,
  XOR,
  SHR,
  SHL,
  NOT,
  
  //flags
  CMP,
  
  //conditional jumps
  //equality
  JE,
  JNE,
  //signed comparison
  JL,
  JLE,
  JG,
  JGE,
  
  //unconditional jumps
  JMP,
  CALL,
  RET,
  INT,
  
  //stack
  PUSH,
  POP
};

// 1 bit
enum class DstOp : uint8_t {
  REG,
  CON
};

// 2 bits
enum class SrcOp : uint8_t {
  NONE,
  REG,
  CON
};

using OpByte = Byte;

inline OpByte makeOpByte(const DstOp first, const SrcOp sec, const OpCode op) {
  return
    static_cast<OpByte>(first) << 7
    | static_cast<OpByte>(sec) << 5
    | static_cast<OpByte>(op)
  ;
}

inline DstOp getDstOp(const OpByte byte) {
  return static_cast<DstOp>(byte >> 7);
}

inline SrcOp getSrcOp(const OpByte byte) {
  return static_cast<SrcOp>((byte >> 5) & 0b11);
}

inline OpCode getOpCode(const OpByte byte) {
  return static_cast<OpCode>(byte & 0b11111);
}

struct Instruction {
  OpByte op;
  RegByte dst;
  union {
    Word w;
    struct {
      // assumes little endian
      Byte l;
      Byte h;
    };
  } src;
};

#endif
