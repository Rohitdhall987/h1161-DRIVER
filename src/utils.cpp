//
// Created by rohit on 1/24/26.
//

#include "utils.h"

#include <iostream>
#include <ostream>

void print(const std::string str) {
    if (!RELEASE) {
        std::cout << str << std::endl;
    }
}
