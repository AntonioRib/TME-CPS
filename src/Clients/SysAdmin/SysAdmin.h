#ifndef _sysadmin_h_
#define _sysadmin_h_

#include <iostream>
#include <string>
#include <thread>

class SysAdmin {
   private:
    ;

   public:
    SysAdmin();
    void saySomething(std::string message);
    std::thread saySomethingThread(std::string message);
};

#endif