#ifndef _minionrequesthandler_h_
#define _minionrequesthandler_h_

#include <iostream>
#include "../Monitor.h"

class MinionRequestHandler {
    Monitor* monitor;
    std::string monitorStore;

   private:
    void attestMinion(); //TODO

    public: 
    MinionRequestHandler();
    MinionRequestHandler(Monitor* monitor);
    static void startMinionRequestHandler(MinionRequestHandler minionRequestHandler);
};

#endif
