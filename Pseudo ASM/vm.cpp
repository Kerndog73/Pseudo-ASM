//
//  vm.cpp
//  Pseudo ASM
//
//  Created by Indi Kernick on 1/3/18.
//  Copyright © 2018 Indi Kernick. All rights reserved.
//

#include "vm.hpp"

Register &getReg(Registers &regs, const RegCode code) {
  static_assert(sizeof(Registers) == 9 * sizeof(Register));
  return *(&regs.ip + static_cast<size_t>(code));
}

namespace {
  Word &getWordReg(Registers &regs, const RegByte byte) {
    return getReg(regs, getRegCode(byte)).w;
  }
  Byte &getByteReg(Registers &regs, const RegByte byte) {
    const RegSize size = getRegSize(byte);
    if (size == RegSize::LOW) {
      return getReg(regs, getRegCode(byte)).l;
    } else {
      return getReg(regs, getRegCode(byte)).h;
    }
  }

  void mov(VM &vm, const Instruction instr) {
    if (getSrcOp(instr.op) == SrcOp::REG) {
      if (getRegSize(instr.dst) == RegSize::WORD) {
        getWordReg(vm.regs, instr.dst) = getWordReg(vm.regs, instr.src.l);
      } else {
        getByteReg(vm.regs, instr.dst) = getByteReg(vm.regs, instr.src.l);
      }
    } else {
      if (getRegSize(instr.dst) == RegSize::WORD) {
        getWordReg(vm.regs, instr.dst) = instr.src.w;
      } else {
        getByteReg(vm.regs, instr.dst) = instr.src.l;
      }
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
    case OpCode::MOV:
      return mov(*this, instr);
    /*case OpCode::LOAD:
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
      break;*/
  }
}
