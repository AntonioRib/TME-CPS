#ifndef _ahubrequesthandler_h_
#define _ahubrequesthandler_h_

#include <iostream>
#include "../Monitor.h"

class AHubRequestHandler {

   private:
    Monitor* monitor;
    std::string monitorStore;
    std::string username;
    std::string sshKey;

    bool sendApp(Minion* minion, std::string appDir);
    bool deployAppOnMinion(std::string appId, Minion* host);
    bool spawnReplacementInstances(Minion* untrustedMinion);

    void attestMinion(std::string untrustedMinion);

    public: 
    AHubRequestHandler();
    AHubRequestHandler(Monitor* monitor);
    static void startAHubRequestHandler(AHubRequestHandler aHubRequestHandler);
};

#endif
