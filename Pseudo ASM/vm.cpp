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
  template <typename T>
  T &getReg(Registers &regs, const RegByte reg) {
    if constexpr (std::is_same_v<T, Byte>) {
      return *(&regs.first.l + static_cast<size_t>(reg));
    } else {
      return *(&regs.first.w + static_cast<size_t>(reg >> 1));
    }
  }
  template <typename T>
  T getOperand(Registers &regs, const OpByte operation, const SplitWord operand) {
    if ((operation & 0b10000) == 0) {
      return getReg<T>(regs, operand.l);
    } else {
      if constexpr (std::is_same_v<T, Byte>) {
        return operand.l;
      } else {
        return operand.w;
      }
    }
  }
  template <typename T>
  T *memPtr(VM &vm, const Register reg) {
    return reinterpret_cast<T *>(vm.mem.get() + reg.w);
  }
  
  #define INSTR(NAME)                                                           \
    template <typename T>                                                       \
    void NAME(VM &vm, const Instruction instr)
  
  #define DST_REG getReg<T>(vm.regs, instr.dst.l)
  #define DST_OP getOperand<T>(vm.regs, instr.op, instr.dst)
  #define SRC_OP getOperand<T>(vm.regs, instr.op, instr.src)
  #define DST_MEM *memPtr<T>(vm, vm.regs.di)
  #define SRC_MEM *memPtr<T>(vm, vm.regs.si)
  
  INSTR(mov) {
    DST_REG = SRC_OP;
  }
  
  INSTR(load) {
    DST_REG = SRC_MEM;
  }
  
  INSTR(store) {
    DST_MEM = DST_REG;
  }
  
  INSTR(add) {
    DST_REG += SRC_OP;
  }
  
  INSTR(sub) {
    DST_REG -= SRC_OP;
  }
  
  INSTR(mul) {
    DST_REG *= SRC_OP;
  }
  
  INSTR(div) {
    DST_REG /= SRC_OP;
  }
  
  INSTR(mod) {
    DST_REG %= SRC_OP;
  }
  
  #undef INSTR
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
        return FUNC_NAME<Byte>(*this, instr);                                   \
      } else {                                                                  \
        return FUNC_NAME<Word>(*this, instr);                                   \
      }
  
    INSTR(MOV, mov)
    INSTR(LOAD, load)
    INSTR(STORE, store)
    INSTR(ADD, add)
    INSTR(SUB, sub)
    INSTR(MUL, mul)
    INSTR(DIV, div)
    INSTR(MOD, mod)
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
