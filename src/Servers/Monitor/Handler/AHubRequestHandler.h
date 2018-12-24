#ifndef _ahubrequesthandler_h_
#define _ahubrequesthandler_h_

#include <iostream>
#include "../Monitor.h"

class AHubRequestHandler {
    Monitor monitor;
    
   private:
    ;

   public:
    AHubRequestHandler();
    AHubRequestHandler(Monitor monitor);
    static void startAHubRequestHandler(AHubRequestHandler aHubRequestHandler);
};

#endif
