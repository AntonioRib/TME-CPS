#ifndef _PORTSSGX_H_
#define _PORTSSGX_H_

namespace PortsSGX{

    //RECIEVER_SENDER_PORT

    //Monitor ports
     static const int MONITOR_LOCAL_PORT = 2000;
     static const int MONITOR_AUDITOR_PORT = 2001;
     static const int MONITOR_DEVELOPER_PORT = 2002;
     static const int MONITOR_SYSADMIN_PORT = 2003;
     static const int MONITOR_AHUB_PORT = 2004;
     static const int MONITOR_MINION_PORT = 2005;
     static const int MONITOR_MONITOR_PORT = 2006;

    //AuditingHub ports
     static const int AHUB_LOCAL_PORT = 6000;
     static const int AHUB_AUDITOR_PORT = 6001;
     static const int AHUB_DEVELOPER_PORT = 6002;
     static const int AHUB_SYSADMIN_PORT = 6003;
     static const int AHUB_AHUB_PORT = 6004;
     static const int AHUB_MINION_PORT = 6005;
     static const int AHUB_MONITOR_PORT = 6005;

    //Minion ports
     static const int MINION_LOCAL_PORT = 7000;
     static const int MINION_AUDITOR_PORT = 7001;
     static const int MINION_DEVELOPER_PORT = 7002;
     static const int MINION_SYSADMIN_PORT = 7003;
     static const int MINION_AHUB_PORT = 7004;
     static const int MINION_MINION_PORT = 7005;
     static const int MINION_MONITOR_PORT = 7005;

     static const int ADMIN_SSH_PORT = 5000;
}
#endif