//
//  print tokens.cpp
//  Pseudo ASM
//
//  Created by Indi Kernick on 25/2/18.
//  Copyright © 2018 Indi Kernick. All rights reserved.
//

#include "print tokens.hpp"

void printTokens(std::ostream &stream, const std::vector<Token> &tokens) {
  for (auto t = tokens.cbegin(); t != tokens.cend(); ++t) {
    switch (t->type) {
      case TokenType::INSTRUCTION:
        stream << "Instruction ";
        break;
      case TokenType::LABEL:
        stream << "Label       ";
        break;
      case TokenType::OPERAND:
        stream << "Operand     ";
        break;
    }
    
    stream << t->str << '\n';
  }
}
