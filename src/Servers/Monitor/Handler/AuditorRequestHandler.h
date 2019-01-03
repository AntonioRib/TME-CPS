#ifndef _auditorrequesthandler_h_
#define _auditorrequesthandler_h_

#include <iostream>
#include "../../../Utilities/httpLib/httplib.h"
#include "../Monitor.h"

class AuditorRequestHandler {
    Monitor* monitor;
    std::string monitorStore;

   private:
     void processAttestation(httplib::Server& svr, std::string nonce, Monitor& monitor);

   public:
    AuditorRequestHandler();
    AuditorRequestHandler(Monitor* monitor);
    static void startAuditorRequestHandler(AuditorRequestHandler auditorRequestHandler);
};

#endif
