//
//  vm.cpp
//  Pseudo ASM
//
//  Created by Indi Kernick on 1/3/18.
//  Copyright © 2018 Indi Kernick. All rights reserved.
//

#include "vm.hpp"

static_assert(sizeof(Register) == sizeof(Word));
static_assert(sizeof(Registers) == 10 * sizeof(Register));

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
  
  #define SIGNED_L(VAL) reinterpret_cast<std::make_signed_t<T> &>(VAL)
  #define SIGNED_R(VAL) static_cast<std::make_signed_t<T>>(VAL)
  
  INSTR(mov) {
    DST_REG = SRC_OP;
  }
  INSTR(load) {
    DST_REG = SRC_MEM;
  }
  INSTR(store) {
    DST_MEM = DST_OP;
  }
  INSTR(add) {
    DST_REG += SRC_OP;
  }
  INSTR(sub) {
    DST_REG -= SRC_OP;
  }
  INSTR(mul) {
    SIGNED_L(DST_REG) *= SIGNED_R(SRC_OP);
  }
  INSTR(div) {
    SIGNED_L(DST_REG) /= SIGNED_R(SRC_OP);
  }
  INSTR(mod) {
    SIGNED_L(DST_REG) %= SIGNED_R(SRC_OP);
  }
  INSTR(neg) {
    DST_REG = -DST_REG;
  }
  INSTR(inc) {
    ++DST_REG;
  }
  INSTR(dec) {
    --DST_REG;
  }
  INSTR(andi) {
    DST_REG &= SRC_OP;
  }
  INSTR(ori) {
    DST_REG |= SRC_OP;
  }
  INSTR(xori) {
    DST_REG ^= SRC_OP;
  }
  INSTR(shr) {
    DST_REG >>= SRC_OP;
  }
  INSTR(shl) {
    DST_REG <<= SRC_OP;
  }
  INSTR(noti) {
    DST_REG = ~DST_REG;
  }
  INSTR(cmp) {
    const auto first = SIGNED_R(DST_OP);
    const auto second = SIGNED_R(SRC_OP);
    vm.regs.sf.w = ((first == second) << 1) | (first < second);
  }
  INSTR(je) {
    if (vm.regs.sf.w & 0b10) {
      vm.regs.ip.w = DST_OP;
    }
  }
  INSTR(jne) {
    if ((vm.regs.sf.w & 0b10) == 0) {
      vm.regs.ip.w = DST_OP;
    }
  }
  INSTR(jl) {
    if (vm.regs.sf.w == 0b01) {
      vm.regs.ip.w = DST_OP;
    }
  }
  INSTR(jle) {
    if (vm.regs.sf.w == 0b11) {
      vm.regs.ip.w = DST_OP;
    }
  }
  INSTR(jg) {
    if (vm.regs.sf.w == 0b00) {
      vm.regs.ip.w = DST_OP;
    }
  }
  INSTR(jge) {
    if (vm.regs.sf.w == 0b10) {
      vm.regs.ip.w = DST_OP;
    }
  }
  INSTR(jmp) {
    vm.regs.ip.w = DST_OP;
  }
  INSTR(call) {
    vm.regs.sp.w -= 2;
    *memPtr<Word>(vm, vm.regs.sp) = vm.regs.ip.w;
    vm.regs.ip.w = DST_OP;
  }
  INSTR(ret) {
    vm.regs.ip.w = *memPtr<Word>(vm, vm.regs.sp);
    vm.regs.sp.w += 2;
  }
  INSTR(inti) {
    if (vm.intHandler) {
      vm.halt = vm.intHandler(vm, DST_OP);
    }
  }
  INSTR(hlt) {
    vm.halt = true;
  }
  INSTR(push) {
    vm.regs.sp.w -= sizeof(T);
    *memPtr<T>(vm, vm.regs.sp) = DST_REG;
  }
  INSTR(pop) {
    DST_REG = *memPtr<T>(vm, vm.regs.sp);
    vm.regs.sp.w += sizeof(T);
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
  ++regs.ip.w;
  const OpCode op = getOpCode(instr.op);
  switch (op) {
    
    #define INSTR(ENUM_NAME, FUNC_NAME)                                         \
    case OpCode::ENUM_NAME:                                                     \
      if ((instr.op & 0b10000000) == 0) {                                       \
        return FUNC_NAME<Byte>(*this, instr);                                   \
      } else {                                                                  \
        return FUNC_NAME<Word>(*this, instr);                                   \
      }
    #define INSTR_B(ENUM_NAME, FUNC_NAME)                                       \
    case OpCode::ENUM_NAME:                                                     \
      return FUNC_NAME<Byte>(*this, instr);
    #define INSTR_W(ENUM_NAME, FUNC_NAME)                                       \
    case OpCode::ENUM_NAME:                                                     \
      return FUNC_NAME<Word>(*this, instr);
  
    INSTR(MOV, mov)
    INSTR(LOAD, load)
    INSTR(STORE, store)
    INSTR(ADD, add)
    INSTR(SUB, sub)
    INSTR(MUL, mul)
    INSTR(DIV, div)
    INSTR(MOD, mod)
    INSTR(NEG, neg)
    INSTR(INC, inc)
    INSTR(DEC, dec)
    INSTR(AND, andi)
    INSTR(OR, ori)
    INSTR(XOR, xori)
    INSTR(SHR, shr)
    INSTR(SHL, shl)
    INSTR(NOT, noti)
    INSTR(CMP, cmp)
    INSTR(JE, je)
    INSTR(JNE, jne)
    INSTR(JL, jl)
    INSTR(JLE, jle)
    INSTR(JG, jg)
    INSTR(JGE, jge)
    INSTR_W(JMP, jmp)
    INSTR_W(CALL, call)
    INSTR_W(RET, ret)
    INSTR_B(INT, inti)
    INSTR_W(HLT, hlt)
    INSTR(PUSH, push)
    INSTR(POP, pop)
  }
}

void VM::execUntilExit() {
  halt = false;
  do {
    execOneInstr();
  } while (!halt);
}
