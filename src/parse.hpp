//
//  parse.hpp
//  
//
//  Created by Yuhui on 1/22/20.
//  Copyright © 2020 Yuhui. All rights reserved.
//

#ifndef parse_hpp
#define parse_hpp

#include <stdio.h>
#include <iostream>
#include "pointer.hpp"

class Expr;
PTR(Expr) parse(std::istream &in);

#endif /* parse_hpp */
