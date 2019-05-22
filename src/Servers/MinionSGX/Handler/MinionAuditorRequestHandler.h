#ifndef _minionauditorrequesthandler_h_
#define _minionauditorrequesthandler_h_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>  // for wait()
#include <time.h>
#include <unistd.h>  // for fork()
#include <algorithm>
#include <iostream>
#include <vector>
#include "../Enclave_u.h"
#include "../../../Utilities/SGX_Utils/sgx_utils.h"
#include "../Minion.h"
using namespace std;

class MinionAuditorRequestHandler {
   private:
    Minion* minion;
    string hostname;
    string minionStore;

    bool processAttestation(int monitorSocket, string nonce);

    public:
     MinionAuditorRequestHandler();
     MinionAuditorRequestHandler(Minion* minion);
     static void startMinionAuditorRequestHandler(MinionAuditorRequestHandler* minionAuditorRequestHandler);
};

#endif