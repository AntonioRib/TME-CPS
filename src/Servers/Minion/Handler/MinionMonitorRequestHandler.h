#ifndef _minionmonitorequesthandler_h_
#define _minionmonitorrequesthandler_h_

#include <iostream>
#include "../Minion.h"
using namespace std;

class MinionMonitorRequestHandler {

   private:
    Minion* minion;
    string hostname;
    string minionStore;

    bool deployApp(string appId);
    bool deleteApp(string appId);

    void processAttestation(string nonce); //TODO

    public:
     MinionMonitorRequestHandler();
     MinionMonitorRequestHandler(Minion* minion);
     static void startMinionMonitorRequestHandler(MinionMonitorRequestHandler minionMonitorRequestHandler);
};

#endif