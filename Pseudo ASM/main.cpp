//
//  main.cpp
//  Pseudo ASM
//
//  Created by Indi Kernick on 25/2/18.
//  Copyright © 2018 Indi Kernick. All rights reserved.
//

#include <fstream>
#include <iostream>
#include "tokenize.hpp"
#include "print tokens.hpp"
#include <Simpleton/Memory/file io.hpp>

int main(int argc, const char **argv) {
  if (argc < 2) {
    std::cout << "No input file\n";
  }
  
  const Memory::Buffer source = Memory::readFile(argv[1]);
  std::vector<Token> tokens;
  try {
    tokens = tokenize({source.cdata<char>(), source.size()});
  } catch (std::exception &e) {
    std::cerr << e.what() << '\n';
  }
  printTokens(std::cout, tokens);
  
  return 0;
}
