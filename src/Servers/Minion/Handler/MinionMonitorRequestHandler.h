#ifndef _minionmonitorequesthandler_h_
#define _minionmonitorrequesthandler_h_

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

class MinionMonitorRequestHandler {

   private:
    Minion* minion;
    string hostname;
    string minionStore;

    bool deployApp(string appId);
    bool deleteApp(string appId);

    void processAttestation(int monitorSocket, string nonce);

    public:
     MinionMonitorRequestHandler();
     MinionMonitorRequestHandler(Minion* minion);
     static void startMinionMonitorRequestHandler(MinionMonitorRequestHandler minionMonitorRequestHandler);
};

#endif