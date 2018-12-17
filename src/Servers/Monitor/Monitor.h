#ifndef _monitor_h_
#define _monitor_h_

#include <iostream>
#include <thread>

class Monitor {
   private:
    ;

   protected:
    std::thread startAHubHandler();

   public:
    Monitor();
};

#endif