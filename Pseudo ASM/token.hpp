//
//  token.hpp
//  Pseudo ASM
//
//  Created by Indi Kernick on 25/2/18.
//  Copyright © 2018 Indi Kernick. All rights reserved.
//

#ifndef token_hpp
#define token_hpp

#include <string_view>

enum class TokenType {
  INSTRUCTION,
  LABEL,
  OPERAND
};

struct Token {
  TokenType type;
  std::string_view str;
  unsigned line;
  unsigned col;
};

#endif
