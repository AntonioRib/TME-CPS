#ifndef _auditorrequesthandler_h_
#define _auditorrequesthandler_h_

#include <iostream>
#include "../AuditingHub.h"

class AuditorRequestHandler {
    //TODO this class needs to be checked better
    AuditingHub* auditingHub;
   private:
    ;

   public:
    AuditorRequestHandler();
    AuditorRequestHandler(AuditingHub* auditingHub);
    static void startAuditorRequestHandler(AuditorRequestHandler auditorRequestHandler);
};

#endif