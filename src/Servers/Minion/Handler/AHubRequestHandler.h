#ifndef _ahubrequesthandler_h_
#define _ahubrequesthandler_h_

#include <iostream>
#include "../Minion.h"

class AHubRequestHandler {
    Minion minion;
   private:
    ;

   public:
    AHubRequestHandler();
    AHubRequestHandler(Minion minion);
    static void startAHubRequestHandler(AHubRequestHandler aHubRequestHandler);
};

#endif