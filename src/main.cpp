//
// Created by rohit on 1/24/26.
// ,,,,,,,,
#include <csignal>
#include <string>
#include "utils.h"


static volatile bool running = true;

void handle_signal(int signum) {
    print(std::to_string(signum));

    running = false;
}

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    while (running) {
        //TODO: Read for user input
    }
    return 0;
}