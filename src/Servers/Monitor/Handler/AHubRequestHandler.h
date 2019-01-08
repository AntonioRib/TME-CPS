#ifndef _AHUBREQUESTHANDLER_H_
#define _AHUBREQUESTHANDLER_H_

#include <algorithm>
#include <iostream>
#include <vector>
#include "../Monitor.h"

class AHubRequestHandler {

   private:
    // Monitor* monitor;
    std::string monitorStore;
    std::string username;
    std::string sshKey;

    bool sendApp(Minion* minion, std::string appDir);
    bool deployAppOnMinion(std::string appId, Minion* host);
    bool spawnReplacementInstances(Minion* untrustedMinion);

    void attestMinion(std::string untrustedMinion);

    public:
     Monitor* monitor;
     AHubRequestHandler();
     AHubRequestHandler(Monitor* monitor);
     static void startAHubRequestHandler(AHubRequestHandler aHubRequestHandler);
};

#endif
