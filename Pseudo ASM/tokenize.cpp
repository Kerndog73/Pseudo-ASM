//
//  tokenize.cpp
//  Pseudo ASM
//
//  Created by Indi Kernick on 25/2/18.
//  Copyright © 2018 Indi Kernick. All rights reserved.
//

#include "tokenize.hpp"

#include <Simpleton/Utils/parse string.hpp>

bool beginIdentifier(const char c) {
  return isalpha(c);
}

bool identifer(const char c) {
  return isalnum(c) || c == '_';
}

bool space(const char c) {
  return c == ' ' || c == '\t';
}

bool operand(const char c) {
  return isalnum(c);
}

std::vector<Token> tokenize(const std::string_view source) {
  Utils::ParseString str = {source.data(), source.size()};
  std::vector<Token> tokens;
  
  while (!str.empty()) {
    str.skipWhitespace();
    if (str.check("#")) {
      str.skipUntil('\n').expect('\n');
      continue;
    }
    
    str.expect(beginIdentifier, "identifier");
    const char *begin = str.data() - 1;
    str.skip(identifer);
    size_t size = str.data() - begin;
    str.skip(space);
    if (str.check(':')) {
      tokens.push_back({TokenType::LABEL, {begin, size}});
    } else {
      tokens.push_back({TokenType::INSTRUCTION, {begin, size}});
      
      while (!str.check('\n') && !str.empty()) {
        begin = str.data();
        str.expect(operand, "operand").skip(operand);
        size = str.data() - begin;
        tokens.push_back({TokenType::OPERAND, {begin, size}});
        str.skip(space);
      }
    }
  }
  
  return tokens;
}
