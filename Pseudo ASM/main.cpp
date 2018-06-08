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
//#include "parse.hpp"
#include "compile.hpp"
#include "tokenize.hpp"
/*#include "validate.hpp"
#include "generate.hpp"*/
#include "print tokens.hpp"
#include <Simpleton/Memory/file io.hpp>

bool handleInt(VM &vm, const Byte code) {
  std::cout << "Interupt code " << static_cast<int>(code) << '\n';
  if (code == 0) {
    return true;
  } else if (code == 1) {
    std::cout << (vm.mem.get() + (VM::MEM_SIZE - VM::MAPPED_IO_SIZE)) << '\n';
  }
  return false;
}

int main(int argc, const char **argv) {
  if (argc < 2) {
    std::cout << "No input file\n";
    return 0;
  }
  
  std::cout << (const char *)0;
  
  const Memory::Buffer source = Memory::readFile(argv[1]);
  std::vector<Token> tokens = tokenize({source.cdata<char>(), source.size()});
  printTokens(std::cout, tokens);
  const std::vector<Byte> bytes = compile(tokens);
  
  VM vm(handleInt);
  vm.loadProgram(bytes.data(), bytes.size());
  vm.execUntilExit();
  
  /*if (argc < 2) {
    std::cout << "No input file\n";
    return 0;
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
