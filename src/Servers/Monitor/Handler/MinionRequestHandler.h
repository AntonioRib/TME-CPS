#ifndef _MINIONREQUESTHANDLER_H_
#define _MINIONREQUESTHANDLER_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include "../Monitor.h"

class MinionRequestHandler {
    Monitor* monitor;
    std::string monitorStore;

   private:
    void attestMinion(int minionSocket); //TODO

    public: 
    MinionRequestHandler();
    MinionRequestHandler(Monitor* monitor);
    static void startMinionRequestHandler(MinionRequestHandler minionRequestHandler);
};

#endif
