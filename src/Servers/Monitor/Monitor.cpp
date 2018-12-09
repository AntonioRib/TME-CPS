#include "Monitor.h"

Monitor::Monitor() {
    std::cout << "Monitor created\n";
}

int main(int argc, char* argv[]) {
    std::cout << "Will try to create Monitor\n";

    Monitor* monitor;
    monitor = new Monitor();

    std::cout << "Bubye\n";

    return 0;
}