#ifndef _DEVELOPERREQUESTHANDLER_H_
#define _DEVELOPERREQUESTHANDLER_H_

#include <iostream>
#include "../Monitor.h"

class DeveloperRequestHandler {
        
   private:
    Monitor* monitor;
    std::string monitorStore;
    std::string username;
    std::string sshKey;

    bool sendApp(Minion* minion, std::string appDir);
    bool deployApp(std::string appID, int instances);
    bool deleteApp(std::string appID);

    void processAttestation(); //TODO

   public:
    DeveloperRequestHandler();
    DeveloperRequestHandler(Monitor* monitor);
    static void startDeveloperRequestHandler(DeveloperRequestHandler developerRequestHandler);
};

#endif
