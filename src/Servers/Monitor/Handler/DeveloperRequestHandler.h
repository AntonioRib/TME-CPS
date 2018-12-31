#ifndef _developerrequesthandler_h_
#define _developerrequesthandler_h_

#include <iostream>
#include "../Monitor.h"

class DeveloperRequestHandler {
    Monitor* monitor;
    
   private:
    ;

   public:
    DeveloperRequestHandler();
    DeveloperRequestHandler(Monitor* monitor);
    static void startDeveloperRequestHandler(DeveloperRequestHandler developerRequestHandler);
};

#endif
