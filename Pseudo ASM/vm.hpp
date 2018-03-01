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
  explicit VM(InteruptHandler);

  void execOneInstr();
  void execNInstrs(size_t);
  void execUntilExit();

  std::unique_ptr<Byte []> mem;
  InteruptHandler intHandler;
  Registers regs;
  bool halt;
};

#endif
