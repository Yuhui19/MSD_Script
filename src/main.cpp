//
//  main.cpp
//  
//
//  Created by Yuhui on 1/22/20.
//  Copyright © 2020 Yuhui. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include "parse.hpp"
#include <sstream>
#include <fstream>
#include <string>
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "Env.hpp"
#include "expr.hpp"
#include "step.hpp"
#include "parse.hpp"

int main(int argc, char *argv[]) {
    
//    Catch::Session().run(argc, argv);
    
    
    if (argc == 1) {
        std::cout << parse(std::cin)->to_value(Env::emptyenv)->to_string() << std::endl;
    } else if (argc == 2 && strcmp(argv[1], "--opt")==0) {
        std::cout << parse(std::cin)->optimize()->to_string() <<std::endl;
    } else if (argc == 2 && strcmp(argv[1], "--step")==0) {
        std::cout << Step::interp_by_steps(parse(std::cin))->to_string() << std::endl;
    } else {
        std::cerr << "Unknown mode: " << argv[1] << std::endl;
        exit(1);
    }

//     insert code here...
//    std::cout << "Hello, World!\n";
    return 0;
}
