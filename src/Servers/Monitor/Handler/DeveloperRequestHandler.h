#ifndef _DEVELOPERREQUESTHANDLER_H_
#define _DEVELOPERREQUESTHANDLER_H_

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>  // for wait()
#include <unistd.h>    // for fork()
#include <algorithm>
#include <iostream>
#include <vector>
#include "../Monitor.h"
using namespace std;

class DeveloperRequestHandler {
        
   private:
    Monitor* monitor;
    string monitorStore;
    string username;
    string sshKey;

    bool sendApp(Minion* minion, string appDir);
    bool deployApp(string appID, int instances);
    bool deleteApp(string appID);

    void processAttestation(int developerSocket); 

   public:
    DeveloperRequestHandler();
    DeveloperRequestHandler(Monitor* monitor);
    static void startDeveloperRequestHandler(DeveloperRequestHandler developerRequestHandler);
};

#endif
