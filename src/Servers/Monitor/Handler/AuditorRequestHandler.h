#ifndef _auditorrequesthandler_h_
#define _auditorrequesthandler_h_

#include <iostream>
// #include "../../../Utilities/httpLib/httplib.h"
#include "../Monitor.h"
#include "../../../Utilities/Messages.h"
#include "../../../Utilities/AttestationConstants.h"
#include "../../../Utilities/Ports.h"
#include "../../../Utilities/General.h"
// #include "../../../Utilities/httpLib/httplib.h"
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
