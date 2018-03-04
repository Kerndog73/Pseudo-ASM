//
//  main.cpp
//  Pseudo ASM
//
//  Created by Indi Kernick on 25/2/18.
//  Copyright © 2018 Indi Kernick. All rights reserved.
//

#include "vm.hpp"
#include <fstream>
#include <iostream>
/*#include "parse.hpp"
#include "tokenize.hpp"
#include "validate.hpp"
#include "generate.hpp"
#include "print tokens.hpp"*/
#include <Simpleton/Memory/file io.hpp>

int main(int argc, const char **argv) {
  VM vm(nullptr);
  Instruction instr;
  instr.op = makeOpByte(OpSize::WORD, DstOp::REG, SrcOp::REG, OpCode::HLT);
  vm.loadProgram(reinterpret_cast<const Byte *>(&instr), sizeof(Instruction));
  vm.execUntilExit();
  
  /*if (argc < 2) {
    std::cout << "No input file\n";
  }
  
  const Memory::Buffer source = Memory::readFile(argv[1]);
  std::vector<Token> tokens = tokenize({source.cdata<char>(), source.size()});
  std::vector<IR> irs = parse(tokens);
  if (validate(irs)) {
    std::ofstream file("out.c");
    generate(file, irs);
  }*/

  return 0;
}
