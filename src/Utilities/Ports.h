#ifndef _PORTS_H_
#define inline 
// #include <iostream>

namespace Ports{

    //RECIEVER_SENDER_PORT

    //Monitor ports
    inline static const int MONITOR_LOCAL_PORT = 2000;
    inline static const int MONITOR_AUDITOR_PORT = 2001;
    inline static const int MONITOR_DEVELOPER_PORT = 2002;
    inline static const int MONITOR_SYSADMIN_PORT = 2003;
    inline static const int MONITOR_AHUB_PORT = 2004;
    inline static const int MONITOR_MINION_PORT = 2005;
    inline static const int MONITOR_MONITOR_PORT = 2006;

    //AuditingHub ports
    inline static const int AHUB_LOCAL_PORT = 6000;
    inline static const int AHUB_AUDITOR_PORT = 6001;
    inline static const int AHUB_DEVELOPER_PORT = 6002;
    inline static const int AHUB_SYSADMIN_PORT = 6003;
    inline static const int AHUB_AHUB_PORT = 6004;
    inline static const int AHUB_MINION_PORT = 6005;
    inline static const int AHUB_MONITOR_PORT = 6005;

    //Minion ports
    inline static const int MINION_LOCAL_PORT = 7000;
    inline static const int MINION_AUDITOR_PORT = 7001;
    inline static const int MINION_DEVELOPER_PORT = 7002;
    inline static const int MINION_SYSADMIN_PORT = 7003;
    inline static const int MINION_AHUB_PORT = 7004;
    inline static const int MINION_MINION_PORT = 7005;
    inline static const int MINION_MONITOR_PORT = 7005;

}
#endif