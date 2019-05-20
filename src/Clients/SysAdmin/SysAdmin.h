#ifndef _SYSADMIN_H_
#define _SYSADMIN_H_

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>  // for wait()
#include <unistd.h>    // for fork()
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include "../../Utilities/AttestationConstants.h"
#include "../../Utilities/General.h"
#include "../../Utilities/Messages.h"
#include "../../Utilities/Ports.h"
using namespace std;

class SysAdmin {
   private:
    string username;
    string hubHost;
    string remoteHost;
    string key;
    string hubSocket;
 
     void attestLogger(int loggetSocket); //TODO
     bool startLocalProxy();

    public:
     SysAdmin();
     SysAdmin(string username, string hubhost, string remoteHost, string key);
     bool manageNode();
     bool urgentManageNode();
     
};

#endif