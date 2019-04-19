#ifndef _auditorrequesthandler_h_
#define _auditorrequesthandler_h_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include "../AuditingHub.h"
#include "../../../Utilities/TPM.h"

class AuditorRequestHandler {
    //TODO this class needs to be checked better
    AuditingHub* auditingHub;
   private:
    void processAttestation(int clientSocket, std::string nonce, AuditingHub& auditingHub);

   public: 
    AuditorRequestHandler();
    AuditorRequestHandler(AuditingHub* auditingHub);
    static void startAuditorRequestHandler(AuditorRequestHandler auditorRequestHandler);
};

#endif