//
//  vm.hpp
//  Pseudo ASM
//
//  Created by Indi Kernick on 1/3/18.
//  Copyright © 2018 Indi Kernick. All rights reserved.
//

#ifndef vm_hpp
#define vm_hpp

#include <memory>
#include <cstdint>
#include "bytecode.hpp"

union Register {
  Word w;
  struct {
    // assumes that this machine is little endian
    Byte l;
    Byte h;
  };
};

struct Registers {
  Register ip;
  Register sp;
  Register bp;
  Register si;
  Register di;
  Register a;
  Register b;
  Register c;
  Register d;
};

Register &getReg(Registers &, RegCode);

struct VM;
using InteruptHandler = bool (*) (VM &, Byte);

struct VM {
  static const size_t MEM_SIZE = 65536;
  static const size_t INSURANCE = sizeof(Instruction) - 1;
  static const size_t MAPPED_IO_SIZE = 1024;
  static const size_t MIN_STACK_SIZE = 1024;

  explicit VM(InteruptHandler);

  bool loadProgram(const Byte *, size_t);

  void execOneInstr();
  void execNInstrs(size_t);
  void execUntilExit();

  std::unique_ptr<Byte []> mem;
  InteruptHandler intHandler;
  Registers regs;
  bool halt;
};

#endif
