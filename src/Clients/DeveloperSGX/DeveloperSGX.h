#ifndef _DEVELOPER_H_
#define _DEVELOPER_H_

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>  // for wait()
#include <unistd.h>  // for fork()
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include "../../Utilities/AttestationConstants.h"
#include "../../Utilities/General.h"
#include "../../Utilities/Messages.h"
#include "../../Utilities/Ports.h"
#include "../../Utilities/TPM.h"
#include "Enclave_u.h"
#include "sgx_urts.h"
#include "../../Utilities/SGX_Utils/sgx_utils.h"
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
using namespace std;

class Developer {
   private:
    string monitorHost;
    string username;
    string key;
    string appDir;
    int instances;

    void attestMonitor(int monitorSocket); //TODO
    bool sendSyncMessageAndGetResponse(string message);
    bool sendApp();

    public: 
    Developer();
    Developer(string monitorHost, string username, string key, string appDir, int instances);
    Developer(string monitorHost, string username, string key, string appDir);
    bool deleteApp();
    bool deployApp();
};

#endif