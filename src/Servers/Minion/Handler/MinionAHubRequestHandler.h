#ifndef _minionahubrequesthandler_h_
#define _minionahubrequesthandler_h_

#include <iostream>
#include "../Minion.h"
using namespace std;

class MinionAHubRequestHandler {
   private:
    Minion* minion;
    string minionStore;

    bool purgeMinion();

   public:
    MinionAHubRequestHandler();
    MinionAHubRequestHandler(Minion* minion);
    static void startMinionAHubRequestHandler(MinionAHubRequestHandler minionAHubRequestHandler);
};

#endif