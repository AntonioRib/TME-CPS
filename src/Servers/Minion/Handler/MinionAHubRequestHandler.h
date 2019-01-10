#ifndef _minionahubrequesthandler_h_
#define _minionahubrequesthandler_h_

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>  // for wait()
#include <unistd.h>    // for fork()
#include <algorithm>
#include <iostream>
#include <vector>
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
    static void startMinionAHubRequestHandler(MinionAHubRequestHandler* minionAHubRequestHandler);
};

#endif