#ifndef _monitorequesthandler_h_
#define _monitorrequesthandler_h_

#include <iostream>
#include "../Minion.h"
using namespace std;

class MonitorRequestHandler {

   private:
    Minion* minion;
    string hostname;
    string minionStore;

    bool deployApp(string appId);
    bool deleteApp(string appId);

    void processAttestation(string nonce); //TODO

    public: 
    MonitorRequestHandler();
    MonitorRequestHandler(Minion* minion);
    static void startMonitorRequestHandler(MonitorRequestHandler minionRequestHandler);
};

#endif