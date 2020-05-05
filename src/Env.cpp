//
//  Env.cpp
//  
//
//  Created by Yuhui on 4/11/20.
//  Copyright © 2020 Yuhui. All rights reserved.
//

#include <stdio.h>
#include "Env.hpp"
#include "value.hpp"
#include "parse.hpp"

PTR(Env) Env::emptyenv = NEW(EmptyEnv)();

PTR(Val) EmptyEnv::lookup(std::string find_name) {
    throw std::runtime_error("free variable: " + find_name);
}

bool EmptyEnv::equals(PTR(Env) env) {
    PTR(EmptyEnv) ee = CAST(EmptyEnv)(env);
    
    return ee != NULL;
}

ExtendedEnv::ExtendedEnv(std::string name, PTR(Val) val, PTR(Env) env) {
    this->rest = env;
    this->name = name;
    this->val = val;
}

PTR(Val) ExtendedEnv::lookup(std::string find_name) {
    if (find_name == name)
        return val;
    else
        return rest->lookup(find_name);
}

bool ExtendedEnv::equals(PTR(Env) env) {
    PTR(ExtendedEnv) ee = CAST(ExtendedEnv)(env);
    
    if (ee == NULL)
        return false;
    else
        return name == ee->name && val->equals(ee->val) && rest->equals(ee->rest);
}
