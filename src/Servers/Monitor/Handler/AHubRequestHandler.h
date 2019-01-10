#ifndef _AHUBREQUESTHANDLER_H_
#define _AHUBREQUESTHANDLER_H_

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>  // for wait()
#include <unistd.h>    // for fork()
#include <algorithm>
#include <iostream>
#include <vector>
#include "../Monitor.h"

class AHubRequestHandler {

   private:
    Monitor* monitor;
    std::string monitorStore;
    std::string username;
    std::string sshKey;

    bool sendApp(Minion* minion, std::string appDir);
    bool deployAppOnMinion(Minion* host, string appId);
    bool spawnReplacementInstances(Minion* untrustedMinion);

    bool attestMinion(std::string untrustedMinion);

    public:
   //   Monitor* monitor;
     AHubRequestHandler();
     AHubRequestHandler(Monitor* monitor);
     void setMinionUntrustedOnMonitor(string untrustedMinion);
     static void startAHubRequestHandler(AHubRequestHandler aHubRequestHandler);
};

#endif
