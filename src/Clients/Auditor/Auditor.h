#ifndef _auditor_h_
#define _auditor_h_

#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

class Auditor {
   private:
   ;

   public:
    Auditor();
    void saySomething(std::string message);
    void attestMonitor(const char* hostname);
};

#endif