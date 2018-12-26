#ifndef _minion_h_
#define _minion_h_

#include <iostream>
#include <thread>

class Minion
{
  private:
    ;

  public:
    Minion();
    Minion(std::string name);
    Minion(const Minion&);
    std::string name;
};

#endif