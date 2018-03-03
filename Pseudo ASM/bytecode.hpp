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

static_assert(sizeof(Byte) == 1);
static_assert(sizeof(Word) == 2);

union SplitWord {
  Word w;
  struct {
    Byte l; // low
    Byte h; // high
  };
};

static_assert(sizeof(SplitWord) == sizeof(Word));

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

// 1 bit
enum class BytePos : uint8_t {
  LOW,
  HIGH
};

using RegByte = Byte;

inline RegByte makeRegByte(const RegCode reg, const BytePos byte) {
  return
    static_cast<RegByte>(reg) << 1
    | static_cast<RegByte>(byte)
  ;
}

// 5 bits (28 operations)
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
  INC,
  DEC,
  
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
enum class OpSize : uint8_t {
  BYTE,
  WORD
};

// 1 bit
enum class DstOp : uint8_t {
  REG,
  CON
};

// 1 bit
enum class SrcOp : uint8_t {
  REG,
  CON
};

using OpByte = Byte;

inline OpByte makeOpByte(
  const OpSize size,
  const DstOp first,
  const SrcOp sec,
  const OpCode op
) {
  return
    static_cast<OpByte>(size) << 7
    | static_cast<OpByte>(first) << 6
    | static_cast<OpByte>(sec) << 5
    | static_cast<OpByte>(op)
  ;
}

inline OpSize getOpSize(const OpByte byte) {
  return static_cast<OpSize>(byte >> 7);
}

inline DstOp getDstOp(const OpByte byte) {
  return static_cast<DstOp>((byte >> 6) & 1);
}

inline SrcOp getSrcOp(const OpByte byte) {
  return static_cast<SrcOp>((byte >> 5) & 1);
}

inline OpCode getOpCode(const OpByte byte) {
  return static_cast<OpCode>(byte & 0b11111);
}

struct Instruction {
  OpByte op;
  Byte padding;
  SplitWord dst;
  SplitWord src;
};

static_assert(sizeof(Instruction) == 6);

#endif
