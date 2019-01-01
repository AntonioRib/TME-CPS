#ifndef _ahubrequesthandler_h_
#define _ahubrequesthandler_h_

#include <iostream>
#include "../Minion.h"
using namespace std;

class AHubRequestHandler {
   private:
    Minion* minion;
    string minionStore;

    bool purgeMinion();

   public:
    AHubRequestHandler();
    AHubRequestHandler(Minion* minion);
    static void startAHubRequestHandler(AHubRequestHandler aHubRequestHandler);
};

#endif