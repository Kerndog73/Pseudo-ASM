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

VM::VM(const InteruptHandler intHandler)
  : mem(std::make_unique<Byte []>(65536 + 1)),
    intHandler(intHandler),
    regs(),
    halt(false) {}

