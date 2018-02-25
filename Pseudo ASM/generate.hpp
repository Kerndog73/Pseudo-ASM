//
//  generate.hpp
//  Pseudo ASM
//
//  Created by Indi Kernick on 25/2/18.
//  Copyright © 2018 Indi Kernick. All rights reserved.
//

#ifndef generate_hpp
#define generate_hpp

#include "ir.hpp"
#include <vector>
#include <ostream>

void generate(std::ostream &, const std::vector<IR> &);

#endif
