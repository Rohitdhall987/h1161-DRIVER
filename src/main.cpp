//
// Created by rohit on 1/24/26.
// ,,,,,,,,
#include <csignal>
#include <string>
#include "utils.h"

void handle_signal(int signum) {
    print(std::to_string(signum));
}

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    return 0;
}