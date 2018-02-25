//
//  print tokens.cpp
//  Pseudo ASM
//
//  Created by Indi Kernick on 25/2/18.
//  Copyright © 2018 Indi Kernick. All rights reserved.
//

#include "print tokens.hpp"

void printToken(std::ostream &stream, const Token token) {
  switch (token.type) {
    case TokenType::INSTRUCTION:
      // padding to a multiple of 8
      stream << "Instruction     ";
      break;
    case TokenType::LABEL:
      stream << "Label           ";
      break;
    case TokenType::OPERAND:
      stream << "Operand         ";
      break;
  }

  stream << token.line << ':' << token.col << '\t';

  stream << '"' << token.str << '"';
}

void printTokens(std::ostream &stream, const std::vector<Token> &tokens) {
  for (auto t = tokens.cbegin(); t != tokens.cend(); ++t) {
    printToken(stream, *t);
    stream << '\n';
  }
}
