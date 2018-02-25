//
//  parse.hpp
//  Pseudo ASM
//
//  Created by Indi Kernick on 25/2/18.
//  Copyright © 2018 Indi Kernick. All rights reserved.
//

#ifndef parse_hpp
#define parse_hpp

#include "ir.hpp"
#include <vector>
#include "token.hpp"

std::vector<IR> parse(const std::vector<Token> &);

#endif
