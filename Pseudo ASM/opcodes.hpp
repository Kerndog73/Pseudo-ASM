//
//  opcodes.hpp
//  Pseudo ASM
//
//  Created by Indi Kernick on 28/2/18.
//  Copyright © 2018 Indi Kernick. All rights reserved.
//

#ifndef opcodes_hpp
#define opcodes_hpp

#include <cstdint>

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

// least significant 4 bits
//   RegCode
//   removing the IP register will make this 3 bits
// next least significant 2 bits
//   RegSize
// this leaves 2 unused bits
using RegByte = uint8_t;

// 6 bits
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


// most significant bit
//   0 (first operand is reg)
//   1 (first operand is constant)
// next 2 most significant bits
//   00 (no second operand)
//   01 (second operand is reg)
//   10 (second operand is constant)
// 6 least significant bits
//   OpCode
//   there are 26 OpCodes currently so only 5 bits are needed
using OpByte = uint8_t;

// the next byte is the RegCode for the desination operand
// the next word is either
//   padding (no second operand)
//   RegCode byte + padding byte (second operand is reg)
//   byte constant + padding byte (second operand is byte constant)
//   word constant (second operand is word constant)

#endif
