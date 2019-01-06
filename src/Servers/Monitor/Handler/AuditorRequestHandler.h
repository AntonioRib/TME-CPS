#ifndef _AUDITORREQUESTHANDLER_H_
#define _AUDITORREQUESTHANDLER_H_

#include <iostream>
#include "../Monitor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class AuditorRequestHandler {
    Monitor* monitor;
    std::string monitorStore;

   private:
     void processAttestation(int socket, std::string nonce, Monitor& monitor);

   public:
    AuditorRequestHandler();
    AuditorRequestHandler(Monitor* monitor);
    static void startAuditorRequestHandler(AuditorRequestHandler auditorRequestHandler);
};

#endif
