#ifndef _ports_h_
#define _ports_h_
// #include <iostream>

namespace Ports{

    //Monitor ports
    static const int MONITOR_DEVELOPER_PORT = 2000;
    static const int MONITOR_MINION_PORT = 2001;
    static const int MONITOR_AHUB_PORT = 2002;
    static const int MONITOR_AUDITOR_PORT = 2003;

    //Minion ports
    static const int AHUB_LOCAL_PORT = 6000;
    static const int AHUB_AUDITOR_PORT = 6001;

    //AuditingHub ports
    static const int MINION_AHUB_PORT = 7000;
    static const int MINION_MONITOR_PORT = 7001;
}
#endif