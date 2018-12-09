#include "Minion.h"

Minion::Minion()
{
    std::cout << "Minion created\n";
}

int main(int argc, char *argv[])
{
    std::cout << "Will try to create Minion\n";

    Minion *minion;
    minion = new Minion();

    std::cout << "Bubye\n";

    return 0;
}