//
// Created by rohit on 1/24/26.
//

#include "utils.h"

#include <iostream>
#include <ostream>
#include <filesystem>
#include <fstream>

void print(const std::string &str) {
    if (!RELEASE) {
        std::cout << "printing: " << str << std::endl;
    }
}

void printerr(const std::string &str) {
    if (!RELEASE)
        std::cerr << "error: " << str << std::endl;
}

