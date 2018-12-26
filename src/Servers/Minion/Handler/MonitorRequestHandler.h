#ifndef _monitorequesthandler_h_
#define _monitorrequesthandler_h_

#include <iostream>
#include "../Minion.h"

class MonitorRequestHandler {
    Minion minion;
   private:
    ;

   public:
    MonitorRequestHandler();
    MonitorRequestHandler(Minion minion);
    static void startMonitorRequestHandler(MonitorRequestHandler minionRequestHandler);
};

#endif