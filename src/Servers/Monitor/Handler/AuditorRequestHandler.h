#ifndef _auditorrequesthandler_h_
#define _auditorrequesthandler_h_

#include <iostream>
#include "../Monitor.h"

class AuditorRequestHandler {
    Monitor monitor;

   private:
    ;

   public:
    AuditorRequestHandler();
    AuditorRequestHandler(Monitor monitor);
    static void startAuditorRequestHandler(AuditorRequestHandler auditorRequestHandler);
};

#endif
