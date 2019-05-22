#ifndef _minionauditorrequesthandler_h_
#define _minionauditorrequesthandler_h_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>  // for wait()
#include <unistd.h>    // for fork()
#include <algorithm>
#include <iostream>
#include <vector>
#include "../Minion.h"
using namespace std;

class MinionAuditorRequestHandler {
   private:
    Minion* minion;
    string hostname;
    string minionStore;

    bool processAttestation(int monitorSocket, string nonce);

    public:
     MinionAuditorRequestHandler();
     MinionAuditorRequestHandler(Minion* minion);
     static void startMinionAuditorRequestHandler(MinionAuditorRequestHandler* minionAuditorRequestHandler);
};

#endif