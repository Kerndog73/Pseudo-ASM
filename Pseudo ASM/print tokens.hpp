//
//  print tokens.hpp
//  Pseudo ASM
//
//  Created by Indi Kernick on 25/2/18.
//  Copyright © 2018 Indi Kernick. All rights reserved.
//

#ifndef print_tokens_hpp
#define print_tokens_hpp

#include <vector>
#include <iostream>
#include "token.hpp"

void printToken(std::ostream &, Token);
void printTokens(std::ostream &, const std::vector<Token> &);

#endif
