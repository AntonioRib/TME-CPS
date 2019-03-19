#ifndef _sysadminrequesthandler_h_
#define _sysadminrequesthandler_h_

#include <iostream> 
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <iterator>
#include <sstream>
#include <string>
#include "../Enclave_u.h"
#include "../../../Utilities/SGX_Utils/sgx_utils.h"
#include "../AuditingHub.h"
using namespace std;

class SysAdminRequestHandler {
    //TODO this class needs to be checked better
    AuditingHub* auditingHub;
    string adminUsername;
    string remoteHost;
    int pidAuditingHubToNodeSessionProcess;
    shared_ptr<spdlog::logger> logger;
    int adminToHubSocket;
    int processWrite[2];
    int processRead[2];
    int processReadErr[2];

    private: 
    void launchSessionProcess();
    void launchLogger();
    bool setNodeUntrusted();
    bool purgeMinion();
    bool launchManagementSession();
    void processAttestation(int adminSocket);

   public:
    SysAdminRequestHandler();
    SysAdminRequestHandler(AuditingHub* auditingHub);
    static void startSysAdminRequestHandler(SysAdminRequestHandler sysAdminRequestHandler, int adminToHubSocket);
};

#endif