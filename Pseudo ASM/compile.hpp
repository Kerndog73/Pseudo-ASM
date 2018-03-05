//
//  compile.hpp
//  Pseudo ASM
//
//  Created by Indi Kernick on 5/3/18.
//  Copyright © 2018 Indi Kernick. All rights reserved.
//

#ifndef compile_hpp
#define compile_hpp

#include <vector>
#include "token.hpp"
#include "bytecode.hpp"

std::vector<Byte> compile(const std::vector<Token> &);

#endif
