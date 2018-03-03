//
//  vm.cpp
//  Pseudo ASM
//
//  Created by Indi Kernick on 1/3/18.
//  Copyright © 2018 Indi Kernick. All rights reserved.
//

#include "vm.hpp"

static_assert(sizeof(Register) == sizeof(Word));
static_assert(sizeof(Registers) == 9 * sizeof(Register));

Register &getReg(Registers &regs, const RegCode code) {
  return *(&regs.first + static_cast<size_t>(code));
}

namespace {
  Word getWord(Registers &regs, const RegByte reg) {
    return *(&regs.first.w + static_cast<size_t>(reg >> 1));
  }
  Byte getByte(Registers &regs, const RegByte reg) {
    return *(&regs.first.l + static_cast<size_t>(reg));
  }
  void setWord(Registers &regs, const RegByte reg, const Word word) {
    *(&regs.first.w + static_cast<size_t>(reg >> 1)) = word;
  }
  void setByte(Registers &regs, const RegByte reg, const Byte byte) {
    *(&regs.first.l + static_cast<size_t>(reg)) = byte;
  }
  
  void movb(VM &vm, const Instruction instr) {
    if (getSrcOp(instr.op) == SrcOp::REG) {
      setByte(vm.regs, instr.dst.l, getByte(vm.regs, instr.src.l));
    } else {
      setByte(vm.regs, instr.dst.l, instr.src.l);
    }
  }
  
  void movw(VM &vm, const Instruction instr) {
    if (getSrcOp(instr.op) == SrcOp::REG) {
      setWord(vm.regs, instr.dst.l, getWord(vm.regs, instr.src.l));
    } else {
      setWord(vm.regs, instr.dst.l, instr.src.l);
    }
  }
}

VM::VM(const InteruptHandler intHandler)
  : mem(std::make_unique<Byte []>(MEM_SIZE + INSURANCE)),
    intHandler(intHandler),
    regs(),
    halt(false) {}

bool VM::loadProgram(const Byte *prog, const size_t size) {
  if (size < MEM_SIZE - MAPPED_IO_SIZE - MIN_STACK_SIZE) {
    std::memcpy(mem.get(), prog, size);
    std::memset(mem.get() + size, 0, MEM_SIZE - size + INSURANCE);
    regs = {};
    regs.sp.w = MEM_SIZE - MAPPED_IO_SIZE;
    regs.bp = regs.sp;
    halt = false;
    return true;
  } else {
    return false;
  }
}

void VM::execOneInstr() {
  const Instruction instr = *reinterpret_cast<Instruction *>(mem.get() + regs.ip.w);
  const OpCode op = getOpCode(instr.op);
  switch (op) {
    
    #define INSTR(ENUM_NAME, FUNC_NAME)                                         \
    case OpCode::ENUM_NAME:                                                     \
      if ((instr.op & 0b10000000) == 0) {                                       \
        return FUNC_NAME##b(*this, instr);                                      \
      } else {                                                                  \
        return FUNC_NAME##w(*this, instr);                                      \
      }
  
    INSTR(MOV, mov)
    case OpCode::LOAD:
      <#code#>
      break;
    case OpCode::STORE:
      <#code#>
      break;
    case OpCode::ADD:
      <#code#>
      break;
    case OpCode::SUB:
      <#code#>
      break;
    case OpCode::MUL:
      <#code#>
      break;
    case OpCode::DIV:
      <#code#>
      break;
    case OpCode::MOD:
      <#code#>
      break;
    case OpCode::NEG:
      <#code#>
      break;
    case OpCode::INC:
    case OpCode::DEC:
    case OpCode::AND:
      <#code#>
      break;
    case OpCode::OR:
      <#code#>
      break;
    case OpCode::XOR:
      <#code#>
      break;
    case OpCode::SHR:
      <#code#>
      break;
    case OpCode::SHL:
      <#code#>
      break;
    case OpCode::NOT:
      <#code#>
      break;
    case OpCode::CMP:
      <#code#>
      break;
    case OpCode::JE:
      <#code#>
      break;
    case OpCode::JNE:
      <#code#>
      break;
    case OpCode::JL:
      <#code#>
      break;
    case OpCode::JLE:
      <#code#>
      break;
    case OpCode::JG:
      <#code#>
      break;
    case OpCode::JGE:
      <#code#>
      break;
    case OpCode::JMP:
      <#code#>
      break;
    case OpCode::CALL:
      <#code#>
      break;
    case OpCode::RET:
      <#code#>
      break;
    case OpCode::INT:
      <#code#>
      break;
    case OpCode::PUSH:
      <#code#>
      break;
    case OpCode::POP:
      <#code#>
      break;
  }
}
