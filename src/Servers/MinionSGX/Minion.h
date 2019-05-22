#ifndef _MINION_H_
#define _MINION_H_

#include <iostream>
#include <map>
#include <thread>
#include <utility>
#include <vector>
#include "../../Utilities/AttestationConstants.h"
#include "../../Utilities/General.h"
#include "../../Utilities/Messages.h"
#include "../../Utilities/Ports.h"
#include "../Application/Application.h"
#include "Enclave_u.h"
#include "../../Utilities/SGX_Utils/sgx_utils.h"

using namespace std;

class Minion
{
    private:
    string ipAddress;
    string monitorHost;
    string hostname;
    map<string, Application*> minionApplications;

    void processAttestation(int monitorSocket);
    bool startMonitorHandler();
    bool startAuditingHubHandler();
    bool startAuditorHandler();

   public:
    Minion();
    Minion(string monitorHost, string hostname, string ipAddress);
    Minion(const Minion&);

    //Getters
    string getMonitorHost();
    string getHostname();
    string getIpAddress();
};

#endif