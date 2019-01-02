#ifndef _sysadminrequesthandler_h_
#define _sysadminrequesthandler_h_

#include <iostream>
#include "../AuditingHub.h"

class SysAdminRequestHandler {
    //TODO this class needs to be checked better
    AuditingHub* auditingHub;
   private:
    ;

   public:
    SysAdminRequestHandler();
    SysAdminRequestHandler(AuditingHub* auditingHub);
    static void startSysAdminRequestHandler(SysAdminRequestHandler sysAdminRequestHandler);
};

#endif